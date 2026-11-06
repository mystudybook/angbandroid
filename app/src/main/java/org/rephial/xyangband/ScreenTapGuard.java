package org.rephial.xyangband;

/**
 * Guard a terminal ScreenView tap against an aborted Android touch stream.
 *
 * Android may send ACTION_POINTER_DOWN/UP before the primary ACTION_UP.
 * Only single-finger screen taps may reach the game's native mouse queue.
 * Virtual D-pad and other overlays retain their existing repeat behavior.
 */
final class ScreenTapGuard {
    private boolean screenGesture;
    private boolean cancelled;

    void begin(boolean onScreen) {
        screenGesture = onScreen;
        cancelled = false;
    }

    /** Mark the current screen gesture as no longer a valid single tap. */
    boolean cancelScreenGesture() {
        if (!screenGesture) return false;
        cancelled = true;
        return true;
    }

    boolean isCancelled() {
        return screenGesture && cancelled;
    }

    /** Called on the final ACTION_UP, before any mouse press or shortcut. */
    boolean finish(boolean releasedOnScreen) {
        boolean accept = !screenGesture || (!cancelled && releasedOnScreen);
        screenGesture = false;
        cancelled = false;
        return accept;
    }
}
