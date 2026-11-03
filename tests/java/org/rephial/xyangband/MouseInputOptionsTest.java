package org.rephial.xyangband;

/** Pure-Java test for localized popup labels vs. stable game commands. */
public final class MouseInputOptionsTest {
    private static int assertions = 0;

    private static void expect(boolean value, String description) {
        assertions++;
        if (!value) throw new AssertionError(description);
    }

    private static void rejects(int index) {
        boolean buttonRejected = false;
        boolean modifiersRejected = false;
        try { MouseInputOptions.button(index); }
        catch (IllegalArgumentException ignored) { buttonRejected = true; }
        try { MouseInputOptions.modifiers(index); }
        catch (IllegalArgumentException ignored) { modifiersRejected = true; }
        expect(buttonRejected && modifiersRejected, "invalid option " + index + " rejected");
    }

    public static void main(String[] args) {
        int[] buttons = {1, 1, 1, 1, 2, 2, 2, 2};
        int[] modifiers = {0, 2, 1, 4, 0, 2, 1, 4};
        expect(MouseInputOptions.COUNT == 8, "expected eight mouse choices");
        for (int i = 0; i < MouseInputOptions.COUNT; i++) {
            expect(MouseInputOptions.button(i) == buttons[i], "button for option " + i);
            expect(MouseInputOptions.modifiers(i) == modifiers[i], "modifiers for option " + i);
        }
        rejects(-1);
        rejects(8);
        System.out.println("PASS: mouse popup button/modifier mapping (" + assertions + " assertions)");
    }
}
