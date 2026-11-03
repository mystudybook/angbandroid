# Traditional Chinese (Taiwan) localization plan

This is an **unofficial** localization of the Android Angband port. The intended end goal is to offer a free Traditional Chinese edition, but **publishing is not yet approved or verified**. Preserve original attribution and review the license of every bundled component (including variants, artwork, fonts, and Android port code) before distributing binaries.

## Status

- `values-zh-rTW/strings.xml` now contains **196 translated string resources** (40 initial + 99 preferences + 15 launcher/profile/dialogs + 21 game quick settings + 17 storage messages + 4 profile validation messages), and **7 display-only arrays** (42 localized items).
- In `res/xml/preferences.xml`, **99 hard-coded UI labels and summaries** now reference localized strings. The remaining 67 hard-coded labels are literal keyboard letters/digits/symbol keycaps (deliberately unchanged).
- Localized launcher buttons, profile editor hints/buttons, profile context actions, localized profile validation, basic dialogs, in-game quick settings, and storage manager buttons/messages.
- The Chinese HTML guide is `app/src/main/assets/help_zh_tw.html`; `HelpActivity` selects it for the `zh-TW` device locale, retaining the upstream `help.html` as English fallback.
- Split D-Pad colors, quick-key popup position and storage location display arrays from their stable persisted `entryValues`.
- Source-level verification checked **186 Java/XML string references** and **18 array references** with no unresolved names; translated array sizes and formatting placeholders match English defaults. All twelve preference entry/entryValues array pairs inspected have matching item counts; persisted value arrays remain untranslated. These are not Android runtime tests.
- A reusable Python 3 verifier is available: `python3 scripts/check_zh_tw_resources.py`. It checks Java/XML resources, array pairs, placeholders, preferences and localized help images. **This script has not yet been executed on a full local checkout.**
- No changes to settings persistence, internal variant IDs or native gameplay rules. Unicode text rendering is now being improved in the Android Java terminal layer; no native game text translations have been made.
- Java 21 offline tests passed on a manually transferred copy of the committed classes: **14 Unicode-width cases, 33 terminal-buffer assertions, and 15,000 seeded mixed operations**. The repository runner is `sh scripts/test-terminal-unicode.sh` and now includes buffer and stress tests. Android Gradle/NDK compilation, emulator testing and screenshot review have **not** been performed.
- Native C Unicode conversion now includes strict UTF-8 encoding and decoding, width-aware `Term_addstr()` / `Term_addch()`, continuation cells and marker-aware redraw hooks. Additional native row-fit, grapheme-boundary and redraw helpers are under test. The core **is still not considered fully Chinese-capable** without NDK compilation, end-to-end integration and layout validation.

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

## CJK input concerns

The profile editor's `res/layout/profileadd.xml` has restrictive `android:digits` attributes on the profile name and savefile inputs. Their translated placeholder labels do not yet imply support for Chinese-character profile names. Review name validation and on-disk savefile compatibility separately before allowing CJK characters in editable names.

## Verification commands

Run from repository root:

```bash
# Offline checks only
sh scripts/verify-zh-tw.sh

# Optional: include debug APK Gradle/NDK compilation on a configured Mac
sh scripts/verify-zh-tw.sh --android
```

This writes `zh-tw-validation.log` in the repository root (gitignored). To run individual stages: `python3 scripts/check_zh_tw_resources.py`, `sh scripts/test-terminal-unicode.sh`, `sh scripts/test-native-utf8.sh`, or `python3 scripts/check-native-unicode-contract.py`.

These offline checks require Python 3, a JDK and a host C compiler. They check resources, Unicode terminal buffers, native encoding/width helpers and protocol source wiring. **Without `--android`, no APK is compiled. With `--android`, Gradle/NDK debug compilation is attempted, but this still does not validate emulator/physical-device gameplay or glyph positioning.**

## Build notes

`app/build.gradle` currently reads `ABI_FILTERS` from the developer's `local.properties`. Its configuration references `compileSdkVersion 34`, `targetSdk 34`, `ndkVersion "21.0.6113669"`, and external CMake. These are existing upstream settings, not a statement that the app currently meets Google Play's latest requirements.

## Licensing notes

The repository root has no consolidated `LICENSE` file. Some Android Java and native Angband source files declare a GPL v2 **or** original Angband-license alternative, while other source files carry distinct notices. The Android repository also bundles multiple variant engines and third-party assets. An overall right to repackage and redistribute this specific app must **not** be assumed solely from the Angband core license.
