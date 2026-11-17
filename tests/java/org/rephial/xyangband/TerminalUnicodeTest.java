package org.rephial.xyangband;

/** Standalone JDK test; does not require the Android SDK. */
public final class TerminalUnicodeTest {
    private static void expect(int codePoint, int columns) {
        int actual = TerminalUnicode.columns(codePoint);
        if (actual != columns) {
            throw new AssertionError("U+" + Integer.toHexString(codePoint)
                    + ": expected " + columns + " columns, got " + actual);
        }
    }

    public static void main(String[] args) {
        expect('A', 1);
        expect('@', 1);
        expect('9', 1);
        expect('ß', 1);
        expect('中', 2);
        expect('龍', 2);
        expect('劍', 2);
        expect('。', 2);
        expect('あ', 2);
        expect('한', 2);
        expect('Ａ', 2);
        expect(0x20000, 2);
        expect(0x303F, 1);
        expect(0xE000, 1); // Game special-key/private-use area.
        expect(0x1F431, 1); // Match native terminal cell width for cat emoji.
        System.out.println("PASS: East Asian terminal width classification (15 cases)");
    }
}
