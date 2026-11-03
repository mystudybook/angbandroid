# Traditional Chinese (Taiwan) localization plan

This is an **unofficial** localization of the Android Angband port. The intended end goal is to offer a free Traditional Chinese edition, but **publishing is not yet approved or verified**. Preserve original attribution and review the license of every bundled component (including variants, artwork, fonts, and Android port code) before distributing binaries.

## Independent app language (2026-11-03)

Android Settings → Game → **App language / 應用程式語言** offers
**Follow system / 跟隨系統** (the default for existing users), **English**,
and **Traditional Chinese (Taiwan) / 繁體中文（台灣）**. The preference
`angband.app_language` is stored in the existing `angband` SharedPreferences
file and is intentionally separate from profiles, game IDs, keyboard
commands and save files.

The selected language controls Android app screen resources, the help HTML,
and the *already translated* display-only text in the original native Angband
engine. With Follow system, `zh-TW` or `zh-Hant` uses available Traditional
Chinese resources; other device languages use the original fallback. This
does not translate the other included variants or untranslated gameplay text.

The settings screen refreshes after a selection; **quit and restart the game**
to update already running screens and the native game's cached locale.
Language changes do **not** automatically kill the game thread or alter
save data. Changing this preference while a game is running should be
done after saving progress and followed by a full app/game restart.

Regression checks include `AppLanguageSelectionTest` (10+ pure JDK
locale/override cases) and `scripts/check_app_language_wiring.py`, both
run by `sh scripts/verify-zh-tw.sh`. A successful APK build still must
be followed by checking both English and Traditional Chinese UI,
help, list overlays and game display on an emulator/phone.

## Status

- `values-zh-rTW/strings.xml` now contains **260 translated string resources** (196 existing + 44 hardware-key labels + 5 installer errors + 7 editor/quantity/opacity captions + 8 floating-button editor captions), and **8 display-only arrays** (50 localized items).
- In `res/xml/preferences.xml`, **99 hard-coded UI labels and summaries** now reference localized strings. The remaining 67 hard-coded labels are literal keyboard letters/digits/symbol keycaps (deliberately unchanged).
- Localized launcher buttons, profile editor hints/buttons, profile context actions, localized profile validation, dialogs, in-game quick settings, storage manager buttons/messages, special mouse-button popup, hardware-key mapping and reset screens, floating-button/keymap editors, quantity/opacity controls, and installation/save-migration errors.
- The Chinese HTML guide is `app/src/main/assets/help_zh_tw.html`; `HelpActivity` selects it for the `zh-TW` device locale, retaining the upstream `help.html` as English fallback.
- Split D-Pad colors, quick-key popup position and storage location display arrays from their stable persisted `entryValues`.
- Source-level checks confirm that added Java/XML string references resolve to English fallback resources and that the new hardware-key, installer and editor captions have zh-TW values. The verifier also compares formatting placeholders and preference array sizes, and rejects new hard-coded English captions in the five edited layouts. Persisted entryValues, action IDs and command keycaps remain untouched. This does not establish Android runtime behavior.
- A reusable Python 3 verifier is available: `python3 scripts/check_zh_tw_resources.py`. It checks Java/XML resources, array pairs, placeholders, preferences, localized help images, hardware-key descriptions, installation errors and floating-button editor captions. **This script has not yet been executed on a full local checkout.**
- No changes to settings persistence, internal variant IDs, saved data formats or native gameplay rules. **The first native game-screen translation batch is now implemented**; monster lore, item grammar and most combat messages remain English.
- Local JDK 21 offline regression checks passed on a synchronized copy of the updated Java terminal and tests: **14 Unicode-width cases, 53 terminal-buffer assertions, and 15,000 seeded mixed operations**. The newer assertions check whole-lower-row clearing and JNI byte-length handling. The repository runner is `sh scripts/test-terminal-unicode.sh` and now includes buffer and stress tests. Android Gradle/NDK compilation, emulator testing and screenshot review have **not** been performed.
- Native C Unicode conversion now includes strict UTF-8 encoding and decoding, width-aware `Term_addstr()` / `Term_addch()`, continuation cells and marker-aware redraw hooks. Additional native row-fit, grapheme-boundary and redraw helpers are under test. The core **is still not considered fully Chinese-capable** without NDK compilation, end-to-end integration and layout validation.

## Android UI localization update (2026-11-03)

- `KeyMapPreference.java` and `KeyMapResetPreference.java`: translated the capture/reset dialogs, back-key confirmation, unassigned summary and 38 human-readable hardware key names. Real key codes, `KeyMap.stringValue()` and stored preference identifiers have not changed. Controller keycap names (`L1`, `L2`, `R1`, `R2`) are still displayed as standardized codes.
- `Installer.java`: translated user-visible file installation, external-storage and old-save migration failures using Android resources, preserving filenames and exception details via `%1$s`.
- `keymapeditor.xml`, `adv_button_options.xml`, `quantity_layout.xml`, `opacity_layout.xml` and `fab_crud.xml`: moved user-facing editor captions and hints to localized resources. `Ret`, `Esc`, `Space`, `Tab`, digits and game command symbols remain literal keycaps to avoid affecting the keyboard/game command protocol.
- `AngbandDialog.java`: warning/fatal-dialog OK buttons now use the existing localized `action_ok` resource.
- **Source review:** GitHub XML source currently has 269 English fallback string definitions, 260 zh-TW translations, and eight translated display-only arrays. All 64 new strings have matching English/zh-TW entries; no duplicate resource keys or unlocalized prose captions were found in the five edited layouts.
- **Pending acceptance:** run `sh scripts/verify-zh-tw.sh --android` on a JDK 21 / Gradle 9.1 / AGP 9.0.1 workstation; verify hardware-key reset/assignment, installer error handling, and editor layout text on a zh-TW emulator. Native game text, monster descriptions and item names are not translated by this stage.

## First native Angband game-text batch (2026-11-03)

Native Angband now uses a **cached display-only Traditional Chinese locale flag** obtained from Java at game startup. The original English screen text remains the fallback.

- The character death menu has **9 translated option labels** and **11 translated prompts**. Command tags and callbacks remain unchanged.
- Monster and item list headings, counts and monster sleep status have Chinese variants, without changing the original object or monster lists.
- **128 ordinary monster display names** are translated only by `get_mon_name()` in the monster list; unique monsters and general combat narration remain English.
- **76 known object-kind display names** are translated only by `object_kind_name()`. Unidentified item flavors, inventory grammar, item descriptions and actual `object_desc()` behavior remain unchanged.
- These English names were verified against Angband **4.2.4** upstream `monster.txt`/`object.txt`. Unknown names fall back to the original text. Neither `zipangband` nor any saved data is modified.
- Added pure Java and native C test cases plus `scripts/check_native_game_localization.py` to the one-command check suite. Native monster/item list lines now use terminal display-column helpers to clip UTF-8 safely and account for two-column glyphs during padding. **Full Android NDK build, bundled zip-data match, visual alignment and emulator rendering remain unverified.**

See [NATIVE_GAME_ZH_TW.md](NATIVE_GAME_ZH_TW.md) for all implementation boundaries, tested inputs and follow-up work.

## Essential localization rules

- Translate **display labels only**. Never translate keys, serialized values, command keystrokes, plugin identifiers, or game data identifiers used by loaders.
- Some original preference menus used the same array for `android:entries` and `android:entryValues`. We separated the display labels for `storageLocations`, `DPadColors` and `FastKeysPopupPosition`; the persisted English strings are unchanged. Continue applying this separation rule to any future editable options.
- Keep English as the fallback locale. Use Android `values-zh-rTW` for Taiwanese Traditional Chinese. Verify device locale behavior.
- Preserve game names, artists' names, licenses, and attribution.

## Next implementation steps

1. **Done (first pass):** extract and translate preference title/summary labels, leaving keyboard keycap literals and persistent values intact.
2. **In progress:** continue extracting remaining Java UI text (including remaining custom input controls, file-system exception messages and other interactive screens). `ProfileList` validation, `StorageActivity` and the main `GameActivity` quick settings have been handled.
3. **Done (first pass):** translated Android help HTML with locale-aware loading and English fallback. Review text and screenshots on a device.
4. **In progress:** Java reserves and draws two cells for common East Asian wide glyphs. The base Angband native terminal now also accounts for two columns in `Term_addstr()` and `Term_addch()` and uses marker-aware redraw callbacks. **Direct cell placement, map/UI layout, partial redraw and actual NDK/device compatibility still need validation and further fixes.** See [`docs/CJK_RENDERING_AUDIT.md`](CJK_RENDERING_AUDIT.md).
5. Catalog and translate the Angband core's user-visible strings and suitable entries in `variant_lib/angband/gamedata`. Do not translate identifiers or data syntax. Treat other included variants as separate projects.
6. Build with the project's Android SDK / NDK requirements and test the app with a zh-TW device locale, then conduct a gameplay smoke test and save compatibility check.
7. Review distribution rights and the current Google Play submission requirements before preparing an independent application ID, branding, disclosures, and a release.

## Fixed-position map grid safety (2026-11-03)

The native grid now distinguishes **streamed terminal text** from **fixed-coordinate map cells**. `Term_addstr()` / `Term_addch()` continue to use two columns for Chinese text, while `Term_draw()` / `Term_queue_chars()` preserve their fixed cell positions and replace otherwise overflowing wide map glyphs with `?`. Existing tile indices, direct-cell terrain clearing, and private-use control characters are preserved. The native `Term_what()` hides the internal continuation marker from game logic; literal U+FDD0 input is also sanitized. Tests and static source checks cover the behavior, but Android compilation and device rendering are **not yet verified**.

## Mobile mouse controls (2026-11-03)

The special mouse-button popup now displays its labels from the `mouse_input_labels` resource array. Its eight commands are defined by stable numeric option IDs in `MouseInputOptions.java`, **not by searching localized labels for `Shift`, `Ctrl`, `Alt` or `2`**. This also fixes the old `Shitf-1` typo that prevented the first Shift mouse action from working correctly. The pure-Java `MouseInputOptionsTest` passes 19 mapping/invalid-input assertions and runs from `scripts/test-terminal-unicode.sh`. Device touch verification remains pending.

## Recent renderer fixes

The Android terminal now clears every lower-row column in `clrtobot()`, respects the explicit JNI byte length in `addnstr()`, and prevents an unplaceable wide glyph from moving into the next row. Native partial redraw preserves unrelated pending screen changes through `angdroid_union_dirty_range()`. The native helper passed seven isolated host C99 union tests; the full Gradle/NDK application and real handset are still untested.

## CJK input concerns

The profile editor's `res/layout/profileadd.xml` has restrictive `android:digits` attributes on the profile name and savefile inputs. Their translated placeholder labels do not yet imply support for Chinese-character profile names. Review name validation and on-disk savefile compatibility separately before allowing CJK characters in editable names.

## Verification commands

Run from repository root:

```bash
# Offline checks only
sh scripts/verify-zh-tw.sh

# Optional: include debug APK Gradle/NDK compilation with JDK 21
# and installed Android SDK Platform 35 on a configured Mac
sh scripts/verify-zh-tw.sh --android
```

This writes `zh-tw-validation.log` in the repository root (gitignored). To run individual stages: `python3 scripts/check_zh_tw_resources.py`, `sh scripts/test-terminal-unicode.sh`, `sh scripts/test-native-utf8.sh`, or `python3 scripts/check-native-unicode-contract.py`.

These offline checks require Python 3, a JDK and a host C compiler. They check resources, Unicode terminal buffers, native encoding/width helpers and protocol source wiring. **Without `--android`, no APK is compiled. With `--android`, Gradle/NDK debug compilation is attempted, but this still does not validate emulator/physical-device gameplay or glyph positioning.**

## Build notes

**JDK 21 migration (2026-11-03):** the project now uses Gradle **9.1.0**, Android Gradle Plugin **9.0.1**, the **JDK 21** toolchain, and `compileSdk 35`. Java source/bytecode stays at **17** for Android compatibility; `targetSdk 34`, `minSdk 22`, NDK `28.2.13676358` (AGP 9 r28c), CMake `4.1.2` (installed externally via PATH or `cmake.dir`); full NDK build compatibility remains to be verified. Debug native builds default to `arm64-v8a` and can optionally use `-PABI_FILTERS=x86_64` (or the legacy `local.properties` setting). Android Studio setup details, AGP 9 legacy resource-ID compatibility flags and Gradle wrapper regeneration notes: [JDK21_BUILD.md](JDK21_BUILD.md). This is not yet a confirmed Gradle/NDK or Android-emulator build, nor a claim of current Google Play compliance.

## Licensing notes

The repository root has no consolidated `LICENSE` file. Some Android Java and native Angband source files declare a GPL v2 **or** original Angband-license alternative, while other source files carry distinct notices. The Android repository also bundles multiple variant engines and third-party assets. An overall right to repackage and redistribute this specific app must **not** be assumed solely from the Angband core license.
