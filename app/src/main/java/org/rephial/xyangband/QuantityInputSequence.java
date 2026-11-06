package org.rephial.xyangband;

/** Builds one complete quantity response without touching Android input state. */
final class QuantityInputSequence {
    private QuantityInputSequence() { }

    // The native input field has at most ten decimal digits for a 32-bit int.
    // Fifteen navigation/deletion keys preserve the historical variant behavior.
    static final int PREFIX_KEYS = 15;

    static int[] forQuantity(int amount, int max, int rightKey,
                             int backspaceKey, int enterKey) {
        if (max <= 0 || amount < 0 || amount > max || rightKey < 0
                || backspaceKey <= 0 || enterKey <= 0) return null;

        String digits = Integer.toString(amount);
        int rightCount = rightKey > 0 ? PREFIX_KEYS : 0;
        int[] keys = new int[rightCount + PREFIX_KEYS + digits.length() + 1];
        int at = 0;
        for (int i = 0; i < rightCount; i++) keys[at++] = rightKey;
        for (int i = 0; i < PREFIX_KEYS; i++) keys[at++] = backspaceKey;
        for (int i = 0; i < digits.length(); i++) keys[at++] = digits.charAt(i);
        keys[at] = enterKey;
        return keys;
    }
}
