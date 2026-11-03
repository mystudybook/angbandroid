package org.rephial.xyangband;

/** Unicode East Asian wide-character classification (display columns, not bytes). */
final class TerminalUnicode {
    private TerminalUnicode() { }

    static int columns(int codePoint) {
        return isWide(codePoint) ? 2 : 1;
    }

    static boolean isWide(int codePoint) {
        return codePoint >= 0x1100 && (
                codePoint <= 0x115F
                || codePoint == 0x2329 || codePoint == 0x232A
                || (codePoint >= 0x2E80 && codePoint <= 0xA4CF && codePoint != 0x303F)
                || (codePoint >= 0xAC00 && codePoint <= 0xD7A3)
                || (codePoint >= 0xF900 && codePoint <= 0xFAFF)
                || (codePoint >= 0xFE10 && codePoint <= 0xFE19)
                || (codePoint >= 0xFE30 && codePoint <= 0xFE6F)
                || (codePoint >= 0xFF00 && codePoint <= 0xFF60)
                || (codePoint >= 0xFFE0 && codePoint <= 0xFFE6)
                || (codePoint >= 0x20000 && codePoint <= 0x2FFFD)
                || (codePoint >= 0x30000 && codePoint <= 0x3FFFD)
        );
    }
}
