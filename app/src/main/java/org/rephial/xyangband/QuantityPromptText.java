package org.rephial.xyangband;

/**
 * Recognizes only the native cores' generic quantity prompts.
 *
 * Caller supplies the numeric bound separately; never translate arbitrary
 * game-specific questions or alter the native input/command protocol.
 */
final class QuantityPromptText {
    static final int ORIGINAL = 0;
    static final int FROM_ONE = 1;
    static final int FROM_ZERO_ALL = 2;

    private QuantityPromptText() { }

    static int classify(String prompt, int max) {
        if (prompt == null || max <= 0) return ORIGINAL;
        if (prompt.equals("Quantity (1-" + max + "): ")) return FROM_ONE;
        if (prompt.equals("Quantity (0-" + max + ", *=all): ")) return FROM_ZERO_ALL;
        return ORIGINAL;
    }
}
