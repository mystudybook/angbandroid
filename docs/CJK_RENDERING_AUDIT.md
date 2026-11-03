# Native Angband Chinese text rendering audit

Status: **initial mitigations implemented on `master`; complete Unicode terminal support remains unfinished.** No Android APK build, emulator or physical-device test has been completed.

## Changes already made

- `TermView.drawPoint()` no longer discards the high byte of UTF-16/BMP characters. Its East Asian glyph path uses an Android system font fallback and scales glyphs to fit the legacy one-cell text clip. This makes a BMP Chinese glyph *eligible* for drawing but is **not proper two-column CJK layout**.
- `TermView.drawSubWindow()` now shares the East Asian font fallback strategy, including the top information bar.
- `TerminalUnicode.java` provides a reusable East Asian wide-character classifier, with pure-Java tests in `tests/java/org/rephial/xyangband/TerminalUnicodeTest.java`. An isolated local JDK compilation and test run passed 14 representative width cases, including Han characters, full-width punctuation, ASCII and the private-use special-key range. Run `sh scripts/test-terminal-unicode.sh` on a full checkout to repeat that test.
- `TermWindow.addnstr()` now iterates decoded text by Unicode code point, avoiding the accidental splitting of supplementary characters into two UTF-16 surrogate halves. **Supplementary code points still fall back to `?`** until the terminal cell model supports full Unicode.

## Unresolved architecture work

1. **Two-column character layout:** `TermWindow` stores one `char` per terminal cell and always advances its cursor one column for each `addch()`. The current drawing workaround scales Han glyphs into one cell rather than reserving a second cell. This can look narrow and is not acceptable as the finished Chinese terminal.
2. **Continuation cells and overwrite/redraw logic:** Implement width-aware storage (one or two columns), account for combining marks and supplementary code points, repair overwrite/scroll/clear/resize/cursor behavior, and keep `TermWindow.overwrite()` / `NativeWrapper.frosh()` correct.
3. **Graphic tile codes:** `TermView.drawText()` still uses a one-byte mask in the tile/ASCII fallback path. Do not simply remove that mask without determining how native tile IDs and flags are encoded. CJK game text should first flow through the ordinary text path.
4. **Font coverage and licensing:** The bundled terminal fonts (`6x13.ttf`, `VeraMoBd.ttf`) are not proven to support Chinese. The system fallback path is new but unverified on devices. If shipping another CJK font, review its redistribution license before bundling.
5. **Native-layout width:** Native Angband uses UTF-8 and `wchar_t` in parts of the codebase. Ensure its notion of terminal columns matches the Java one, particularly menus, descriptions, wrapping, cursor positioning and non-ASCII special keys.
6. **Per-variant encoding:** `Plugins.Plugin.getEncoding()` uses `ISO-8859-1` for some NPPAngband variants while ordinary Angband uses UTF-8. Full Chinese gameplay localization must be validated per engine.

## Recommended next steps

1. Introduce a self-contained width-aware text buffer (including continuation markers) with isolated JVM tests and integration into `TermWindow`, leaving tiles and key codes alone.
2. Verify redraw, overwrite, erase, cursor, subwindow text and text wrapping, using `繁體中文、龍、劍、牆、＠` plus mixed ASCII and CJK strings.
3. Obtain an Android Gradle/NDK debug build and test on a zh-TW emulator before translating game data. No test should overwrite valuable user save files.
4. Translate native messages and selected `variant_lib/angband/gamedata` display fields only after confirming rendering. Preserve parser keys, identifiers, gameplay scripting and existing licenses.

## Acceptance checklist

- ASCII gameplay and all existing text/graphic modes render correctly.
- CJK glyphs have appropriate width and do not clip, overlap or misalign the terminal cursor.
- Combining marks and supplementary Unicode inputs behave predictably.
- Word wrapping, modal dialogs, item selection, subwindows, redraw and scrolling remain aligned.
- English fallback and other bundled variants remain playable.
- Save/load works across app restarts, and an Android debug APK runs on emulator and a real device.

This is an **engineering audit, not a claim of a completed Chinese game-core port**. Changes are committed directly on `master` as requested.
