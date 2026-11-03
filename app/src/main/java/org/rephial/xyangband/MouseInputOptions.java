package org.rephial.xyangband;

/** Stable popup option IDs; visible labels may be localized independently. */
final class MouseInputOptions {
    static final int COUNT = 8;

    private MouseInputOptions() { }

    static int button(int index) {
        requireValid(index);
        return index / 4 + 1;
    }

    static int modifiers(int index) {
        requireValid(index);
        switch (index % 4) {
            case 1: return 2; // Shift
            case 2: return 1; // Ctrl
            case 3: return 4; // Alt
            default: return 0;
        }
    }

    private static void requireValid(int index) {
        if (index < 0 || index >= COUNT) {
            throw new IllegalArgumentException("Invalid mouse option: " + index);
        }
    }
}
