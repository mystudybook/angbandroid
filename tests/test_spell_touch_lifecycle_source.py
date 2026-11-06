#!/usr/bin/env python3
"""Source contracts for the Android UI -> JNI -> native spell touch epoch."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


def source(path):
    return (ROOT / path).read_text(encoding="utf-8")


STATE = source("app/src/main/java/org/rephial/xyangband/StateManager.java")
EPOCH = source("app/src/main/java/org/rephial/xyangband/InputLifecycleEpoch.java")
NATIVE = source("app/src/main/java/org/rephial/xyangband/NativeWrapper.java")
TERM = source("app/src/main/java/org/rephial/xyangband/TermView.java")
CURSES = source("app/src/main/cpp/curses/curses.c")
DROID = source("app/src/main/cpp/curses/droid.h")
SPELL = source("app/src/main/cpp/angband/ui-spell.c")
HEADER = source("app/src/main/cpp/angband/zh-tw-spell-reader-pages.h")
C_TEST = source("tests/native/test_spell_reader_pages.c")
JAVA_TEST = source("tests/java/org/rephial/xyangband/InputLifecycleEpochTest.java")
JAVA_RUNNER = source("scripts/test-terminal-unicode.sh")
VERIFY = source("scripts/verify-zh-tw.sh")


class SpellTouchLifecycleSourceTest(unittest.TestCase):
    def test_epoch_is_atomic_and_never_a_gameplay_key(self):
        self.assertIn("private final AtomicInteger value = new AtomicInteger(1);", EPOCH)
        self.assertIn("value.compareAndSet(old, next)", EPOCH)
        self.assertIn("old == Integer.MAX_VALUE ? 1 : old + 1", EPOCH)
        self.assertIn("new InputLifecycleEpoch()", STATE)
        self.assertIn("public int getInputLifecycleEpoch()", STATE)
        self.assertIn("public void invalidateInputLifecycleEpoch()", STATE)
        self.assertIn("return inputLifecycleEpoch.current();", STATE)
        self.assertIn("inputLifecycleEpoch.invalidate();", STATE)
        self.assertIn("int getInputLifecycleEpoch()", NATIVE)
        self.assertIn("return state.getInputLifecycleEpoch();", NATIVE)
        self.assertNotIn("state.addKey(", EPOCH)
        self.assertNotIn("keyBuffer", EPOCH)

    def test_pause_resume_and_focus_loss_publish_epochs(self):
        focus = TERM.split(
            "public void onWindowFocusChanged(boolean hasWindowFocus)", 1
        )[1].split("public void onPause()", 1)[0]
        pause = TERM.split("public void onPause()", 1)[1].split(
            "public static int directionKeyFromIndex", 1
        )[0]
        resume = TERM.split("public void onResume()", 1)[1].split(
            "public void onWindowFocusChanged", 1
        )[0]
        self.assertIn("if (state != null) state.invalidateInputLifecycleEpoch();",
                      focus)
        self.assertIn("if (!hasWindowFocus)", focus)
        for section in (pause, resume):
            self.assertIn("state.invalidateInputLifecycleEpoch();", section)
            self.assertLess(section.index("state.invalidateInputLifecycleEpoch();"),
                            section.index("screenTapGuard.abortGesture();"))
        self.assertLess(pause.index("state.invalidateInputLifecycleEpoch();"),
                        pause.index("state.gameThread.send(GameThread.Request.SaveGame);"))

    def test_jni_caches_readonly_epoch_not_special_key(self):
        self.assertIn("extern int angdroid_input_lifecycle_epoch(void);", DROID)
        self.assertIn("int angdroid_input_lifecycle_epoch(void) {", CURSES)
        self.assertIn('JAVA_METHOD("getInputLifecycleEpoch", "()I")', CURSES)
        self.assertIn("JAVA_CALL_INT(NativeWrapper_get_input_lifecycle_epoch)", CURSES)
        self.assertIn("(*env)->ExceptionCheck(env)", CURSES)
        self.assertIn("(*env)->ExceptionClear(env)", CURSES)
        self.assertIn("return -1;", CURSES)
        jni = CURSES.split("int angdroid_input_lifecycle_epoch(void) {", 1)[1]
        jni = jni.split("int mvinch(", 1)[0]
        self.assertNotIn("angdroid_getch(", jni)
        self.assertNotIn("Term_keypress(", jni)
        self.assertNotIn("state.addKey(", jni)

    def test_native_picker_discards_epoch_changes_before_touch(self):
        self.assertIn("int touch_lifecycle_epoch;", SPELL)
        self.assertIn("d->touch_lifecycle_epoch = angdroid_input_lifecycle_epoch();",
                      SPELL)
        self.assertIn("spell_menu_sync_touch_epoch(struct spell_menu_data *d)",
                      SPELL)
        self.assertIn("angdroid_spell_menu_touch_epoch_changed(", SPELL)
        handler = SPELL.split("static bool spell_menu_handler(", 1)[1].split(
            "static void spell_menu_browser(", 1
        )[0]
        self.assertIn("bool lifecycle_changed = spell_menu_sync_touch_epoch(d);",
                      handler)
        self.assertIn("if (lifecycle_changed) return true;", handler)
        self.assertLess(handler.index("if (lifecycle_changed) return true;"),
                        handler.index("angdroid_spell_menu_confirm_touch("))
        self.assertIn("d->selected_spell = d->spells[oid];", handler)
        self.assertEqual(1, handler.count("d->selected_spell = d->spells[oid];"))
        browser = SPELL.split("static void spell_menu_browser(", 1)[1].split(
            "static const menu_iter", 1
        )[0]
        self.assertIn("(void)spell_menu_sync_touch_epoch(d);", browser)
        self.assertIn("ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS", HEADER)
        self.assertIn("angdroid_spell_menu_cancel_touch(armed_oid, armed_at_ms);",
                      HEADER)
        self.assertIn("if (observed_epoch <= 0)", HEADER)
        self.assertIn("if (observed_epoch == *known_epoch) return 0;", HEADER)
        self.assertIn("*known_epoch = observed_epoch;", HEADER)
        self.assertIn("if (!armed_oid || !armed_at_ms || !known_epoch) return 1;",
                      HEADER)
        self.assertIn("/* onPause */", C_TEST)
        self.assertIn("/* onResume */", C_TEST)

    def test_tests_registered_without_github_actions(self):
        self.assertIn("InputLifecycleEpoch.java", JAVA_RUNNER)
        self.assertIn("InputLifecycleEpochTest.java", JAVA_RUNNER)
        self.assertIn("org.rephial.xyangband.InputLifecycleEpochTest",
                      JAVA_RUNNER)
        self.assertIn("tests/test_spell_touch_lifecycle_source.py", VERIFY)
        self.assertIn("concurrent generations are not lost", JAVA_TEST)
        self.assertIn("focus loss advances", JAVA_TEST)
        self.assertIn("resume advances", JAVA_TEST)


if __name__ == "__main__":
    unittest.main()
