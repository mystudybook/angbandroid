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

        System.out.println("PASS: screen-only touch cancellation (" +
            checks + " assertions)");
    }
}
