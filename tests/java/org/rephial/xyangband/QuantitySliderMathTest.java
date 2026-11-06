package org.rephial.xyangband;

/** Android-SDK-free regression for zero ranges and 32-bit extreme values. */
public final class QuantitySliderMathTest {
    private static int checks = 0;
    private static void expect(int expected, int actual) {
        checks++;
        if (actual != expected)
            throw new AssertionError("expected " + expected + " but got " + actual);
    }
    private static void assertRange(int min, int max) {
        int previous = Integer.MIN_VALUE;
        for (int p = 0; p <= 100; p++) {
            int value = QuantitySliderMath.valueForProgress(min, max, p);
            if (value < min || value > max || value < previous)
                throw new AssertionError("bad slider range at " + p + ": " + value);
            int roundTrip = QuantitySliderMath.progressForValue(min, max, value);
            if (roundTrip < 0 || roundTrip > 100 || roundTrip > p)
                throw new AssertionError("bad progress round-trip at " + p + ": " + roundTrip);
            previous = value;
            checks += 2;
        }
    }
    public static void main(String[] args) {
        expect(0, QuantitySliderMath.valueForProgress(0, 0, 100));
        expect(0, QuantitySliderMath.progressForValue(0, 0, 0));
        expect(7, QuantitySliderMath.valueForProgress(7, 7, 30));
        expect(0, QuantitySliderMath.progressForValue(7, 7, 7));
        expect(7, QuantitySliderMath.valueForProgress(7, 2, 30));
        expect(0, QuantitySliderMath.progressForValue(7, 2, 7));
        expect(0, QuantitySliderMath.valueForProgress(0, 10, -1));
        expect(10, QuantitySliderMath.valueForProgress(0, 10, 101));
        expect(0, QuantitySliderMath.progressForValue(0, 10, -99));
        expect(100, QuantitySliderMath.progressForValue(0, 10, 1000));
        expect(10, QuantitySliderMath.valueForProgress(0, 10, 100));
        expect(3, QuantitySliderMath.valueForProgress(0, 10, 30));
        expect(30, QuantitySliderMath.progressForValue(0, 10, 3));
        expect(Integer.MAX_VALUE,
                QuantitySliderMath.valueForProgress(0, Integer.MAX_VALUE, 100));
        expect(100, QuantitySliderMath.progressForValue(0, Integer.MAX_VALUE, Integer.MAX_VALUE));
        expect(Integer.MIN_VALUE,
                QuantitySliderMath.valueForProgress(Integer.MIN_VALUE, Integer.MAX_VALUE, 0));
        expect(Integer.MAX_VALUE,
                QuantitySliderMath.valueForProgress(Integer.MIN_VALUE, Integer.MAX_VALUE, 100));
        expect(-1, QuantitySliderMath.valueForProgress(
                Integer.MIN_VALUE, Integer.MAX_VALUE, 50));
        assertRange(0, 1);
        assertRange(0, 10);
        assertRange(1, 99);
        assertRange(0, Integer.MAX_VALUE);
        assertRange(Integer.MIN_VALUE, Integer.MAX_VALUE);
        System.out.println("PASS: quantity slider arithmetic (" + checks + " checks)");
    }
}
