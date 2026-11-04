# Angband Android — Traditional Chinese terminal rendering status

Last source review: 2026-11-04. Changes are tracked on `master` through direct commits and merged pull requests.
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

## Native CJK grid integration (2026-11-03)

This stage has been committed to `master`, but **has not passed Android NDK compilation or emulator/gameplay integration tests**:

- `curses/unicode_utf8.h` now provides locale-independent UTF-8 encoding **and decoding** with validation and replacement of malformed code points. `common/angdroid.c` installs these conversions as the base Angband `text_mbcs_hook` / `text_wctomb_hook`; other variants keep their own encoding choices.
- `curses/unicode_layout.h` defines a private U+FDD0 **continuation marker**, row-width clipping, and a helper that separates visible redraw runs from continuation columns. `unicode_width.h` matches the Java width classifier.
- `angband/ui-term.c` now uses width-aware `Term_addstr()` and `Term_addch()`. The native cursor advances two physical columns for common Han glyphs, and the screen buffer stores a continuation marker in the second column. Writing or erasing either half also invalidates the other half. A wide glyph that does not fit on the final column is not split.
- `common/angdroid.c` removes internal continuation markers from `Term_text_android()` redraw runs while preserving their **physical x offsets**. Its `Term_pict_android()` sends unflagged Unicode glyphs through the text renderer instead of truncating them into 8-bit tile IDs. Existing high-bit tile indices remain on the tile path.
- `curses/waddnwstr()` encodes explicit `wchar_t` lengths as UTF-8 without relying on the C locale; failures are bounds-checked and allocation is freed.

## Incremental redraw and overwrite hardening (2026-11-03)

- `Term_redraw_section()` now validates requested bounds and expands **each row independently** when a selected column is a wide glyph's leading or continuation cell. It examines both the requested and previously drawn native grids, and preserves the upstream big-tile pad behavior. The prior implementation modified the shared `x1` across rows, causing unnecessary redraws on later rows.
- `unicode_layout.h` now exposes `angdroid_expand_redraw_columns()` and `angdroid_wide_partner_column()`. The native `Term_queue_char()` overwrite path uses the same testable partner-cell rule rather than duplicating glyph-span checks.
- `tests/native/test_unicode_layout.c` covers 8 redraw-boundary cases and 14 overwrite-partner assertions in addition to existing Unicode width/marker tests. An isolated local C99 copy of the helper logic passed these 22 checks under `-Wall -Wextra -Werror -pedantic`. **A complete checkout/NDK build has not been run for this change.**

## Additional terminal integrity fixes (2026-11-03)

- `TermWindow.clrtobot()` now clears the current row from the cursor and **all columns** on subsequent rows. Previously it incorrectly reused the original cursor column on every row, potentially leaving stale Chinese characters and continuation cells on lower rows.
- `TermWindow.addnstr(n, bytes, encoding)` now respects the explicit JNI **byte count**, clamps it to the actual byte-array size, and ignores null, zero or negative lengths. Previously it decoded the entire Java byte array regardless of `n`.
- `Term_redraw_section()` now **unions** its requested row/column bounds with native updates already queued for refresh instead of replacing them. `angdroid_union_dirty_range()` is shared with host-C tests so a localized CJK redraw cannot discard unrelated pending redraws.
- At the final column, the Java terminal now replaces an unplaceable double-column glyph with `?` rather than silently drawing it on the following row. This matches native Angband's current terminal-edge policy and prevents corrupting unrelated game content.
- Source-level checks in `scripts/check-native-unicode-contract.py` cover JNI bounds, full-row clearing, right-edge behavior and pending redraw preservation.

### Verified at the host-language level

A local JDK compilation of the corresponding Java classes/tests passed **14 width cases**, **53 terminal-buffer assertions**, and **15,000 seeded mixed terminal operations**. A host C99 `-Wall -Wextra -Werror -pedantic` build passed **7 redraw-dirty-range union cases** using an isolated copy of the committed helper implementation. These checks **do not establish that the complete current checkout or Android APK compiles**, and Android on-device CJK rendering is still unverified.

## Fixed-grid map compatibility (2026-11-03)

`Term_draw(x,y,a,c)` and `Term_queue_chars(x,y,n,a,text)` are **fixed-coordinate cell APIs**, often used for map cells and graphics. Widening a Han character in these calls would overwrite an unrelated neighboring map cell. To preserve the original grid contract:

- `angdroid_fixed_cell_glyph()` renders a wide glyph as `?` in these fixed-cell APIs instead of allocating a second column. Ordinary ASCII/Latin-1, private-use command glyphs and graphical tile attributes retain their original value.
- `Term_queue_chars()` now checks whether each target overwrites part of an existing wide text glyph and invalidates its other half. It still clears terrain values as it did previously.
- `Term_what()` now hides the internal U+FDD0 continuation marker from callers, returning a blank cell with `COLOUR_WHITE`. Direct game logic cannot accidentally treat that marker as a game character.
- Twelve native fixed-grid test assertions and four public-marker tests were added, along with source-level contract checks. Regular text-stream APIs `Term_addch()` and `Term_addstr()` also sanitize literal U+FDD0 input before it reaches the native grid; four additional stream-marker regression assertions cover this rule.

**Trade-off:** Chinese text sent through a *fixed-position per-cell map API* is intentionally replaced with `?` rather than widened. Use `Term_addstr()`/`Term_addch()` for regular two-column user-facing text. This distinction is necessary until the native map grid and textual UI coordinate schemes can be separated more comprehensively.

## Native menu page-relative shortcut safety (2026-11-04)

- Scroll and object menu skins now interpret **relative selection tags against the visible page**. Once the list scrolls, the displayed `a` shortcut selects the first visible row rather than an unrelated item from the first page.
- Both native key selection and Android soft-keyboard hints use the same visible row range. Multi-column menus retain all displayed rows instead of being truncated to one column's row count; filtered menus still translate visible indices into their underlying item IDs for validation.
- Skin tag lookup checks negative/out-of-range offsets and short tag strings, rather than indexing before or beyond the `selections` array. The nonrelative selection scan also stops at the menu's row count.
- `tests/native/test_menu_visible_tags.c` exercises page changes, a partial last page, column menus, filtered rows, negative and extreme offsets. `tests/test_menu_visible_tags_source.py` checks that native selection, display and soft-keyboard hint paths call the same helpers. The C test is included in `scripts/test-native-utf8.sh` and the source guard in `scripts/verify-zh-tw.sh`.

**Scope:** this is host-C and source-level protection. A debug APK/emulator test must still check mouse selection, scrolling, visible labels and shortcut behavior during real gameplay.

## Regression checks

Run from the repository root:

```bash
sh scripts/verify-zh-tw.sh
```

The script checks Android resources, Java Unicode terminal behavior, native UTF-8 encode/decode, native width/row fitting, native continuation redraw segmentation, and static source wiring. It produces `zh-tw-validation.log`.

Local host-C compilation of isolated equivalent helper implementations passed strict C99 `-Wall -Wextra -Werror -pedantic` checks for width, row fitting, redraw-run segmentation and UTF-8 decoding. **The full repository tests have not been run in an Android build environment in this round**, and source-level checks do not replace an end-to-end renderer test.

## Remaining integration issues — do not assume game-wide CJK support

- The main cursor paths `Term_addstr()` and `Term_addch()` now account for double-width glyphs, but **direct cell APIs such as `Term_draw()`, legacy `Term_queue_chars()`, game map coordinate calculations and various UI layout routines may still assume one logical cell per character**. The native/Java policy is not yet verified consistently across all rendering paths.
- Rendering a changed lead or continuation cell must remain correct during incremental refresh, scrolling, graphics switching and background redraw. Native code was updated, but it needs Android emulator screenshots and tests.
- Combining marks, emoji sequences and grapheme clusters still need handling; the implemented classifier currently supports widths 1 and 2, not zero-width attachments.
- The special internal U+FDD0 continuation marker is reserved, not user-displayable text. Control paths must not expose it to Java.
- Map tiles, half-tile redraws, alternate Angband variants and savefile compatibility require separate regression testing.

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
