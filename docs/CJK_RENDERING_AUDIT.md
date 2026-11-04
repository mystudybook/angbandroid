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

## Finite menu navigation across invalid rows (2026-11-04)

- The previous `menu_handle_keypress()` navigation retry loop advanced by `ddy[dir]`. In column menus, horizontal movements have `ddy[dir] == 0`, so moving onto an invalid row could loop forever and freeze the game.
- Invalid-row navigation now uses a bounded row search with a direction derived from vertical **or horizontal** input, wrapping safely in either direction. It returns no selection when no valid destination exists.
- `tests/native/test_menu_valid_cursor.c` covers invalid horizontal destinations, both directions, wraparound, extreme cursors and empty/all-invalid menus. The C99 test is wired into `scripts/test-native-utf8.sh`; `tests/test_menu_navigation_source.py` guards the native integration through `scripts/verify-zh-tw.sh`.

**Acceptance gap:** actual gamepad/touch/keyboard movement, resize handling and APK-level input-loop behavior still require emulator/device verification.

## Blank-cell touch hit-testing in native menus (2026-11-04)

- The scroll/object and multi-column native menu skins formerly clamped clicks beyond the final item to `n - 1`. Tapping an empty row of a short menu, or an unpopulated cell in the last column, could unexpectedly select the last item.
- New `menu-hit-test.h` helpers reject empty cells with `-1` without changing keyboard navigation. Their coordinate math uses 64-bit intermediates to avoid overflowing when checking offsets, and invalid/zero-height layouts return no hit.
- `tests/native/test_menu_hit_test.c` covers partial columns, blank scroll rows, paging, row-count changes (rotation simulation), and extreme coordinates. `tests/test_menu_hit_test_source.py` checks that actual native selection paths call the helpers.
- `tests/test_menu_column_width_source.py` now asserts the updated cursor mapping and the zero-row guard.

**Limit:** These tests validate terminal-cell hit-test math and native source wiring; they do not validate Android pixel-to-cell conversion, device rotation lifecycle, or actual screenshots.

## Native menu compact layout and rotation behavior (2026-11-04)

- Native `menu_calc_size()` now clamps active menu bounds to the current terminal size and reserves real space for titles, headers and prompts. On extremely small surfaces, decorations are omitted rather than allowing negative widths or overlaying the only selectable row.
- The original skin is retained so multi-column menus can become single-column scrolling menus when each requested column cannot fit a minimum readable width (12 terminal cells). Resizing back to landscape restores the preferred multi-column skin and clears a stale scroll offset. Filter changes also reevaluate the current skin.
- Both multi-column painting and touch hit-testing now compute `colw` from the active region width rather than the full terminal, preventing a title indentation or bounded menu region from generating mismatched tap targets.
- The native input loop returns an escape event if a transitional resize has zero drawable rows/columns. The menu refresh and title/header/prompt rendering avoid invalid terminal geometry.
- `tests/native/test_menu_compact_layout.c` checks wide/small/tiny layouts, decoration positions, rotation-sized boundaries and column-to-scroll fallback. `tests/test_menu_compact_layout_source.py` checks native wiring.
- Actual Android orientation changes, subwindows, translated labels, mouse coordinates, and release APK behavior still require emulator or physical-device acceptance.

## Narrow terminal cells and CJK menu text (2026-11-04)

- `Term_putstr(n)` in the Angband native terminal counts decoded codepoints, not terminal columns, while Chinese characters often occupy **two cells**. The string-row callback previously sent the available column width as `n`, so mixed Chinese/English text could extend beyond its allotted menu region.
- The shared native `menu_put_clipped()` painter now copies complete UTF-8 scalars into a bounded temporary buffer and clips action strings, generic string rows, title/header/prompt by **rendered terminal column width** before calling `Term_putstr(..., -1, ...)`.
- Malformed UTF-8 source bytes are replaced with `?` in that display-only buffer; no underlying game records, localization keys, or saved data are modified.
- Very narrow selectable menu rows (1 or 2 columns) now truncate the display of their shortcut prefix rather than subtracting three cells and passing a negative width to a callback. Positive remaining widths are guaranteed before drawing row text.
- `tests/native/test_menu_text_fit.c` covers mixed CJK/ASCII boundaries, Chinese word clipping, fixed destination capacity, malformed bytes, and 1–2 cell prefixes. `tests/test_menu_text_fit_source.py` guards all the native render paths. Both are included in the offline verification scripts.

**Android verification still required:** render translated inventory and action menus with 1–12 terminal cells, rotate between landscape/portrait while selecting items, compare ASCII/tiles and Chinese fallback fonts on emulator and a real device.

## Action selection and filtered-cursor safety (2026-11-04)

- Native `menu_action_handle()` checked `acts->flags` (the **first item**) to decide whether the selected action was disabled. This could incorrectly block every action when the first row was gray, or execute a gray action when the first row was enabled. The selection handler now uses `acts[oid].flags` via a small tested guard.
- `menu_handle_action()` now verifies the visible cursor, filtered index and active row validity before invoking callbacks. A zero-item filter or stale `cursor == -1` during Android input processing can no longer read `filter_list[-1]` in this callback path.
- Added `tests/native/test_menu_action_guard.c` to the C99 helper test runner and `tests/test_menu_action_guard_source.py` to the combined offline verification. The test covers independent gray state, reordered selections, missing callbacks, empty menus, invalid cursor positions, and invalid mapped indices.

**Remaining Android acceptance:** gray disabled actions in mixed menus, filtered menus during quick screen changes, hardware keyboard Enter, and touch selection; tests above do not represent actual emulator results.

## Store menu scrolling and stock-free commands (2026-11-04)

- Store `EVT_SELECT` and touch handlers previously treated stock item IDs as screen row offsets. After scrolling to `top > 0`, an item context popup could be placed far below the visible list or could fail to open on its selected row. `store-menu-row.h` converts absolute item indices into bounded visible terminal rows, and handlers now reject stale indices/empty entries.
- The selected-row safety change in PR #9 also blocked **all command handlers** when a store had no stock, inadvertently preventing Sell, Help, and tapping the store header. This is now an **explicit store-only opt-in**, `MN_ALLOW_EMPTY_INPUT`: keyboard and mouse events may reach the store handler with `oid = -1`; selection events still require a valid item.
- `tests/native/test_store_menu_flow.c` covers scrolled popup positions, unused rows, integer bounds, empty-menu opt-in and filtered item selection. `tests/test_store_menu_flow_source.py` checks the native dispatch and store integration. Both tests are included in the combined verification runner.
- This change does not touch in-game inventory/save data structures or currency transactions; the handlers still execute the existing purchase/sell functions.

**Android verification still required:** empty shop `s` (sell), `?` (help), header context clicks; scrolling stock pages; tapping an existing item, cancelling its popup, then purchasing one item; device rotation while inside a shop.

## Native inventory/equipment item text clipping (2026-11-04)

- `ui-object.c:show_obj()` previously shortened the **original** `items[obj_num].o_name` buffer to a byte index whenever the available item name width was small. That permanently damaged the displayed name for the current inventory list and could terminate UTF-8 in the middle of a Traditional Chinese character.
- The inventory, equipment, floor and quiver paths now copy each label and object name into temporary UTF-8 buffers, clipping by terminal **display columns** without mutating the cached source strings. The displayed label/equipment prefix and object name share the same bounded row budget.
- `set_obj_names()` measures columns instead of encoded bytes. Both the regular inventory pane and the interactive item menu compute a nonnegative name budget even if the terminal is too narrow for its optional extra columns. Interactive row rendering uses its actual allocated region width.
- `tests/native/test_object_list_text.c` checks Chinese item names, equipment prefixes, narrow widths, no source mutation and signed-width corner cases. `tests/test_object_list_text_source.py` checks the native source call paths. Both are wired into the root verification command.
- The item structures and game savefile format are unchanged. Detailed device-level testing of weight/failure/price fields and source locale is still needed.

**Android acceptance pending:** inspect inventory/equipment/quiver/floor in portrait and landscape; rotate while selecting items; verify a longer name reappears after the menu widens; confirm labels and adjacent numeric fields do not overlap at tiny terminal sizes.

## Shop/Home narrow terminal columns and resized screens (2026-11-04)

- `ui-store.c` previously wrote price, weight, gold and owner positions as `screen_width - 14/-24/-26` into **unsigned** coordinate arrays. On portrait terminals narrower than the reserved columns these calculations could wrap into enormous coordinates, and the store's full UTF-8 object descriptions could overwrite price/weight cells.
- New `store-column-layout.h` makes price and weight **optional display-only columns**. Regular shops reserve three shortcut cells, twelve actual item-name cells, and a gap; the price column appears at 31 total terminal cells and weight at 41 cells; Home shows weight if it fits. All coordinates are nonnegative, and stock names are copied at UTF-8 scalar boundaries and clipped in **terminal display cells**.
- Shop owner and right-aligned store-summary labels are only placed together when they fit without overlap. The header, localized labels, gold balance and help prompt use bounded text painters. For extremely narrow views, price/weight are temporarily hidden rather than drawing over item names. Purchase and sell calculations remain unchanged.
- The store menu now registers a resize callback to recalculate all display columns and redraw the shop frame on terminal rotation. Footer/help Y locations are clamped when a transiently tiny Android terminal would otherwise underflow.
- `tests/native/test_store_column_layout.c` covers widths 0–105, both Home/shop layouts, 31/41-cell visibility transitions, clipped row budgets, large integer inputs, small heights, and a 3,542-case geometry sweep. `tests/test_store_column_layout_source.py` guards all integration paths; both are wired into the combined offline verification.

**Pending acceptance:** build an Android debug APK, view Home and a regular shop at 20/30/31/40/41/80 terminal cells, verify name/price/weight visibility and long Traditional Chinese item names, rotate while shopping, and complete buy/sell/cancel interactions on an emulator or device.

## Store purchase cancellation and invalid stock indices (2026-11-04)

- `store_purchase()` allocates a temporary stack with `object_new()` before the final confirmation prompt. On ESC/No, the original code returned immediately without releasing that stack. The rejection branch now invokes `object_delete(NULL, NULL, &dummy)` before returning; the existing successful-purchase and cannot-carry cleanup paths are preserved.
- Nested `store_get_stock()` no longer falls back to an original focused item when the menu returns an event other than `EVT_SELECT`. ESC, an unexpected event or a zero-stock shop returns `-1` instead. Actual selection must be within both the current store stock count **and** the allocated backing array capacity, and its object pointer must be non-null.
- Purchase, examination, item context popups, direct menu selection and touch selection validate the stock index before dereferencing it. `inspect_only` shop views explicitly reject purchase and sale commands even if an unexpected input event reaches those handlers.
- The C99 helper regression `tests/native/test_store_stock_guard.c` covers explicit SELECT, cancellation, zero stock, negative and stale cursors, capacity bounds and large indices. `tests/test_store_stock_guard_source.py` guards the real purchase cancellation cleanup, popup entry checks, read-only mode and keyboard dispatch. Both are wired into the root offline verification command.
- No purchase pricing, sale calculation, inventory ownership data or savefile structure is changed. Full game execution, Android screenshots, emulator input and heap profiling are still needed to verify end-to-end behavior and observe memory use.

**Pending Android acceptance:** repeatedly cancel a purchase after the price is displayed and monitor memory usage; buy one item, empty the store, invoke Help and Sell, then attempt buying or examining from an empty/stale list; rotate while item-selection is open; enter and leave read-only store knowledge.

## Dynamic context menus: g-to-Buy and cancelled choices (2026-11-04)

- Store item popups historically declared `g` as a switch key and installed a temporary row-handler that attempted to synthesize a `p` keypress. However, `menu_dynamic_select()` returned the **currently focused** item's value for any non-Escape return event, including this `g` keyboard event. Thus a Buy shortcut could accidentally execute Examine (or Buy one) depending on cursor position.
- Dynamic menus now only return an item value for a true `EVT_SELECT` event or for an explicitly declared alias that resolves to a **valid tagged row**. The shop item popup uses `menu_dynamic_select_alias(m, 'g', 'p')`, which selects the tagged `p` (Buy/Take) row independent of the cursor. Unrecognized key events, Escape and missing target tags return cancellation (`-1`).
- The old store-specific temporary row-handler hack is removed. The generic `dynamic_display()` also uses the existing bounded UTF-8 terminal-column painter, so Traditional Chinese popup labels cannot extend beyond the available menu width.
- Added `tests/native/test_menu_dynamic_choice.c` for selected and aliased rows, read-only menus, empty menus, cancelled events and invalid cursors, plus `tests/test_menu_dynamic_choice_source.py` to verify native dispatch. Both are included in the offline verification suite. Item identities, buy/sell transaction logic and savefiles remain unchanged.

**Pending Android acceptance:** open a store item popup with Examine focused, press `g` and verify it starts Buy instead of Examine; repeat with Buy one focused, then cancel at the price confirmation; verify Escape and right-click cancellation; check Chinese Buy/Examine labels at narrow widths.

## Dynamic menu popups on rotated and tiny Android terminals (2026-11-04)

- `ui-menu.c:menu_dynamic_calc_location()` previously used the fixed offsets `terminal_width - popup_width - 1` and `terminal_height - page_rows - 1`. For a terminal only 1–2 cells wide, the popup column could become negative. A menu taller than the screen could also be placed beyond the lower edge.
- The new `menu-popup-layout.h` computes a **bounded** popup width and visible row count from actual terminal cells. It preserves one-cell margins when possible; on extremely small surfaces, it drops the margins rather than making the popup unreachable.
- The popup origin is clamped even when the mouse anchor is negative, offscreen, or `INT_MAX`. Oversized menus use the existing scrolling skin; empty menus and zero-sized terminals have no selectable region, and `menu_select()` cancels safely.
- `tests/native/test_menu_popup_layout.c` includes boundary cases and a 400,000+ invocation sweep covering terminal widths/heights, menu lengths and extreme mouse coordinates. `tests/test_menu_popup_layout_source.py` checks native integration. Both are part of the combined offline verification.
- These changes only adjust display placement; store transactions, inventory indexes and savefile data are unchanged.

**Pending Android acceptance:** Open store popups near the right and bottom edges, rotate with a popup visible, try a terminal shorter than the action list, and verify touch selection, shortcuts and ESC.

## Dynamic menu filtered selections (2026-11-04)

- The generic dynamic popup receives its cursor from **visible rows**, which may be a filtered or reordered subset. Previously, `menu_dynamic_select_alias()` traversed the backing linked list using that visible index, so selecting visible row 0 in a reordered filter could return the wrong underlying action.
- `menu-dynamic-choice.h` now maps and bounds-checks the visible selection against `filter_list` and the original menu count before any linked-list traversal or row-validity callback. The ordinary unfiltered store context menu and the explicit `g` → Buy shortcut retain their behavior.
- `tests/native/test_menu_dynamic_choice.c` now covers reordered filters, empty/invalid filtered positions, bad underlying indices and alias cancellations. `tests/test_menu_dynamic_choice_source.py` checks the native mapping order. Existing combined verification executes both.
- This is a UI selection-index fix. Purchase/sale calculations and savefile data are unchanged.

**Pending Android acceptance:** open an item popup, use its keyboard tags and the `g` shortcut, cancel and reopen; where a filtered dynamic menu is used, verify the displayed row matches the chosen action.

## Generic filtered menu callback safety (2026-11-04)

- The generic menu previously bounded visible cursor positions but sent `filter_list` values straight into validity, private-tag, row-rendering, browse and Android soft-keyboard callbacks. A stale or malformed original index could access outside a menu's backing entries **before any selection**.
- These paths now share `angdroid_menu_action_item_index()`, rejecting invalid visible and source indices before calling an iterator or browse hook. Invalid rows cannot be selected or advertised as soft keys; valid filtered and unfiltered rows keep their existing behavior.
- `tests/native/test_menu_action_guard.c` includes `INT_MIN`, `INT_MAX`, reordered and invalid source indices; `tests/test_menu_action_guard_source.py` and `tests/test_menu_key_hints_source.py` assert source integration. The host C99 regression passed with address and undefined-behavior sanitizers against the exact GitHub test and helper blobs. Full Android build and actual malformed-filter UI acceptance have not been run.

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
