#!/usr/bin/env python3
"""Activity recreation must keep native state but drop obsolete UI ownership.

The host Java fixture embeds the unchanged rebindContext() and
onViewReplaced() bodies from production source, with only Android classes
replaced by minimal stubs. No emulator or Android SDK is required.
"""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"


def source(filename):
    return (JAVA / filename).read_text(encoding="utf-8")


def method(source_text, signature):
    start = source_text.index(signature)
    brace = source_text.index("{", start)
    depth = 1
    for i in range(brace + 1, len(source_text)):
        if source_text[i] == "{":
            depth += 1
        elif source_text[i] == "}":
            depth -= 1
            if depth == 0:
                return source_text[start:i + 1]
    raise AssertionError("Unclosed Java method: " + signature)


STATE = source("StateManager.java")
ACTIVITY = source("GameActivity.java")
VIEW = source("TermView.java")
THREAD = source("GameThread.java")
NATIVE = source("NativeWrapper.java")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

REBIND = method(STATE, "public void rebindContext(GameActivity activity) {")
REPLACED = method(VIEW, "public void onViewReplaced() {")
UNLINK_IF_LINKED = method(NATIVE, "public boolean unlinkIfLinked(TermView retired) {")


class ActivityRecreationSourceTest(unittest.TestCase):
    def test_recycled_state_rebinds_current_activity(self):
        self.assertIn("public static StateManager state = null;", ACTIVITY)
        constructor = ACTIVITY.split("public void onCreate(Bundle savedInstanceState)", 1)[1]
        block = constructor.split("public boolean shouldAdjustByWidth()", 1)[0]
        self.assertIn("if (state == null)", block)
        self.assertIn("state = new StateManager(this);", block)
        self.assertIn("state.rebindContext(this);", block)
        self.assertLess(block.index("state.rebindContext(this);"),
                        block.index("new AngbandDialog(this,state)"))
        self.assertIn("public volatile GameActivity context = null;", STATE)
        self.assertIn("context = activity;", REBIND)
        self.assertIn("if (activity == null)", REBIND)
        self.assertNotIn("new GameThread(", REBIND)
        self.assertNotIn("nativew.wipeAll()", REBIND)
        self.assertNotIn("gameThread.stop(", REBIND)
        self.assertIn("return AppLanguageContext.usesTraditionalChinese(state.context);", NATIVE)

    def test_view_rebuild_revokes_old_gestures_without_native_reset(self):
        self.assertIn("term.onViewReplaced();", ACTIVITY)
        rebuild = ACTIVITY.split("protected void rebuildViews()", 1)[1].split(
            "public void openContextMenu()", 1
        )[0]
        self.assertLess(rebuild.index("term.onViewReplaced();"),
                        rebuild.index("term.unloadTiles();"))
        self.assertLess(rebuild.index("term.onViewReplaced();"),
                        rebuild.index("term = new TermView(this);"))
        for fragment in (
            "state.invalidateInputLifecycleEpoch();",
            "screenTapGuard.abortGesture();",
            "stopTimers();",
        ):
            self.assertIn(fragment, REPLACED)
        self.assertLess(REPLACED.index("state.invalidateInputLifecycleEpoch();"),
                        REPLACED.index("screenTapGuard.abortGesture();"))
        self.assertLess(REPLACED.index("screenTapGuard.abortGesture();"),
                        REPLACED.index("stopTimers();"))
        self.assertNotIn("SaveGame", REPLACED)
        self.assertNotIn("Term_discard_saved_screens", REPLACED)

    def test_destroy_only_detaches_owned_view(self):
        destroy = method(ACTIVITY, "public void onDestroy()")
        self.assertIn("state.nativew.unlinkIfLinked(term)", destroy)
        self.assertIn("term.onViewReplaced();", destroy)
        self.assertLess(destroy.index("state.nativew.unlinkIfLinked(term)"),
                        destroy.index("term.onViewReplaced();"))
        self.assertIn("if (term != retired) return false;", UNLINK_IF_LINKED)
        self.assertIn("term = null;", UNLINK_IF_LINKED)
        self.assertIn("return true;", UNLINK_IF_LINKED)
        self.assertIn("synchronized (display_lock)", UNLINK_IF_LINKED)
        self.assertNotIn("gameThread.send", destroy)
        self.assertNotIn("Term_discard_saved_screens", destroy)

    def test_pause_resume_does_not_discard_snapshot_stack(self):
        pause = method(VIEW, "public void onPause() {")
        resume = method(VIEW, "public void onResume() {")
        self.assertIn("state.gameThread.send(GameThread.Request.SaveGame);", pause)
        for block in (pause, resume):
            self.assertIn("state.invalidateInputLifecycleEpoch();", block)
            self.assertNotIn("screen_reset_overlay_state", block)
            self.assertNotIn("Term_discard_saved_screens", block)
            self.assertNotIn("state = new StateManager", block)
        self.assertIn("state.nativew.resize();", THREAD)
        self.assertIn("state.link(term, handler);", ACTIVITY)
        self.assertIn("test_android_activity_recreation_source.py", VERIFY)

    def test_production_java_methods_run_against_host_stubs(self):
        if not shutil.which("javac") or not shutil.which("java"):
            self.skipTest("Host JDK not installed")
        fixture = """
public final class ActivityRecreationSmoke {
    static class GameActivity {
        final int id;
        GameActivity(int id) { this.id = id; }
    }
    static class GameThread {}
    static class StateManager {
        volatile GameActivity context;
        final GameThread gameThread = new GameThread();
        int invalidations;
        StateManager(GameActivity activity) { context = activity; }
        void invalidateInputLifecycleEpoch() { ++invalidations; }
        REBIND_HERE
    }
    static class TapGuard {
        int aborted;
        void abortGesture() { ++aborted; }
    }
    static class TermView {
        StateManager state;
        final TapGuard screenTapGuard = new TapGuard();
        int stoppedTimers;
        TermView(StateManager state) { this.state = state; }
        void stopTimers() { ++stoppedTimers; }
        VIEW_REPLACED_HERE
    }
    static class NativeWrapper {
        final Object display_lock = new Object();
        TermView term;
        void link(TermView t) { synchronized (display_lock) { term = t; } }
        UNLINK_IF_LINKED_HERE
    }
    static void check(boolean yes) {
        if (!yes) throw new AssertionError("Activity recreation regression");
    }
    public static void main(String[] args) {
        GameActivity original = new GameActivity(1);
        StateManager state = new StateManager(original);
        GameThread nativeThread = state.gameThread;
        TermView oldView = new TermView(state);
        oldView.onViewReplaced();
        check(oldView.stoppedTimers == 1);
        check(oldView.screenTapGuard.aborted == 1);
        check(state.invalidations == 1);
        GameActivity recreated = new GameActivity(2);
        state.rebindContext(recreated);
        check(state.context == recreated);
        check(state.context != original);
        check(state.gameThread == nativeThread);
        boolean refused = false;
        try { state.rebindContext(null); }
        catch (IllegalArgumentException e) { refused = true; }
        check(refused && state.context == recreated);
        TermView nextView = new TermView(state);
        NativeWrapper wrapper = new NativeWrapper();
        wrapper.link(oldView);
        check(wrapper.unlinkIfLinked(oldView));
        check(wrapper.term == null);
        wrapper.link(nextView);
        check(!wrapper.unlinkIfLinked(oldView));
        check(wrapper.term == nextView);
        // A delayed onDestroy from the prior Activity must not touch
        // the live replacement's epoch, timers or native output owner.
        check(state.invalidations == 1);
        check(nextView.screenTapGuard.aborted == 0);
        check(nextView.stoppedTimers == 0);
        nextView.onViewReplaced();
        check(state.invalidations == 2);
        check(nextView.screenTapGuard.aborted == 1);
        check(oldView.screenTapGuard.aborted == 1);
        check(wrapper.unlinkIfLinked(nextView));
        check(wrapper.term == null);
        check(state.gameThread == nativeThread);
        System.out.println("PASS: actual Activity rebind and view replacement methods");
    }
}
""".replace("REBIND_HERE", REBIND).replace("VIEW_REPLACED_HERE", REPLACED).replace(
    "UNLINK_IF_LINKED_HERE", UNLINK_IF_LINKED)
        with tempfile.TemporaryDirectory() as temp:
            src = Path(temp) / "ActivityRecreationSmoke.java"
            src.write_text(fixture, encoding="utf-8")
            subprocess.run(["javac", "-encoding", "UTF-8", str(src)],
                           cwd=temp, check=True, capture_output=True, text=True)
            result = subprocess.run(["java", "-cp", temp, "ActivityRecreationSmoke"],
                                    cwd=temp, check=True, capture_output=True, text=True)
            self.assertIn("PASS: actual Activity rebind", result.stdout)


if __name__ == "__main__":
    unittest.main()
