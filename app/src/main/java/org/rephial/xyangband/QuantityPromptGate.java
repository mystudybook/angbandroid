package org.rephial.xyangband;

/**
 * Identifies the currently active native quantity prompt.
 * All callers must synchronize on this object when combining a token
 * check with an input-queue write.
 */
final class QuantityPromptGate {
    private int serial = 0;
    private int active = 0;
    private int latest = 0;

    synchronized int open() {
        ++serial;
        if (serial == 0) ++serial;
        active = serial;
        latest = serial;
        return active;
    }

    synchronized int close() {
        active = 0;
        return latest;
    }

    synchronized boolean isActive(int token) {
        return token != 0 && token == active;
    }

    synchronized boolean resolve(int token) {
        if (!isActive(token)) return false;
        active = 0;
        return true;
    }
}
