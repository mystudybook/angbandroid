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
    static final int FROM_ZERO_NPP_ALL = 3;
    static final int FROM_ZERO_SILQ = 4;
    static final int FROM_ZERO_NPP041 = 5;

    private QuantityPromptText() { }

    static int classify(String prompt, int max) {
        return classify(prompt, max, false);
    }

    /** NPP041 and Sil-Q use the same English prompt but different all rules. */
    static int classify(String prompt, int max, boolean isNpp041) {
        if (prompt == null || max <= 0) return ORIGINAL;
        if (prompt.equals("Quantity (1-" + max + "): ")) return FROM_ONE;
        if (prompt.equals("Quantity (0-" + max + ", *=all): ")) return FROM_ZERO_ALL;
        if (prompt.equals("Quantity (0-" + max + ", '*' or 'a' = all): "))
            return FROM_ZERO_NPP_ALL;
        if (prompt.equals("Quantity (0-" + max + "): "))
            return isNpp041 ? FROM_ZERO_NPP041 : FROM_ZERO_SILQ;
        return ORIGINAL;
    }
}
