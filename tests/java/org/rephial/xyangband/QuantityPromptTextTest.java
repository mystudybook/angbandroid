package org.rephial.xyangband;

/** Exact-match prompt recognition without the Android SDK or locale changes. */
public final class QuantityPromptTextTest {
    private static int checks = 0;
    private static void check(int actual, int expected, String reason) {
        checks++;
        if (actual != expected) throw new AssertionError(reason + ": " + actual);
    }

    public static void main(String[] args) {
        check(QuantityPromptText.classify(null, 9),
                QuantityPromptText.ORIGINAL, "null");
        check(QuantityPromptText.classify("", 9),
                QuantityPromptText.ORIGINAL, "empty");
        check(QuantityPromptText.classify("Quantity (1-9): ", 0),
                QuantityPromptText.ORIGINAL, "invalid max");
        check(QuantityPromptText.classify("Quantity (1-9): ", -3),
                QuantityPromptText.ORIGINAL, "negative max");

        for (int bound = 1; bound <= 10000; bound++) {
            check(QuantityPromptText.classify("Quantity (1-" + bound + "): ", bound),
                    QuantityPromptText.FROM_ONE, "Frog default " + bound);
            check(QuantityPromptText.classify(
                            "Quantity (0-" + bound + ", *=all): ", bound),
                    QuantityPromptText.FROM_ZERO_ALL, "FA/Angband default " + bound);
            check(QuantityPromptText.classify(
                            "Quantity (0-" + bound + ", '*' or 'a' = all): ", bound),
                    QuantityPromptText.FROM_ZERO_NPP_ALL, "NPP default " + bound);
            check(QuantityPromptText.classify("Quantity (0-" + bound + "): ", bound),
                    QuantityPromptText.FROM_ZERO_SILQ, "Sil-Q default " + bound);
            check(QuantityPromptText.classify("Quantity (0-" + bound + "): ", bound, true),
                    QuantityPromptText.FROM_ZERO_NPP041, "NPP041 default " + bound);
            check(QuantityPromptText.classify("Quantity (0-" + bound + "): ", bound, false),
                    QuantityPromptText.FROM_ZERO_SILQ, "Sil-Q explicit context " + bound);
            check(QuantityPromptText.classify("Quantity (1-" + bound + "): ", bound + 1),
                    QuantityPromptText.ORIGINAL, "mismatched bound " + bound);
            check(QuantityPromptText.classify("Quantity (0-" + bound + ", *=all):", bound),
                    QuantityPromptText.ORIGINAL, "spacing must match " + bound);
            check(QuantityPromptText.classify("Quantity (0-" + bound + "): ", bound + 1),
                    QuantityPromptText.ORIGINAL, "Sil-Q wrong limit " + bound);
            check(QuantityPromptText.classify("Quantity (0-" + bound + "): ", bound + 1, true),
                    QuantityPromptText.ORIGINAL, "NPP041 wrong limit " + bound);
            check(QuantityPromptText.classify(
                            "Quantity (0-" + bound + ", '*' or 'a' = all):", bound),
                    QuantityPromptText.ORIGINAL, "NPP whitespace " + bound);
        }

        String[] customPrompts = {
            "Buy how many?",
            "Sell how many?",
            "How many items? ",
            "數量（1～10）：",
            "Quantity (1-999): ",
            "Quantity (0-999, *=all): ",
            "Quantity (0-999, '*' or 'a' = all): ",
            "Quantity (0-999): ",
            "quantity (1-10): ",
            "Quantity (1-10): 0",
            "Quantity (1-10): \n",
        };
        for (String prompt : customPrompts) {
            check(QuantityPromptText.classify(prompt, 10),
                    QuantityPromptText.ORIGINAL, "custom prompt unchanged");
        }
        check(QuantityPromptText.classify(
                        "Quantity (1-" + Integer.MAX_VALUE + "): ", Integer.MAX_VALUE),
                QuantityPromptText.FROM_ONE, "32-bit max");
        check(QuantityPromptText.classify(
                        "Quantity (0-" + Integer.MAX_VALUE + ", *=all): ",
                        Integer.MAX_VALUE),
                QuantityPromptText.FROM_ZERO_ALL, "32-bit all");
        check(QuantityPromptText.classify(
                        "Quantity (0-" + Integer.MAX_VALUE + "): ",
                        Integer.MAX_VALUE, true),
                QuantityPromptText.FROM_ZERO_NPP041, "32-bit NPP041");
        check(QuantityPromptText.classify(null, 10, true),
                QuantityPromptText.ORIGINAL, "null with NPP041 context");
        System.out.println("PASS: quantity prompt localization recognition (" + checks + " checks)");
    }
}
