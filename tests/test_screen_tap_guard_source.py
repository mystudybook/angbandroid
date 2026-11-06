#!/usr/bin/env python3
"""Guard Android screen taps against cancelled, long-press, and multi-touch streams."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
JAVA = (ROOT / "app/src/main/java/org/rephial/xyangband/TermView.java").read_text(
    encoding="utf-8")
GUARD = (ROOT / "app/src/main/java/org/rephial/xyangband/ScreenTapGuard.java").read_text(
    encoding="utf-8")
TEST = (ROOT / "tests/java/org/rephial/xyangband/ScreenTapGuardTest.java").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-terminal-unicode.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class ScreenTapGuardSourceTest(unittest.TestCase):
    def test_guard_is_screen_only(self):
        self.assertIn("private final ScreenTapGuard screenTapGuard = new ScreenTapGuard();",
                      JAVA)
        event = JAVA.split("public boolean onTouchEvent(MotionEvent me)", 1)[1]
        self.assertIn("getViewByXY(me.getY(), me.getX()) instanceof ScreenView",
                      event)
        self.assertIn("screenTapGuard.begin(", event)
        self.assertIn("screenTapGuard.cancelScreenGesture()", event)
        self.assertIn("screenTapGuard.isCancelled()", event)
        self.assertIn("screenTapGuard.finish(", event)
        self.assertIn("me.getActionMasked()", event)
        self.assertIn("MotionEvent.ACTION_POINTER_DOWN", event)
        self.assertIn("MotionEvent.ACTION_POINTER_UP", event)
        self.assertIn("me.getPointerCount() > 1", event)
        self.assertIn("MotionEvent.ACTION_CANCEL", event)
        self.assertIn("MotionEvent.ACTION_UP", event)
        self.assertIn("stopTimers();", event)

        # Suppression happens before the Android bottom-corner shortcut.
        self.assertLess(event.index("screenTapGuard.isCancelled()"),
                        event.index("// Display quick settings"))
        self.assertLess(event.index("screenTapGuard.finish("),
                        event.index("// Display quick settings"))

    def test_long_press_does_not_turn_into_normal_screen_tap(self):
        timer = JAVA.split("if (msg.what == LONG_PRESS) {", 1)[1]
        self.assertLess(timer.index("screenTapGuard.cancelScreenGesture();"),
                        timer.index("lastView.executeLongPress(null);"))
        self.assertIn("if (maskedAction == MotionEvent.ACTION_UP)", JAVA)
        self.assertIn("screenTapGuard.finish(false);", JAVA)

    def test_rotation_cancels_pending_screen_tap(self):
        resize = JAVA.split("protected void onSizeChanged(int w, int h, int oldw, int oldh)", 1)[1]
        self.assertLess(resize.index("screenTapGuard.cancelScreenGesture();"),
                        resize.index('Log.d("Angband", "onSizeChanged");'))

    def test_helper_preserves_virtual_controls(self):
        self.assertIn("private boolean activeGesture;", GUARD)
        self.assertIn("activeGesture = true;", GUARD)
        self.assertIn("if (!activeGesture || !screenGesture) return false;", GUARD)
        self.assertIn("boolean accept = activeGesture &&", GUARD)
        self.assertIn("(!screenGesture || (!cancelled && releasedOnScreen));",
                      GUARD)
        self.assertIn("screenGesture = onScreen;", GUARD)
        self.assertIn("cancelled = false;", GUARD)
        self.assertIn("activeGesture && screenGesture && cancelled", GUARD)
        self.assertIn("activeGesture = false;", GUARD)
        self.assertIn("screenGesture = false;", GUARD)
        self.assertIn("void begin(boolean onScreen)", GUARD)
        self.assertIn("void abortGesture()", GUARD)
        self.assertIn("boolean finish(boolean releasedOnScreen)", GUARD)
        self.assertIn("abortGesture();", GUARD)

    def test_pause_resume_and_focus_loss_cancel_before_saving(self):
        lifecycle = JAVA.split("public void onResume() {", 1)[1]
        lifecycle = lifecycle.split(
            "public static int directionKeyFromIndex(int idx)", 1)[0]
        self.assertIn("screenTapGuard.abortGesture();", lifecycle)
        self.assertIn("public void onWindowFocusChanged(boolean hasWindowFocus)",
                      lifecycle)
        focus = lifecycle.split("public void onWindowFocusChanged(", 1)[1]
        focus = focus.split("public void onPause() {", 1)[0]
        self.assertIn("super.onWindowFocusChanged(hasWindowFocus);", focus)
        self.assertIn("if (!hasWindowFocus) {", focus)
        self.assertIn("screenTapGuard.abortGesture();", focus)
        self.assertIn("stopTimers();", focus)

        pause = lifecycle.split("public void onPause() {", 1)[1]
        self.assertLess(pause.index("screenTapGuard.abortGesture();"),
                        pause.index("stopTimers();"))
        self.assertLess(pause.index("stopTimers();"),
                        pause.index(
                            "state.gameThread.send(GameThread.Request.SaveGame);"))
        # Saving already clears the native input queue; do not change the
        # global queue or drop unrelated game input at resume.
        KEYBUFFER = (ROOT / "app/src/main/java/org/rephial/xyangband/KeyBuffer.java"
                     ).read_text(encoding="utf-8")
        saved = KEYBUFFER.split("public void signalSave() {", 1)[1]
        saved = saved.split("public void addSpecialCommand(", 1)[0]
        self.assertLess(saved.index("keybuffer.clear();"),
                        saved.index("keybuffer.offer(-1);"))
        self.assertNotIn("state.clearKeys();", lifecycle)

    def test_cancel_aborts_any_gesture_and_rejects_orphan_release(self):
        event = JAVA.split("public boolean onTouchEvent(MotionEvent me)", 1)[1]
        event = event.split("public boolean onScroll(", 1)[0]
        cancelled = event.split(
            "} else if (maskedAction == MotionEvent.ACTION_CANCEL) {", 1)[1]
        cancelled = cancelled.split("if (screenTapGuard.isCancelled())", 1)[0]
        self.assertIn("screenTapGuard.abortGesture();", cancelled)
        self.assertIn("stopTimers();", cancelled)
        self.assertIn("return true;", cancelled)
        self.assertIn("!screenTapGuard.finish(", event)
        self.assertLess(event.index("!screenTapGuard.finish("),
                        event.index("// Display quick settings"))
        self.assertIn("void abortGesture()", GUARD)
        self.assertIn("boolean accept = activeGesture &&", GUARD)

    def test_java_regressions_and_offline_registration(self):
        for token in (
            "ordinary screen tap accepted",
            "second pointer cancels screen",
            "final up after multi touch rejected",
            "up after cancel rejected",
            "drag from screen onto overlay rejected",
            "D-pad/overlay release unaffected",
            "long press up not also a single tap",
            "new down clears stale cancelled touch",
            "orphan screen release rejected",
            "orphan overlay release rejected",
            "release after pause rejected",
            "release after focus loss rejected",
            "release after resume still rejected",
            "overlay release after pause rejected",
            "duplicate ACTION_UP not a second tap",
            "fresh down after cancellation works",
        ):
            with self.subTest(token=token):
                self.assertIn(token, TEST)
        self.assertIn("ScreenTapGuard.java", RUNNER)
        self.assertIn("ScreenTapGuardTest.java", RUNNER)
        self.assertIn("org.rephial.xyangband.ScreenTapGuardTest", RUNNER)
        self.assertIn("tests/test_screen_tap_guard_source.py", VERIFY)


if __name__ == "__main__":
    unittest.main()
