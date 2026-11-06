package org.rephial.xyangband;

import java.util.concurrent.atomic.AtomicInteger;

/**
 * Cross-thread lifecycle generation for touch input, not game commands.
 *
 * The Android UI thread changes the generation when the view pauses,
 * resumes or loses focus; the native game thread reads it through JNI.
 * No special key is inserted into the Angband input queue.
 */
final class InputLifecycleEpoch {
    private final AtomicInteger value = new AtomicInteger(1);

    int current() {
        return value.get();
    }

    int invalidate() {
        for (;;) {
            int old = value.get();
            int next = old == Integer.MAX_VALUE ? 1 : old + 1;
            if (value.compareAndSet(old, next)) return next;
        }
    }
}
