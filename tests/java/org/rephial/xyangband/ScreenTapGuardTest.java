package org.rephial.xyangband;

/** Host-JDK regression tests for screen-only multi-pointer tap suppression. */
public final class ScreenTapGuardTest {
    private static int checks = 0;

    private static void expect(boolean success, String message) {
        checks++;
        if (!success) throw new AssertionError(message);
    }

    public static void main(String[] args) {
        ScreenTapGuard guard = new ScreenTapGuard();

        guard.begin(true);
        expect(!guard.isCancelled(), "single pointer starts clean");
        expect(guard.finish(true), "ordinary screen tap accepted");
        expect(!guard.isCancelled(), "completed tap resets");

        guard.begin(true);
        expect(guard.cancelScreenGesture(), "second pointer cancels screen");
        expect(guard.isCancelled(), "cancellation persists on pointer removal");
        expect(!guard.finish(true), "final up after multi touch rejected");
        expect(!guard.isCancelled(), "finish resets cancellation");

        guard.begin(true);
        expect(guard.cancelScreenGesture(), "cancel event marks screen touch");
        expect(!guard.finish(true), "up after cancel rejected");
        guard.begin(true);
        expect(guard.finish(true), "next clean screen tap recovers");

        guard.begin(true);
        expect(!guard.finish(false), "drag from screen onto overlay rejected");
        guard.begin(true);
        expect(guard.finish(true), "following clean screen tap accepted");

        guard.begin(false);
        expect(!guard.cancelScreenGesture(), "overlay pointer unaffected");
        expect(!guard.isCancelled(), "overlay remains unblocked");
        expect(guard.finish(false), "D-pad/overlay release unaffected");

        guard.begin(true);
        expect(guard.cancelScreenGesture(), "long press cancels release tap");
        expect(!guard.finish(true), "long press up not also a single tap");

        guard.begin(true);
        guard.cancelScreenGesture();
        guard.begin(true);
        expect(guard.finish(true), "new down clears stale cancelled touch");

        /* A view can receive orphan ACTION_UP after Activity pause,
         * focus loss, rotation, CANCEL or an already-finished gesture. */
        expect(!guard.finish(true), "orphan screen release rejected");
        expect(!guard.finish(false), "orphan overlay release rejected");

        guard.begin(true);
        guard.abortGesture(); // TermView.onPause()
        expect(!guard.finish(true), "release after pause rejected");
        expect(!guard.isCancelled(), "pause leaves no cancelled ghost");

        guard.begin(true);
        guard.abortGesture(); // TermView.onWindowFocusChanged(false)
        expect(!guard.finish(true), "release after focus loss rejected");
        guard.abortGesture(); // TermView.onResume()
        expect(!guard.finish(true), "release after resume still rejected");

        guard.begin(false);
        guard.abortGesture();
        expect(!guard.finish(false), "overlay release after pause rejected");
        guard.begin(false);
        expect(guard.finish(false), "fresh overlay down/up still works");

        guard.begin(true);
        guard.cancelScreenGesture(); // second pointer or long press
        guard.abortGesture(); // focus or pause during cancelled gesture
        expect(!guard.finish(true), "cancelled sequence cannot resurrect");
        guard.begin(true);
        expect(guard.finish(true), "next fresh single-finger tap works");
        expect(!guard.finish(true), "duplicate ACTION_UP not a second tap");

        guard.begin(true);
        guard.abortGesture(); // explicit ACTION_CANCEL
        expect(!guard.finish(true), "cancel without final up stays rejected");
        guard.begin(true);
        expect(guard.finish(true), "fresh down after cancellation works");

        guard.begin(false);
        expect(!guard.cancelScreenGesture(),
            "ordinary overlay remains exempt from screen-only cancel");
        expect(guard.finish(false), "overlay release remains usable");
        expect(!guard.finish(false), "overlay duplicate up not executed");

        System.out.println("PASS: screen-only touch cancellation (" +
            checks + " assertions)");
    }
}
