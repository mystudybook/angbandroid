# Angband Android — Traditional Chinese terminal rendering status

Last source review: 2026-11-03. All changes so far were committed directly to `master`.
This is a **working implementation, not an Android release sign-off**.

## Implemented

1. `TerminalUnicode.columns()` classifies representative East Asian wide code points as two terminal columns; ordinary ASCII remains one.
2. `TermWindow.TermPoint` stores the original Unicode code point and its text-cell span: `2` (leading wide cell), `0` (continuation), or `1` (regular). Text is decoded as UTF-8/selected plugin encoding at the JNI boundary, then iterated by Unicode code point. Supplementary code points are retained for drawing.
3. `TermWindow.addnstr()` now reserves two terminal cells for Han characters and other classified wide glyphs. Wide glyphs are moved to the next row if only the final column is available; on the final row an unplaceable wide glyph falls back to `?` rather than stranding a partial cell.
4. Text writes, explicit `addch`, graphic tile writes, erasure, scrolling, window overwrites, and resize handle wide-cell invalidation and orphaned continuation cells. `isBigPad()` distinguishes the graphic padding sentinel from actual U+1E00 text.
5. `NativeWrapper.frosh()` skips text continuation cells so a wide leading glyph is not immediately painted over by an empty cell.
6. `TermView.drawPoint()` and `drawSubWindow()` render full code-point strings over two-column rectangles with Android font fallback for East Asian text. Graphic tile decoding remains separate and the tile-byte mask was **not** removed.
7. `scripts/test-terminal-unicode.sh` builds the real `TermWindow.java` with Android-only test stubs and runs pure-JDK tests. Tests are in `tests/java/org/rephial/xyangband/`.

## Local test evidence (not Android integration)

A local JDK 21 validation of a manually transferred copy of the committed Java source and regression tests passed:

- 14 Unicode width cases.
- 33 assertions covering ASCII/CJK mixing, cursor advancement, overwriting either half of a glyph, clear and resize, supplementary characters, scroll, tile compatibility, sentinel collision, and clipped overlay boundaries.
- 15,000 seeded text/tile/erase/resize/scroll operations verifying no orphaned lead or continuation cells.

These were **offline Java tests**, not a full GitHub checkout build, Android Gradle build, emulator run, or screenshot review. Run `sh scripts/test-terminal-unicode.sh` on a fresh checkout to reproduce.

## Confirmed native column-width mismatch — important

The Java terminal now correctly reserves two cells for a Chinese glyph, **but Angband's native terminal currently counts each wide character as exactly one column**:

- `app/src/main/cpp/angband/ui-term.c`, `Term_queue_chars()`: queues one `wchar_t` for each `x++`; there is no wide-character continuation cell.
- The same file, `Term_addstr()`: queues `n` characters and then advances the native cursor using `Term->scr->cx += n`.
- `app/src/main/cpp/common/angdroid.c`, `Term_text_android()`: forwards the original native `(x,y,n,wchar_t*)` positions through `wmove()` and `waddnwstr()`.

For a string `A中B龍`, the Java buffer reserves six display columns, while native `Term_addstr()` counts four. This will cause native cursor coordinates and Java renderer coordinates to diverge for actual translated gameplay. **Do not present the current implementation as end-to-end Chinese support.** Before translating core data, decide how to represent wide-glyph continuation cells in native `ui-term.c`, or introduce an explicit native-to-Android logical/physical column mapping with full redraw behavior. Existing ASCII/tile modes must remain compatible.

## Important limitations and remaining tasks

- **Native terminal layout:** Check Angband's `wcwidth`-like rules, `wchar_t` output, curses compatibility, command prompts, the text UI's own wrapping decisions and cursor coordinates. Java's new two-column policy and native layout must agree on real device screens.
- **Combining characters / emoji:** The current width model only distinguishes one vs. two columns; combining marks, emoji variation selectors and complex grapheme clusters need explicit handling and tests.
- **Font availability:** Android system font fallback is not a guarantee that every glyph displays properly on every device. Test actual Traditional Chinese and extension-B glyphs, including glyph height, clipping and alignment.
- **Non-Angband variants:** Some included engines use ISO-8859-1; do not assume native game message translation works across all plugins.
- **Graphic and UI interactions:** Test graphical tiles, ASCII mode, top bar, secondary windows, selection, cursor, redraw/scroll, rotation, keyboard and physical button mapping.
- **Savefiles and permissions:** Test installs, game startup, save/load and updates on an expendable emulator profile. Do not overwrite a user's existing save directory.
- **Distribution:** Preserve source licenses, variant permissions, art/font redistribution rights and attribution before making any Google Play release.

## Validation milestones

1. Run the standalone JDK script on a fresh checkout.
2. Produce a debug APK with the project's compatible Gradle, Android SDK and NDK; fix any build problems.
3. Exercise the terminal on a `zh-TW` Android emulator and a physical device, capturing screenshot evidence for `A中B龍`, `劍、牆、龍`, mixed punctuation, row-boundary glyphs and supplementary code points.
4. Compare game-core cursor movement and wrapping with Java terminal coordinates. Correct any mismatch in both layers.
5. Translate native game content only after those checks, keeping internal IDs, parser syntax, saved data and existing English fallback intact.

**No Android device-level acceptance or native game text translation is claimed yet.**
