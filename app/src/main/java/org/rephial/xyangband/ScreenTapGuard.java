package org.rephial.xyangband;

/**
 * Guard a terminal ScreenView tap against an aborted Android touch stream.
 *
 * Android may send ACTION_POINTER_DOWN/UP before the primary ACTION_UP.
 * Only single-finger screen taps may reach the game's native mouse queue.
 * Virtual D-pad and other overlays retain their existing repeat behavior.
 */
final class ScreenTapGuard {
    private boolean activeGesture;
    private boolean screenGesture;
    private boolean cancelled;

    void begin(boolean onScreen) {
        activeGesture = true;
        screenGesture = onScreen;
        cancelled = false;
    }

    /** Mark the current screen gesture as no longer a valid single tap. */
    boolean cancelScreenGesture() {
        if (!activeGesture || !screenGesture) return false;
        cancelled = true;
        return true;
    }

    boolean isCancelled() {
        return activeGesture && screenGesture && cancelled;
    }

    /** End a pending gesture without emitting a mouse/keyboard action. */
    void abortGesture() {
        activeGesture = false;
        screenGesture = false;
        cancelled = false;
    }

    /** Reject orphan ACTION_UP events that have no preceding ACTION_DOWN. */
    boolean finish(boolean releasedOnScreen) {
        boolean accept = activeGesture &&
            (!screenGesture || (!cancelled && releasedOnScreen));
        abortGesture();
        return accept;
    }
}
