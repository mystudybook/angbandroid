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
        w.move(1, 0);
        write(w, "AB");
        w.move(0, 2);
        write(w, "中");
        check(w.buffer[0][2].ch == '?' && w.buffer[0][2].textColumns == 1,
                "wide character at right edge uses visible single-cell fallback");
        check(w.buffer[1][0].ch == 'A' && w.buffer[1][1].ch == 'B',
                "CJK fallback must not overwrite the following row");
        check(!w.buffer[0][2].isTextContinuation(),
                "last-column fallback never creates a stranded continuation");
        w.move(1, 2);
        write(w, "龍");
        check(w.buffer[1][2].ch == '?' && w.buffer[1][2].textColumns == 1,
                "last-row right-edge fallback remains single-cell");
        check(w.buffer[1][0].ch == 'A' && w.buffer[1][1].ch == 'B',
                "last-row fallback preserves preceding text");
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

    private static void testExplicitJniByteLength() {
        TermWindow w = new TermWindow(2, 8, 0, 0);
        byte[] padded = "中XY".getBytes(StandardCharsets.UTF_8);
        w.addnstr(3, padded, "UTF-8");
        check(w.buffer[0][0].ch == '中' && w.buffer[0][1].isTextContinuation(),
                "JNI byte length preserves complete UTF-8 character");
        check(w.buffer[0][2].ch == ' ' && w.col == 2,
                "unused trailing bytes are not decoded");

        w.addnstr(99, "AB".getBytes(StandardCharsets.UTF_8), "UTF-8");
        check(w.buffer[0][2].ch == 'A' && w.buffer[0][3].ch == 'B',
                "oversized JNI count is safely clamped to byte array length");

        int cursor = w.col;
        w.addnstr(0, padded, "UTF-8");
        w.addnstr(-1, padded, "UTF-8");
        w.addnstr(4, null, "UTF-8");
        check(w.col == cursor, "zero, negative and null JNI input does not move the cursor");
    }

    private static void testClearToBottomClearsCompleteLowerRows() {
        TermWindow w = new TermWindow(3, 6, 0, 0);
        write(w, "AB中D");
        w.move(1, 0);
        write(w, "龍XY");
        w.move(2, 0);
        write(w, "Z中Q");

        w.move(0, 4);
        w.clrtobot();
        check(w.buffer[0][0].ch == 'A' && w.buffer[0][1].ch == 'B',
                "clrtobot preserves preceding current-row text");
        check(w.buffer[0][2].ch == '中' && w.buffer[0][3].isTextContinuation(),
                "clrtobot preserves complete wide text before cursor");
        check(w.buffer[0][4].ch == ' ' && w.buffer[0][5].ch == ' ',
                "clrtobot clears current row through final column");
        for (int r = 1; r < 3; r++) {
            for (int c = 0; c < 6; c++) {
                check(w.buffer[r][c].ch == ' ' &&
                      w.buffer[r][c].textColumns == 1,
                      "clrtobot clears every lower-row column, including former CJK spans");
            }
        }
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

    private static void testPartialOverlayEdges() {
        TermWindow target = new TermWindow(1, 6, 0, 0);
        write(target, "中abc");
        TermWindow overlay = new TermWindow(1, 2, 0, 1);
        write(overlay, "XY");
        target.overwrite(overlay);
        check(target.buffer[0][0].ch == ' ', "overlay through trail clears old wide lead");
        check(target.buffer[0][1].ch == 'X' && target.buffer[0][2].ch == 'Y', "overlay letters copied");

        TermWindow source = new TermWindow(1, 4, 0, 0);
        write(source, "中A");
        TermWindow clipped = new TermWindow(1, 2, 0, 1);
        clipped.overwrite(source);
        check(clipped.buffer[0][0].textColumns == 1 && clipped.buffer[0][0].ch == ' ', "orphan continuation cleared on clipped copy");
        check(clipped.buffer[0][1].ch == 'A', "clipped overlay keeps independent characters");
    }

    public static void main(String[] args) {
        testPlacement();
        testExplicitJniByteLength();
        testOverwriteAndErase();
        testBoundary();
        testSupplementary();
        testResizeAndClear();
        testClearToBottomClearsCompleteLowerRows();
        testCopyAndScroll();
        testTileDoesNotInheritTextSpan();
        testTilePaddingSentinelIsNotText();
        testPartialOverlayEdges();
        System.out.println("PASS: terminal Unicode buffer and overwrite tests (" + cases + " assertions)");
    }
}
