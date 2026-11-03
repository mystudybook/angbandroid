# Traditional Chinese (Taiwan) localization plan

This is an **unofficial** localization of the Android Angband port. The intended end goal is to offer a free Traditional Chinese edition, but **publishing is not yet approved or verified**. Preserve original attribution and review the license of every bundled component (including variants, artwork, fonts, and Android port code) before distributing binaries.

## Status

- `values-zh-rTW/strings.xml` contains **154 translated string resources** (40 initial + 99 preference UI + 15 launcher/profile/dialog strings) and **4 translated display-only arrays** (28 items).
- In `res/xml/preferences.xml`, **99 hard-coded UI labels and summaries** now reference localized strings. The remaining 67 hard-coded labels are literal keyboard letters/digits/symbol keycaps (deliberately unchanged).
- Localized launcher buttons, profile editor hints and buttons, profile context actions, several profile dialogs and the help activity title.
- Static source checks verified all 140 observed Java/XML `R.string` and `@string` references exist in the English defaults, translated keys exist in English, array sizes agree and literal percentage strings have `formatted="false"`. These checks are not an Android compilation.
- A reusable Python 3 static verifier is available: `python3 scripts/check_zh_tw_resources.py`. **It has been added but not executed in an Android build environment yet.**
- No changes to settings persistence, internal variant IDs, gameplay logic, or asset bundles.
- Compilation, emulator testing, and screenshot review have **not** been performed.

## Essential localization rules

- Translate **display labels only**. Never translate keys, serialized values, command keystrokes, plugin identifiers, or game data identifiers used by loaders.
- Some existing preference menus use the same resource array for `android:entries` and `android:entryValues` (e.g. `storageLocations`, `DPadColors`, and `FastKeysPopupPosition`). Do **not** translate these arrays in place: split display entries and stable entry values before localizing them.
- Keep English as the fallback locale. Use Android `values-zh-rTW` for Taiwanese Traditional Chinese. Verify device locale behavior.
- Preserve game names, artists' names, licenses, and attribution.

## Next implementation steps

1. **Done (first pass):** extract and translate preference title/summary labels, leaving keyboard keycap literals and persistent values intact.
2. **In progress:** extract hard-coded English UI text from remaining Java classes (notably `StorageActivity`, `GameActivity`, and profile-validation messages).
3. Translate `app/src/main/assets/help.html` with an explicit locale-aware loading strategy.
4. Validate Unicode/CJK display in native Angband: its core uses UTF-8 and wide characters in parts of the rendering pipeline, but the Android terminal currently uses bundled monospace fonts such as `VeraMoBd.ttf` and `6x13.ttf`. Test missing glyphs, display width, wrapping, clipping, and input before translating gameplay strings.
5. Catalog and translate the Angband core's user-visible strings and suitable entries in `variant_lib/angband/gamedata`. Do not translate identifiers or data syntax. Treat other included variants as separate projects.
6. Build with the project's Android SDK / NDK requirements and test the app with a zh-TW device locale, then conduct a gameplay smoke test and save compatibility check.
7. Review distribution rights and the current Google Play submission requirements before preparing an independent application ID, branding, disclosures, and a release.

## CJK input concerns

The profile editor's `res/layout/profileadd.xml` has restrictive `android:digits` attributes on the profile name and savefile inputs. Their translated placeholder labels do not yet imply support for Chinese-character profile names. Review name validation and on-disk savefile compatibility separately before allowing CJK characters in editable names.

## Build notes

`app/build.gradle` currently reads `ABI_FILTERS` from the developer's `local.properties`. Its configuration references `compileSdkVersion 34`, `targetSdk 34`, `ndkVersion "21.0.6113669"`, and external CMake. These are existing upstream settings, not a statement that the app currently meets Google Play's latest requirements.

## Licensing notes

The repository root has no consolidated `LICENSE` file. Some Android Java and native Angband source files declare a GPL v2 **or** original Angband-license alternative, while other source files carry distinct notices. The Android repository also bundles multiple variant engines and third-party assets. An overall right to repackage and redistribute this specific app must **not** be assumed solely from the Angband core license.
