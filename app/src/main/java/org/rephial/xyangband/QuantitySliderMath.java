package org.rephial.xyangband;

/** Integer-safe conversion between a quantity range and a 0-100 seek bar. */
final class QuantitySliderMath {
    private QuantitySliderMath() { }

    static int clamp(int value, int min, int max) {
        if (max <= min) return min;
        if (value < min) return min;
        return Math.min(value, max);
    }

    static int valueForProgress(int min, int max, int progress) {
        if (max <= min) return min;
        int bounded = clamp(progress, 0, 100);
        long range = (long) max - min;
        return (int) (min + (range * bounded) / 100L);
    }

    static int progressForValue(int min, int max, int value) {
        if (max <= min) return 0;
        long range = (long) max - min;
        long offset = (long) clamp(value, min, max) - min;
        return (int) ((offset * 100L) / range);
    }
}
