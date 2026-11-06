package org.rephial.xyangband;

/** Pure JDK test for the complete raw sequence sent to the native prompt. */
public final class QuantityInputSequenceTest {
    private static int checks = 0;
    private static void expect(boolean condition, String detail) {
        checks++;
        if (!condition) throw new AssertionError(detail);
    }
    private static void check(int amount, int max, int rightKey) {
        int backspace = 159, enter = 156;
        int[] keys = QuantityInputSequence.forQuantity(amount, max, rightKey,
                                                        backspace, enter);
        expect(keys != null, "valid amount " + amount);
        String digits = Integer.toString(amount);
        int count = QuantityInputSequence.PREFIX_KEYS;
        int rc = rightKey > 0 ? count : 0;
        expect(keys.length == rc + count + digits.length() + 1, "sequence length");
        int at = 0;
        for (int i = 0; i < rc; i++) expect(keys[at++] == rightKey, "right prefix");
        for (int i = 0; i < count; i++) expect(keys[at++] == backspace, "deletion prefix");
        for (int i = 0; i < digits.length(); i++) expect(keys[at++] == digits.charAt(i), "ASCII digit");
        expect(keys[at] == enter, "single final Enter");
    }
    public static void main(String[] args) {
        check(0, 10, 0);
        check(1, 1, 0);
        check(10, 10, 0);
        check(13, 20, 6);
        check(Integer.MAX_VALUE, Integer.MAX_VALUE, 6);
        for (int i = 0; i < 10001; i++) check(i, 10000, i % 2 == 0 ? 0 : 6);
        expect(QuantityInputSequence.forQuantity(-1, 5, 0, 159, 156) == null, "negative");
        expect(QuantityInputSequence.forQuantity(6, 5, 0, 159, 156) == null, "above max");
        expect(QuantityInputSequence.forQuantity(0, 0, 0, 159, 156) == null, "empty range");
        expect(QuantityInputSequence.forQuantity(1, 5, -1, 159, 156) == null, "negative right key");
        expect(QuantityInputSequence.forQuantity(1, 5, 0, 0, 156) == null, "missing backspace");
        expect(QuantityInputSequence.forQuantity(1, 5, 0, 159, 0) == null, "missing enter");
        System.out.println("PASS: quantity input sequence (" + checks + " assertions)");
    }
}
