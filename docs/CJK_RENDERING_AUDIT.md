# Native Angband Chinese text rendering audit

Status: **analysis only**. The Android-facing UI localization can proceed, but native-game text must not be treated as Chinese-capable yet. Do not translate native gameplay data until a Unicode rendering proof-of-concept and Android device test pass.

## Confirmed source-level problems

1. **Characters are truncated before drawing.** In `app/src/main/java/org/rephial/xyangband/TermView.java`, both `drawPoint()` and `drawText()` apply `&= 0xFF` to their character values before building a Java `String`. Most Traditional Chinese code points are above U+00FF and cannot survive that operation.
2. **Every terminal character is rendered into exactly one cell.** `TermView.drawPoint()` and `drawText()` compute the rectangle from `char_width` and `char_height`, then `canvas.clipRect(bg)` to a single cell. Full-width CJK glyphs can be clipped or overlap incorrectly. Fixing only the bit mask is insufficient.
3. **Terminal string writing advances by UTF-16 code unit, not Unicode code point or visual column width.** `TermWindow.addnstr()` decodes bytes using the selected encoding, then iterates `String.length()` and calls `addch(str.charAt(i))`. BMP Han characters are one UTF-16 unit but occupy two traditional terminal columns; supplementary characters are split into surrogate halves. A width-aware terminal model is required.
4. **The chosen game font is not validated for Chinese.** `TermView.setFontFace()` loads `6x13.ttf` or `VeraMoBd.ttf`. No CJK font-coverage check or fallback behavior has been demonstrated. Don't assume that removing the bit mask guarantees visible Chinese.
5. **The encoding changes between bundled variants.** `Plugins.Plugin.getEncoding()` returns `ISO-8859-1` for two NPPAngband variants, otherwise `UTF-8`. Prototype native Chinese rendering with the ordinary Angband variant first; assess other variants independently.
6. **Byte input alone is not enough.** `NativeWrapper.waddnstr()` passes bytes and the plugin encoding to `TermWindow.addnstr()`. The native Angband core uses UTF-8/wide characters in several places, but the Android rendering layer still needs a coherent Unicode and column-width contract.

## Proposed implementation order

1. Create a **separate proof-of-concept branch** and a minimal terminal rendering test that injects ASCII, Latin-1, CJK, punctuation, combining characters, and a supplementary Unicode character. Preserve the current renderer for production until results are verified.
2. Specify the Unicode contract: code points, terminal columns (0/1/2), continuation cells, width of fullwidth glyphs, combining marks, selection/cursor behavior, subwindows and map tiles.
3. Replace one-byte truncation in text rendering without altering the byte-based special commands, tile codes or color attributes. Keep text and tile paths explicitly separate.
4. Add fonts with proper redistribution rights and tested CJK glyph coverage. Measure text and clipping using **display columns**, not raw UTF-16 length, while keeping ASCII tiles and existing savefiles intact.
5. Test UTF-8 decoding end to end from native game text to Java `TermWindow` to Canvas, with representative characters such as `繁體中文、龍、劍、牆、＠`.
6. Only after that, translate in-game messages and suitable `variant_lib/angband/gamedata` display fields. Keep internal identifiers, parser keys and command characters unchanged.

## Acceptance checklist

- ASCII gameplay and all existing font/graphic modes still render correctly.
- Han ideographs, full-width punctuation and mixed English/Chinese strings display without replacement glyphs, clipping, overlapping or off-by-one cursor movement.
- Word wrap, modal dialogs, item selection, quick keys, subwindows and terminal redraws remain aligned.
- English language fallback and all other bundled variants remain playable.
- Save/load and restart do not corrupt character, world or user preference data.
- Run Gradle/NDK build, emulator and a physical Android device smoke test before merging.

## Scope and evidence limits

This report is grounded in the checked-in source paths above. **No Android build or emulator test has been run here.** Even after fixing the Java renderer, native layout, terminal width calculation and font shaping may require additional changes. This document is not a claim that full native Chinese support has been implemented.
