package org.rephial.xyangband;

import java.nio.charset.StandardCharsets;

/** JDK-only regression tests against the actual TermWindow source. */
public final class TermWindowUnicodeTest {
    private static int cases = 0;

    private static void check(boolean condition, String message) {
        cases++;
        if (!condition) throw new AssertionError(message);
    }

    private static void write(TermWindow win, String text) {
        byte[] bytes = text.getBytes(StandardCharsets.UTF_8);
        win.addnstr(bytes.length, bytes, "UTF-8");
    }

    private static void testPlacement() {
        TermWindow w = new TermWindow(3, 9, 0, 0);
        write(w, "A中B龍");
        check(w.buffer[0][0].ch == 'A', "ASCII first cell");
        check(w.buffer[0][1].ch == '中' && w.buffer[0][1].textColumns == 2, "wide lead");
        check(w.buffer[0][2].isTextContinuation(), "wide trailing cell");
        check(w.buffer[0][3].ch == 'B', "following ASCII starts after two columns");
        check(w.buffer[0][4].ch == '龍' && w.buffer[0][4].textColumns == 2, "second CJK");
        check(w.col == 6 && w.row == 0, "cursor uses six display columns");
        check("中".equals(w.buffer[0][1].displayText()), "BMP display text");
    }

    private static void testOverwriteAndErase() {
        TermWindow w = new TermWindow(2, 8, 0, 0);
        write(w, "中龍");
        w.move(0, 1);
        w.addch('X');
        check(w.buffer[0][0].ch == ' ', "overwriting continuation clears old lead");
        check(w.buffer[0][1].ch == 'X', "replacement remains");
        check(w.buffer[0][2].ch == '龍' && w.buffer[0][3].isTextContinuation(), "neighbor remains intact");
        w.clearPoint(0, 3);
        check(w.buffer[0][2].ch == ' ' && !w.buffer[0][3].isTextContinuation(), "erase continuation clears pair");
        w.move(0, 4);
        write(w, "中");
        w.clearPoint(0, 4);
        check(w.buffer[0][4].ch == ' ' && !w.buffer[0][5].isTextContinuation(), "erase leading clears pair");
    }

    private static void testBoundary() {
        TermWindow w = new TermWindow(2, 3, 0, 0);
        w.move(0, 2);
        write(w, "中");
        check(w.buffer[1][0].ch == '中' && w.buffer[1][1].isTextContinuation(), "wide char wraps as pair");
        check(w.row == 1 && w.col == 2, "wide wrap cursor");
        w.move(1, 2);
        write(w, "龍");
        check(w.buffer[1][2].ch == '?' && w.buffer[1][2].textColumns == 1, "last cell fallback");
        check(w.buffer[1][0].ch == '中', "last-row fallback does not damage earlier cells");
    }

    private static void testSupplementary() {
        TermWindow w = new TermWindow(2, 5, 0, 0);
        write(w, new String(Character.toChars(0x20000)));
        check(w.buffer[0][0].textColumns == 2, "non-BMP ideograph reserves two cells");
        check(w.buffer[0][1].isTextContinuation(), "non-BMP trailing cell");
        check(w.buffer[0][0].textCodePoint == 0x20000, "full code point is preserved");
        check(w.buffer[0][0].displayText().codePointAt(0) == 0x20000, "full display string");
    }

    private static void testResizeAndClear() {
        TermWindow w = new TermWindow(2, 4, 0, 0);
        w.move(0, 2);
        write(w, "中");
        w.resize(3, 2);
        check(w.buffer[0][2].ch == ' ' && w.buffer[0][2].textColumns == 1, "resize removes cropped glyph lead");
        w.move(1, 0);
        write(w, "龍");
        w.move(1, 1);
        w.clrtoeol();
        check(w.buffer[1][0].ch == ' ' && w.buffer[1][1].ch == ' ', "clrtoeol removes both halves");
    }

    private static void testCopyAndScroll() {
        TermWindow source = new TermWindow(2, 4, 0, 0);
        TermWindow dest = new TermWindow(2, 4, 0, 0);
        source.move(1, 0);
        write(source, "中Q");
        dest.overwrite(source);
        check(dest.buffer[1][0].textColumns == 2 && dest.buffer[1][1].isTextContinuation(), "overwritten buffer preserves spans");
        dest.scroll();
        check(dest.buffer[0][0].ch == '中' && dest.buffer[0][1].isTextContinuation(), "scroll preserves wide pair");
        check(dest.buffer[1][0].ch == ' ' && dest.buffer[1][1].ch == ' ', "scroll clears new bottom row");
    }

    private static void testTileDoesNotInheritTextSpan() {
        TermWindow w = new TermWindow(1, 4, 0, 0);
        write(w, "中");
        w.addTile(1, 0, 0x80, 0xF0, 0, 0);
        check(w.buffer[0][0].textColumns == 1 && w.buffer[0][0].ch == ' ', "tile overwrites trailing cell and invalidates text lead");
        check(w.buffer[0][1].textColumns == 1 && w.buffer[0][1].isGraphicTile(), "tile maintains original flags");
    }

    private static void testTilePaddingSentinelIsNotText() {
        TermWindow w = new TermWindow(1, 3, 0, 0);
        write(w, "\u1E00");
        check(!w.buffer[0][0].isBigPad(), "real U+1E00 glyph must not be treated as a tile pad");
        w.buffer[0][1].ch = TermView.BIG_PAD;
        w.buffer[0][1].textCodePoint = 0;
        check(w.buffer[0][1].isBigPad(), "graphics tile pad sentinel still works");
    }

    public static void main(String[] args) {
        testPlacement();
        testOverwriteAndErase();
        testBoundary();
        testSupplementary();
        testResizeAndClear();
        testCopyAndScroll();
        testTileDoesNotInheritTextSpan();
        testTilePaddingSentinelIsNotText();
        System.out.println("PASS: terminal Unicode buffer and overwrite tests (" + cases + " assertions)");
    }
}
