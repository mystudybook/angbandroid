# Traditional Chinese (Taiwan) localization plan

This is an **unofficial** localization of the Android Angband port. The intended end goal is to offer a free Traditional Chinese edition, but **publishing is not yet approved or verified**. Preserve original attribution and review the license of every bundled component (including variants, artwork, fonts, and Android port code) before distributing binaries.

## Independent app language (2026-11-03)

The launcher also displays **Language / 語言** after the startup installer completes, allowing language changes before a game begins. The launcher refreshes its resources after a selection.

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


## Android 數量選擇視窗與輸入防呆（第 91 批）

- 數量選擇視窗新增繁中動態提示、「確認數量」、「取消」與明確「全部（上限）」標籤；1、10、全部仍可直接選取送出。
- 轉動 NumberPicker 或拖曳滑桿僅改變視窗顯示，**不再反覆注入尚未確認的遊戲按鍵**。按「確認數量」才送出數字加 Enter；按取消、Android 返回或關閉未確認視窗時送 Esc。
- 視窗可取得焦點且不允許外部點擊穿透，避免在選擇數量時操作遊戲。Activity 暫停時關閉視窗。
- 使用純 Java 長整數中介計算，避免滑桿換算時的整數溢位、零區間除以零，並限制初始化數量在合法範圍。
- 離線驗證：`sh scripts/test-quantity-popup.sh`（純 JDK）及 `python3 tests/test_quantity_popup_source.py`（Android 程式與資源連線）。完整 Android 編譯、模擬器觸控與多遊戲核心的端到端驗收仍需本機執行。未使用 GitHub Actions。


## Android 數量輸入原子佇列（第 92 批）

- `QuantityPopup` 按確認或「1／10／全部」時只呼叫 `StateManager.enqueueQuantity()`，由 `QuantityInputSequence` 先建立完整的游標移動／清除舊數字／輸入數字／Enter 序列，再由 `KeyBuffer.addRawSequence()` 在**同一把 queue lock**下全部排入，防止其他 UI 執行緒的按鍵插入**序列中間**。
- 保留既有 FrogComposband 專用的 15 次右移、15 次 Backspace、原生 Enter 編碼和其他遊戲版本的數量規則；不更動商店價格、背包物件、核心資料或存檔。合成數字使用 ASCII，跳過使用者鍵盤修飾鍵，不會消耗 Ctrl／Shift。
- 排隊前驗證 0 至 max 的範圍、整數及必要的原生按鍵編碼；若驗證失敗，不會把視窗標記成確認成功。取消／Back 仍只送出 Esc。
- **限制：** 原子排入只保證整組按鍵無法被其他排入操作插開，**不會**清掉當時已在佇列的按鍵，也無法保證原生遊戲此刻仍停留在數量輸入狀態。清空共用 queue 可能破壞儲存／退出等訊號，因此未採用。仍需 Android 模擬器／實機確認快速連點、跨畫面與多遊戲核心互動。
- 純 JDK 測試 `tests/java/org/rephial/xyangband/QuantityInputSequenceTest.java`，由 `sh scripts/test-quantity-popup.sh` 執行；Python 原始碼檢查 `python3 tests/test_quantity_queue_source.py` 已加入 `scripts/verify-zh-tw.sh`。未使用 GitHub Actions。


## 原生數量視窗生命週期同步（第 93 批）

原生 Angband 的 `textui_get_quantity()` 在有效輸入、重填後有效輸入、Esc 取消時都會送出明確的 `TERM_CONTROL_QUANTITY = "end"`，避免原生遊戲已離開數量欄而 Android 仍保留可操作的視窗。數量上限 ≤ 1 時原本就不會開啟視窗，無須送結束通知。

- 在原生 callback 執行緒同步處理每次 start/end，指派不同的 `QuantityPromptGate` token，然後以 `Message.arg2` 將 token 傳給 Android 主執行緒。已過期的 start 通知不再開視窗；延遲到達的 end 通知只會關閉其所屬視窗。
- `QuantityPopup` 的「確認數量」、1／10／全部、取消、返回與程式關閉行為都攜帶 session token；`StateManager` 在同一個 token 鎖內檢查有效性並寫入 `KeyBuffer`，無效操作不會送入 Enter／Esc。 每次原生提示皆建立**新的 Android PopupWindow**，避免舊視窗延遲的 dismiss callback 被綁到新 token。
- 遊戲執行緒退出或重設按鍵佇列時也會將原有 token 作廢，防止跨遊戲／重新啟動後的舊視窗繼續寫入新的佇列。 收到 `OnGameExit` 時，Android 同時以靜默方式關閉殘留視窗。
- 若玩家透過原生遊戲鍵盤自行完成數量輸入，結束通知只關閉 Android 視窗，**不會**再補送 Esc；Activity 暫停時仍維持原本取消目前輸入的策略。
- 原有按鍵佇列不會被清空。**本批仍不能撤銷已經排入、但尚未由原生遊戲讀取的按鍵**；若希望避免這類殘留，後續需要具有 session 標籤的佇列項目與原生消費端驗證。也未變更商店價格、背包資料、存檔或遊戲玩法。
- 此次原生 `"end"` 通知只新增於**主要 Angband 核心**；其他獨立 variant 仍有 Android token 的「單次確認／取消」保護，但尚未全面加入其原生提示結束事件，需分別審查。
- 離線驗證：`sh scripts/test-quantity-popup.sh` (新增 `QuantityPromptGateTest`)，`python3 tests/test_quantity_prompt_session_source.py`，以及 `sh scripts/verify-zh-tw.sh --android zh-tw-validation-93.log`。最後的完整 Android 編譯、模擬器／實機操作仍須在具備 SDK 的環境驗證。本批不使用 GitHub Actions。


## 數量佇列 session 標記與 FAangband 結束事件（第 94 批）

- 新增純 Java `QuantityKeyQueue` 作為 `KeyBuffer` 的 FIFO 容器。一般輸入、巨集 delimiters（-200）、存檔訊號（-1）都帶 **token 0**；Android 數量對話框產生的數字、Backspace、Enter 及 Esc 才帶上原生提示的 token。
- `QuantityPromptGate.resolve()` 僅禁止 UI 重複確認或取消，**不立即丟棄**已提交的數量按鍵。只有原生下一次 `TERM_CONTROL_QUANTITY` start／end、遊戲退出或輸入佇列重設時，才更新佇列 token 並移除舊的數量按鍵。原生遊戲已經讀取的按鍵不可能撤銷；尚未讀取的過期輸入則會被丟棄。
- `KeyBuffer.get(v)` 在原有 `keybuffer` 鎖內依序讀取有效按鍵；遇到過期 token 會跳過；blocking 模式繼續等待新有效輸入，保留遊戲停止特殊鍵優先處理、一般快捷鍵和既有 save 覆寫行為。更換 token 不會粗暴清空正常鍵或巨集內容。
- FAangband 的 `textui_get_quantity()` 現在在成功與 Esc 取消時均送出 `TERM_CONTROL_QUANTITY: "end"`，使舊 popup 和過期數量按鍵能立即失效。**FAangband 的既有數量解析（如字母代表全部）仍照舊**，本輪只修正原生 Android 控制事件，不改其數量運算及角色／存檔資料。
- **注意：** 其他獨立 native variant 仍須逐一核實其 `Term_control_quantity` 用法、原生終了事件和返回流程。本批的 FIFO 保護會阻止上一個 session 的殘留數量序列流入下一個 session，但**無法判別先前排入的普通按鍵是否也已過時**；消費端已讀走的按鍵也不能回溯。這不是整體遊戲交易的原子化。
- 離線測試：`QuantityKeyQueueTest.java` 對 FIFO、原生通知、只移除 tagged keys、巨集／存檔、連續切換與部分消費進行純 JDK 測試；`test_quantity_tagged_queue_source.py` 驗證 Java 串接與 FAangband 原生程式碼。均納入 `scripts/verify-zh-tw.sh`，不需 GitHub Actions。完整 APK／NDK 編譯及 Android 模擬器／實機驗證仍需本機進行。


## 第 95 批 — FrogComposband 數量提示結束與更多繁體中文

- **FrogComposband**：在 `get_quantity_aux()` 完成原生數量輸入後、Esc 取消後均送出 `TERM_CONTROL_QUANTITY = "end"`。訊息在 native prompt 清除之後、數量結果傳回之前送出，讓第 93–94 批的 session gate 與 tagged FIFO 能清除尚未讀取的過期數量鍵；`command_arg` 和 `repeat_pull` 這兩條不曾開啟 popup 的快速路徑不發送 end。
- 原有 FrogComposband `atoi`、字母代表全部、上限／下限、重複指令和存檔格式保持不變。這一批只補生命週期，不擅自套用主要 Angband 的嚴格數量解析規則。
- **Android 共用數量視窗繁體中文**：對 FrogComposband 預設 `Quantity (1-N): ` 和 FAangband／英文 Angband 預設 `Quantity (0-N, *=all): `，僅在提示原文與原生 max 完全一致時才使用 Android locale strings 顯示「選擇數量（1～N）」或「選擇數量（0～N，* 表示全部）」。其他商店／背包自訂提示保留原文，未變動遊戲命令或數量序列。英文資源提供等價提示。
- **主要 Angband 原生繁體中文**：原本字典已有「正在儲存為 %s。」但顯示端未使用，現已串接；新增操作次數 `Repeat: %d` →「重複次數：%d」，兩者保留原來 `%s`／`%d` 參數與英文 fallback，僅改顯示訊息。
- JDK 21 可獨立執行 `QuantityPromptTextTest`，由 `scripts/test-quantity-popup.sh` 整合；原生字典 `tests/native/test_game_ui_locale.c` 增加映射驗證；Python 來源保護 `tests/test_frog_quantity_locale_source.py` 已加入 `scripts/verify-zh-tw.sh`。
- **範圍限制**：FrogComposband 原生終端內的其他大量英文文字尚未整體翻譯，這次先改 Android 彈窗及主要 Angband 的兩項資訊列；其它 native variant 亦不保證已有完整 start/end 事件。完整 APK／NDK、模擬器與實機操作仍需本機驗收，不使用 GitHub Actions。


## NPPAngband 7.1.0／Sil-Q 數量輸入生命週期與繁體中文化（第 96 批）

- 兩個核心都會送出原生 `Term_control_quantity` 開始事件；本批補上成功輸入和 Esc 取消時的 `TERM_CONTROL_QUANTITY: "end"`，讓 Android 關閉對應的舊視窗、清理尚未讀取的數量按鍵。未開啟數量視窗的指令參數及重複操作分支不會送出新的 end。
- NPPAngband 7.1.0 原先在數量輸入取消時直接 return，可能留下暫時性的 `[ALL]` 按鈕；現在取消路徑一併 `button_kill('a')` 並通知 `EVENT_MOUSEBUTTONS`。
- Android 數量視窗對 NPP 的 `Quantity (0-N, '*' or 'a' = all): ` 和 Sil-Q 的 `Quantity (0-N): ` 新增精確原文字串及數量上限匹配，並提供各自繁中顯示資源。核心英文輸入文字及解析規則不變；自訂商店、裝備或任務提示不會被任意改寫。
- Sil-Q 既有「非數字表示全部」、NPPAngband 的 `*`／字母代表全部、數量上下限與重複命令行為均保留。NPP 0.5.4 並未使用數量 popup start callback，本批不任意替它加入 end。
- 已增加 `tests/test_variant_quantity_session_source.py`、JDK `QuantityPromptTextTest` 正反比對案例，並整合 `scripts/verify-zh-tw.sh`。這些測試不等於完整 APK／NDK 或模擬器、實機驗證。未使用 GitHub Actions。


## NPPAngband 0.5.4 數量視窗生命週期與繁中化（第 97 批）

- 0.5.4 先前提供一般軟鍵盤與數量字串，但**沒有**呼叫原生 `Term_control_quantity` 開始事件；因此 Android 數量選擇視窗無法使用。現在僅在原有 `max != 1` 且沒有 `command_arg` 的互動輸入路徑送出 start，並在成功或 Esc 取消後送出 `TERM_CONTROL_QUANTITY: "end"`，保護前一數量 session 的未讀取按鍵。
- 將 `soft_kbd_linger("0123456789*")` 換成 Android 可辨識的 `${quant}*` 標記：設定開啟 popup 時顯示 Android 數量視窗；關閉 popup 時仍可顯示既有數量軟鍵與 `*`。**不修改**原生數量解析（`*`／英文字母代表全部）、上限、一般鍵盤輸入或存檔。
- NPP 0.5.4 取消輸入時原本可能遺留 `[ALL]` 快捷鍵；現在取消分支也執行 `button_kill('a')` 與 `EVENT_MOUSEBUTTONS`，而正常結束仍沿用原清理。
- 0.5.4 與 7.1.0 使用相同的原生 `Quantity (0-N, '*' or 'a' = all): ` 格式，因此直接重用第 96 批的精確比對與繁中 `quantity_prompt_npp_all` 資源，不建立多餘翻譯，也不翻譯自訂商店／裝備問題。
- Python `tests/test_variant_quantity_session_source.py` 擴大涵蓋 0.5.4 的開始／完成／取消與臨時按鍵清理；已由 `scripts/verify-zh-tw.sh` 呼叫。尚未聲稱 Android APK／NDK 完整編譯或模擬器／實機已通過。
- **NPPAngband 0.4.1 限制**：原生檔案目前使用 `Term_control_android` 私有回呼，且 CMake 未整合與新版相同的 `droid.c` 控制橋接；不應直接複製第 97 批新增的 `Term_control_quantity` 呼叫而不先修復連結及控制協定。下一輪需要獨立評估。


## NPPAngband 0.4.1 Android 控制橋接及數量中文化（第 98 批）

- **控制橋接**：0.4.1 原本只有 `static Term_control_android()`，`CMakeLists.txt` 沒有編譯 `../curses/droid.c`，不能直接使用其他核心的 `Term_control_quantity`。現在透過 `droid.h` 將舊 callback 對外命名為 `Term_control`（仍保留原本的 `IN_THE_DUNGEON` context／`HAS_KEYMAP_PACK` 邏輯），並把共用 `droid.c` 納入 **npp041** native library。原有游標控制改走同一個 `Term_control` 函式。
- **原生數量 session**：只在 `max != 1 && allow_quantity`、未由 `command_arg` 或 `ALLOW_REPEAT` 決定數量的互動路徑送出 `Term_control_quantity(prompt,max,amt)`；成功或 Esc 取消都會在提示結束時送出 `TERM_CONTROL_QUANTITY: "end"`。既有的字母「全部」、`atoi`、上下限及重複指令操作不變。
- **快速鍵與 popup 模式**：在原本 `inkey()` 的主畫面等待輸入前呼叫 `soft_kbd_flush()`，並在 keymap 早退時清除非持續鍵盤；數量輸入用 `${quant}`，於 Android 設定開啟 popup 時隱藏暫時的數字快速鍵，關閉時仍展開為 `0123456789`。成功或取消後強制清理暫時軟鍵。
- **避免錯誤中文提示**：0.4.1 和 Sil-Q 原文都是 `Quantity (0-N): `，但 0.4.1 只有**英文字母**代表全部，Sil-Q 則是**非數字**代表全部。純 Java 分類器增加 NPP041 專用辨識參數，`QuantityPopup` 由目前 Plugin 決定翻譯，因此不會誤用 Sil-Q 的文字；自訂的任務／商店提示及英文原始命令不更動。
- **驗證界線**：新增 `tests/test_npp041_quantity_bridge.py`，檢查 CMake、callback 符號、原生 session、繁中資源、舊數量規則，並以 host C99 單獨編譯真實 `Term_control` 函式及共用 `droid.c` 的控制邏輯。擴充 `QuantityPromptTextTest`，將兩項檢查納入 `scripts/verify-zh-tw.sh`。這**不能取代**完整 Android Gradle／NDK shared library 連結、模擬器或實機驗收。未使用 GitHub Actions。


## 第 99 批 — 背包、裝備子視窗與數字欄位繁體中文

- **原生 Angband 4.2.5 物品列表**：將背包／裝備／商店物品清單中原本顯示的 `au` 金額、`fail` 失敗率（已知與未鑑定）、充能失敗率與 `lb` 重量，用繁體中文的「金」「失敗」「磅」顯示。每欄仍由既有 `object_list_put_detail()` 判斷完整數字能否放入原本的 **9／10 個終端格**，超寬數值一律不輸出半截數字／半個中文字。英文介面維持原字串。
- **背包與箭袋**：背包子視窗的 `Burden … lb` 改為「負重 … 磅（剩餘／超出 … 磅）」；處理 `remaining == INT_MIN` 時不再呼叫可能溢位的 `abs(INT_MIN)`。裝備子視窗的 `In quiver` 只在繪製時顯示「箭袋內」，保留資料中的英文 `In quiver` 哨兵字串，避免更動 `set_obj_names()` 的判斷、實際欄位索引或物品資料。箭袋欄位 `Slot n` 以「格 n」顯示；子視窗中彈藥數量改為「箭袋：N 支彈藥」，與既有主要物品選單一致。 另外，子視窗箭袋摘要列現在檢查有效列數、快捷鍵索引及欄寬，用 UTF-8 終端格截斷顯示文字，避免手機旋轉或極窄視窗在底部畫出界；若 `quiver_slot_size` 不合理則略過該摘要。
- **安全設計**：新增純 C99 的 `inventory-list-display.h`，文字緩衝區不足會返回失敗並清空輸出；超出預留欄位的整段數字會由原有版面函式略過。確認選取標籤、`object_desc()`、`get_item()`、購物／出售／存放／取出交易命令與存檔格式都沒有更動。
- **回歸驗證**：`tests/native/test_inventory_list_display.c` 測試中英數字、固定終端欄寬、狹窄緩衝區、超長整數、過重與 `INT_MIN`；在 `scripts/test-native-utf8.sh` 使用 host C99 編譯。`tests/test_inventory_list_locale_source.py` 檢查 source-only 顯示、原本的辨識哨兵、物品選擇與商店指令均不變，納入 `scripts/verify-zh-tw.sh`。這些測試不代表 APK／NDK 或模擬器實機已完整驗收；不使用 GitHub Actions。


## 第 100 批 — 七種商店名稱與刻字安全確認的繁體中文化

- **商店右側標題**：原版 Angband `terrain.txt` 定義的七種商店名稱（General Store／Armoury／Weapon Smiths／Bookseller／Alchemy Shop／Magic Shop／Black Market）分別顯示為「雜貨店／防具店／武器店／書店／鍊金商店／魔法商店／黑市」；保留 `Home` →「家」的純顯示映射。只在 `ui-store.c` 繪製店名時查詢 `zh-tw-store-names.h`，保留 `f_info[]`、遊戲解析器、`FEAT_`、`store->feat`、店主姓名、商店金額 `max_cost` 及交易規則，英語或未知商店名稱原樣回傳。寬度繼續使用原本 UTF-8 欄位計算，不與店主資訊重疊。
- **物品安全警示**：物品刻字 `!*` 或 `!key` 的確認畫面原本顯示 `Really <verb> <item>?`，現在以 `zh-tw-inscription-confirm.h` 精確匹配 `cmd_verb()` 原始動詞，將 21 種常見動作（裝備、卸下、丟棄、使用、飲用、投擲、出售、存放、購買、取出等）顯示成「確定要… <物品>？」。沒有命中的動詞仍顯示英文，不會影響 mod 自訂命令。只有確認畫面的 `verify_object()` 物品說明啟用 `ODESC_ZH_TW` 並擴充本機文字緩衝區；`check_for_inscrip` 的比對、`while (n--)` 逐次確認、取消結果、原生 `cmd_verb()` 及存檔都不更動。
- **回歸測試**：新增純 C99 `tests/native/test_store_inscription_labels.c`，以 `-Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security` 測試每種店名、21 個動詞的繁中與英文、未知名稱／動詞、NULL 與超短緩衝區，接入 `scripts/test-native-utf8.sh`。新增 `tests/test_store_inscription_locale_source.py` 對照 shipped `terrain.txt` 和 `cmd-core.c`、只修改顯示畫面、保留安全確認與交易指令，接入 `scripts/verify-zh-tw.sh`。沒有使用 GitHub Actions。完整 APK／NDK 編譯、模擬器／實機仍需本機驗證。


## 第 101 批 — 品質忽略動態選單與畫面匯出繁中化

- **物品忽略選單**：原版 Angband 的品質忽略對話框曾留下 `All bad Rings`、`All non-artifact Bows` 等英文動態選項；現在透過 `zh-tw-ignore-quality-action.h` 使用既有 `angdroid_game_ui_text()` 的「劣質／普通／優良／非神器」與各種物品類型翻譯，顯示成「忽略所有劣質戒指」等完整的繁體中文。以純 C99 的 `snprintf` 檢查容量；若不認識任一品質或物品分類（例如其他版本或 MOD），則**整句**保留英文，避免一半中文一半英文；緩衝區不足時顯示原有「依品質忽略物品」通用標題，不輸出不完整的 UTF-8。原有的 `IGNORE_THIS_QUALITY`、物品分類、品質閾值、選取鍵、`ignore_level[ignore_type] = ignore_value` 和存檔內容不變。
- **畫面匯出**：`ui-command.c` 的 `Dump as (H)TML or (F)orum text?` 改成「匯出格式：H HTML／F 論壇文字？」、`Include monster list?` 改成「要包含怪物清單嗎？」；失敗通知改成「畫面匯出失敗。」、成功則顯示「畫面已匯出為 HTML／論壇文字格式。」。設定為英文時仍顯示原文，原本的 `h`／`f` 按鍵、`dump.html`／`dump.txt`、`dump.prf`、怪物清單選項與真正輸出的 HTML/文字內容**沒有修改**。
- **測試**：新增 `tests/native/test_ignore_quality_action_locale.c`（以 C99 編譯兩種語言、已知與未知類型、NULL、超短緩衝區及匯出提示），納入 `scripts/test-native-utf8.sh`；新增 `tests/test_ignore_quality_export_source.py` 核對 `list-ignore-types.h`、`obj-ignore.c`、`zh-tw-ui.h` 的全部現有分類與品質名稱，檢查玩家操作、品質 ID、匯出按鍵及檔名未變，納入 `scripts/verify-zh-tw.sh`。未使用 GitHub Actions。獨立／原始碼檢查不能取代完整 Android NDK／Gradle 或模擬器／實機驗收。


## 第 102 批 — 遊戲版本畫面與忽略物品選擇提示中文化

- **遊戲版本畫面**：原版 Angband `ui-command.c` 的動態標題 `You are playing <buildver>. Type '?' for more info.` 現在顯示為「正在遊玩 <buildver>。按 ? 查看詳細資訊。」；英文模式保留原文、真實 `buildver`／`copyright` 資料不更動。新增純 C99 `zh-tw-version-header.h`，以 `snprintf` 產生完整字串並驗證緩衝容量，避免截斷 UTF-8 中文碼點。螢幕表頭緩衝區擴為 256 bytes；極長版本號無法完整放入時，只顯示「遊戲版本資訊」或英文「Version information」，不顯示部分版本字串。
- **忽略物品選擇**：`textui_cmd_ignore()` 的 `Ignore which item?` 改成「要忽略哪件物品？」；`You have nothing to ignore.` 改為「沒有可以忽略的物品。」。這兩個字串只經 `angdroid_game_ui_text()` 進行顯示端翻譯，英文模式仍用原字串。`get_item` 的 `CMD_IGNORE`、物品來源 `USE_INVEN | USE_QUIVER | USE_EQUIP | USE_FLOOR`、取消時的早退、品質忽略選單與存檔均不變。
- **避免數量視窗回歸**：商店已在舊批次完成的買入／取出數量提示保持原狀；`textui_get_quantity()` 的 `Term_control_quantity`／`"end"` 生命週期不修改，以免不必要地干擾 Android 的原生數量 Popup。
- **離線回歸測試**：新增 `tests/native/test_version_header_text.c`，對中／英文、NULL、格式字元、字元邊界（48 與 49 bytes）、過長版本號與 0 容量進行 host C99 檢查；已納入 `scripts/test-native-utf8.sh`。新增 `tests/test_version_ignore_pick_source.py` 驗證中文對應、版本／版權資料、忽略操作與數量 Popup 的安全邊界；已納入 `scripts/verify-zh-tw.sh`。本批未使用 GitHub Actions；獨立 C99 測試不能取代完整 Gradle／NDK／Android 模擬器或實機驗收。


## 第 103 批 — 裝備比較詳細視窗中的兩件物品中文化

- **修復原版裝備比較的漏譯**：`ui-equip-cmp.c` 的 `display_object_comparison()` 原本在選取第一件／第二件物品後，兩件物品的詳細標題仍以英文 `object_desc(..., ODESC_PREFIX | ODESC_FULL | ODESC_CAPITAL)` 顯示，而且雙物品畫面標題固定為 `Object comparison`。現在兩個物品名稱都可依當前語言附加 `ODESC_ZH_TW`；雙物品視窗標題使用既有 `angdroid_equip_cmp_text()` 顯示「裝備比較」，英文則維持 `Object comparison`。保留既有 `object_info(..., OINFO_NONE)`、兩個 `textblock` 的釋放和已辨識物品的知識限制；**不使用 `ODESC_SPOIL`，不可將未鑑定資訊顯示出來**。
- **UTF-8 與資料隔離**：詳細資訊表頭緩衝區由 120 提升到 256 bytes，降低中文物品名稱因多位元組而遭截斷的風險。`set_short_name()`、原版 `short_name`、`cmp_by_short_name()`、`Term_putstr()` 的比較清單、裝備來源篩選、選取索引以及 `append_to_file()` 匯出原理都**沒有更動**，讓顯示語言不影響項目排序／檔案內容或物品 ID。
- **離線測試**：在既有 `tests/native/test_equip_cmp_locale.c` 增加「Object comparison → 裝備比較」的雙語／未知標籤檢查；既有 `scripts/test-native-utf8.sh` 已**註冊**該 C99 檢查；本批另行實際編譯了與 GitHub 完全相同的翻譯標頭，進行獨立完整映射 smoke（50 個標籤、兩種語言、未知標籤及 NULL，共 102 項），不宣稱已在完整 repo checkout 執行該腳本。新增 `tests/test_equipment_comparison_detail_locale_source.py`，逐項檢查雙物品 `ODESC_ZH_TW`、`OINFO_NONE`、雙語視窗標題、英文 fallback、核心比較排序與匯出保持不變，並接入 `scripts/verify-zh-tw.sh`。未使用 GitHub Actions；Android APK／NDK 與真實遊玩切換測試仍需 SDK／裝置驗證。


## 第 104 批 — 裝備比較清單的中文簡稱與終端欄位寬度

- **只在互動畫面翻譯清單文字**：`ui-equip-cmp.c` 的 `display_page()` 同時供玩家互動與輸出檔案使用。實際比較畫面呼叫 `display_page(the_summary, player, true)`；`append_to_file()` 兩次呼叫 `display_page(s_for_file, player, false)`。本批**只在 `allow_reconfig && angdroid_zh_tw_enabled()`** 時產生另一份暫時性的繁中清單名稱；正常英文模式及匯出檔案仍使用原本 `e->short_name` 與 `e->nmlen`，並保留 `strcmp(left->short_name, right->short_name)` 的排序、篩選、選取索引與原有終端格網輸出。
- **維持裝備認知規則**：玩家已知的神器／特殊詞綴使用 `angdroid_zh_tw_artifact_name`／`angdroid_zh_tw_ego_name` 對照，遇到未知名稱回退英文；兩者沿用原本「顯示名稱尾部」的簡稱風格。普通裝備在畫面上使用 `object_desc(..., ODESC_COMBAT | ODESC_SINGULAR | ODESC_TERSE | ODESC_ZH_TW)`，不使用 `ODESC_SPOIL`，未知物品仍遵守原有的鑑定與外觀限制。清單的英文 `set_short_name()`、物品資料與儲存格式一概不變。
- **避免切壞 UTF-8、擠壓屬性欄位或留下殘字**：新增純 C99 `equip-compare-name-display.h`，重用 `angdroid_menu_copy_columns`、`angdroid_utf8_display_columns`、`angdroid_utf8_skip_columns`，依手機終端機**顯示欄數**（而非 UTF-8 bytes）裁切，再以空白填滿固定欄寬，避免前次較長中文留下殘字。繪製中文字串時使用 `Term_putstr(..., -1, ...)` 讓終端系統依 Unicode 字元數輸出；若格式化失敗則回退原本英文清單。原生 CJK 欄寬與文字流仍以共用的 `unicode_display.h` 為準。
- **測試**：新增 `tests/native/test_equip_cmp_name_display.c`，測試純中英字串、寬字元邊界、神器／詞綴類尾部保留、前綴裁切、補空白、損壞 UTF-8、NULL 和極小 buffer，接入 `scripts/test-native-utf8.sh`。新增 `tests/test_equip_cmp_name_display_source.py` 核對純顯示翻譯、未鑑定資訊保護、原名排序／匯出不變與畫面／檔案呼叫參數差異，接入 `scripts/verify-zh-tw.sh`。本批不使用 GitHub Actions；Android NDK／Gradle 與模擬器／實機仍需本機驗收。


## 第 105 批 — 裝備比較在極窄螢幕／初始化失敗時的安全性

- **實際發現的原生缺陷**：`initialize_summary()` 在終端高度太低或寬度不足時，可能在 `reconfigure_for_term_if_necessary()` 提前回傳，尚未執行 `filter_items()`／`sort_items()`；原先 `equip_cmp_display()` 的 `EQUIP_CMP_MENU_BAIL` 分支卻會繼續讀取 `the_summary->nfilt`，而首次配置 `mem_alloc` 時它未被初始化，存在未定義讀取與顯示錯誤「沒有符合的物品」的風險。本批在初始摘要中明確設定 `nfilt=0`，並使 `BAIL` 狀態**不讀取** `dlg_trans_msg`／`nfilt`，只顯示原有失敗提示，讓使用者按鍵返回。
- **終端格網保護**：`equip_cmp_print_label()` 在輸出中文及一般提示之前，現在檢查 `Term` 是否存在、終端寬高是否有效、`row/col` 是否在合法範圍內。避免旋轉期間 `hgt-1` 變成 `-1` 或寬度為 0 時繼續呼叫 `prt()`；有效情況仍使用既有 `angdroid_menu_copy_columns()`，完整裁切繁中 UTF-8，不改選單原本按鍵。
- **狀態與匯出不變**：新增獨立 C99 `equip-compare-state-guard.h`；沒有修改 `display_page(the_summary, player, true)`、檔案匯出 `display_page(s_for_file, player, false)`、排序、裝備篩選或任何存檔格式。正常有物品／無物品狀態與原版互動維持一致。
- **回歸測試**：新增 `tests/native/test_equip_cmp_tiny_terminal.c`（初始／失敗狀態、0×0、1×1、視窗上下邊界、無效座標、螢幕縮放）到 `scripts/test-native-utf8.sh`；新增 `tests/test_equip_cmp_tiny_terminal_source.py` 檢查實際初始化順序、失敗路徑、UTF-8 安全裁切及匯出隔離，註冊到 `scripts/verify-zh-tw.sh`。所有提交包含 `[skip ci]`，不使用 GitHub Actions。獨立原生測試不能取代 Android NDK／Gradle 與 Android 模擬器／實機在旋轉或切換視窗時的驗收。


## 第 106 批 — 裝備比較觸控選取、手機旋轉及中文操作提示

- **觸控選擇的實際風險**：`handle_input_equip_cmp_select()` 原本只按 `mouse.y` 換算目前列，沒有確認橫向 `mouse.x` 在終端範圍內，也沒有核對依 `ifirst`／`indinc` 算出的結果是否位於 `0 .. nfilt-1`。當反向排序、清單分頁、手機旋轉或接收到失效點擊時，可能把不合法篩選索引存成 `work_sel`，第二次點擊又直接以 `sorted_indices[work_sel]` 存取。新的純 C99 `equip-compare-touch-guard.h` 同時驗證終端寬高、X/Y、可見列、升降序、篩選數量與計算結果，超界不更新選取；安全索引仍以原有 `sorted_indices[]` 映射至物品。
- **空清單／失效選取**：當 `nfilt == 0` 或該頁沒有可選列時，按 `x/I` 不再進入會造成斷言的選取模式，而是顯示「沒有可查看的裝備。」。確認選取前再次檢查 `work_sel` 是否仍合法，避免旋轉或過期事件造成無效索引讀取。取消、右鍵返回、單點選取／同列二次點擊確認、`Enter`、`x` 跳過、選單操作仍使用原本狀態與命令，不改變物品 ID／存檔。
- **旋轉與反向排序**：`reconfigure_for_term_if_necessary()` 在新終端大小且正常配置時，會把原本 `work_sel` 限定在現有頁面可見範圍；原索引仍可見時維持不變，超出頁面的情況落在最接近的可見列，無效頁面則取消選擇。新增索引運算使用 `int64_t`，避免極端輸入下加減／乘法溢位。
- **持續中文化**：裝備比較選取模式新增狀態提示「點選物品列以選取，再點一次確認」，並為空結果提示提供中英文對照；仍使用現有 `angdroid_equip_cmp_text()`、`equip_cmp_print_label()` 與 UTF-8 安全欄寬裁切，不更動按鍵綁定、品質比較運算或匯出檔案。
- **離線驗證**：新增 `tests/native/test_equip_cmp_touch_guard.c` 測試正反序列、邊界、未知座標、0 件物品、失效點擊、旋轉後可見選取；加入 `scripts/test-native-utf8.sh`。新增 `tests/test_equip_cmp_touch_selection_source.py` 核對實際遊戲接線、中文對照、原有 `sorted_indices` 與匯出不變，加入 `scripts/verify-zh-tw.sh`。GitHub 提交全部 `[skip ci]`，不使用 GitHub Actions；整套 Gradle／NDK 編譯與 Android 模擬器／實機的觸控、旋轉行為仍需本機測試。


## 第 107 批 — 物品選擇器 Esc 取消中文提示、箭袋標籤安全與詳細標題

- **物品選擇器的中文取消指引**：原本 `menu_header()` 在背包、裝備、箭袋、地面與投擲物選擇器的標頭末尾固定追加 ` ESC`。現在改用 `angdroid_game_ui_text(" ESC", angdroid_zh_tw_enabled())`，中文顯示「Esc 取消」，英文仍保留原本 `ESC`；`m->switch_keys = "/|-"`、`menu_select(m, 0, true)`、`EVT_SWITCH`、取消時不寫入選取結果的既有規則、`*choice` 與回傳狀態均未改。為容納多組中文物品來源標籤，`out_val` 從 75 增至 256 bytes、`header` 從 80 增至 288 bytes；原先的 `angdroid_utf8_display_columns(header)` 與 `angdroid_menu_copy_columns` 負責實際終端畫面欄數。
- **箭袋快速標籤越界保護**：`get_tag()` 在 `quiver_tags` 模式原本直接以 `i = tag - '0'` 讀取 `player->upkeep->quiver[i]`，未檢查標籤是否為 ASCII 數字，以及 `i` 是否小於目前 `z_info->quiver_size`。新增純 C99 `item-picker-tag-guard.h`，允許 `@0` 到 `@9` 且索引在本次箭袋容量內；其他標籤則走原本 `@n` 與 `@xn` 的物品刻字掃描，不會讀取非法陣列索引、也不改快捷鍵語意／物品身份。
- **中文詳細物品名稱顯示**：`display_object_recall()`、`display_object_recall_interactive()` 與 `textui_obj_examine()` 的 `header_buf` 從 120 擴至 256 bytes，降低較長中文物品名稱被截斷的風險；原有 `ODESC_ZH_TW`、`object_info(..., OINFO_NONE)`、物品追蹤與 `textblock_free` 都未改，不暴露未鑑定物品資訊。
- **離線回歸**：新增 `tests/native/test_item_picker_tag_guard.c` 測試不同箭袋容量與非法／非數字標籤，另以 `tests/native/test_item_picker_cancel_locale.c` 測試英文／中文 Esc 標示、未知字串與 NULL 回退；兩者均接入 `scripts/test-native-utf8.sh`。新增 `tests/test_item_picker_tag_locale_source.py`，檢查來源接線、舊刻字掃描、取消狀態及詳細標題的安全範圍；接入 `scripts/verify-zh-tw.sh`。所有提交均包含 `[skip ci]`，不使用 GitHub Actions；完整 Android 編譯、真實取消流程和旋轉驗收需本機執行。


## 第 108 批 — 字形編輯 Unicode 輸入防呆、物品／符文刻字中文化

- **修正知識圖鑑字形編輯器的無效字元風險**：`ui-knowledge.c` 原先在字形選擇畫面按 `i` 輸入 Unicode 碼時，使用 `get_string("(up to 5 hex digits):", code_point, 5)`，長度可能少於畫面宣稱的 5 位，隨後直接 `strtoul(..., NULL, 16)` 並寫入 `*cur_char_ptr`。因此空字串、不完整十六進位、非字元碼都可能把目前字形換成 U+0000 或其他錯誤結果。現在改為 `char code_point[6]` 搭配 `sizeof(code_point)`，以純 C99 `knowledge-glyph-codepoint.h` 驗證 **1～5 位 ASCII 十六進位數字**，拒絕空白、前後空格、超長、不合法字元、控制碼、UTF-16 surrogate 及 Unicode noncharacters，只有成功解析才更新目前字形。取消時不更動字形，也保留原本 `Esc` 還原、`Enter` 接受、`c/p` 複製貼上與原有儲存規則。
- **繼續繁體中文化**：字形輸入提示翻譯為「Unicode 十六進位（1～5 位）：」，輸入錯誤時顯示「Unicode 字元碼無效，請輸入合法字元。」。原本分別硬編碼在物品與符文自動刻字介面的 `Inscribe with: `／「刻上文字：」改成共用 `angdroid_game_ui_text("Inscribe with: ", ...)` 中英對照。**只改畫面文字，不修改 `remove_autoinscription()`、`add_autoinscription()`、`CMD_AUTOINSCRIBE`、`rune_set_note()`、`rune_autoinscribe()`、物品 ID、刻字內容或存檔格式**。
- **離線檢查**：新增 `tests/native/test_knowledge_glyph_codepoint.c`，測試合法中英數字與 Unicode scalar、非法字元與超長、失敗時不修改輸出、中文／英文提示、未知字串與 NULL 回退；接入 `scripts/test-native-utf8.sh`。新增 `tests/test_knowledge_glyph_codepoint_locale_source.py` 驗證真實接線、輸入上限與原本取消／字形／刻字行為，接入 `scripts/verify-zh-tw.sh`。本批所有 commits 使用 `[skip ci]`，不使用 GitHub Actions；完整 Android NDK／Gradle、模擬器／實機編輯器畫面與輸入法仍需本機驗收。


## 第 109 批 — 物品操作選單繁中詳情、窄螢幕選單與數量重試安全

- **物品選單中文化**：`ui-context.c` 的 `context_menu_object()` 雖然早已使用 `angdroid_game_ui_text()` 翻譯「Inspect／Equip／Drop／Inscribe」等動作文字，但仍以原生英文 `object_desc(..., ODESC_PREFIX | ODESC_FULL)` 顯示物品詳情與檢視標題，並使用未翻譯的 `"(Enter to select, ESC) Command for %s:"` 標頭。本批在原本 `OINFO_NONE` 鑑定保護下為**畫面顯示**加入 `ODESC_ZH_TW`，重用已存在的 `"Enter 選取／Esc 返回：%s"` 字典翻譯，並透過 `angdroid_menu_copy_columns()` 依終端顯示欄位裁切完整 UTF-8 字元。英文模式維持原文，物品身分、命令 ID 與存檔不變；詳情標題緩衝區從 120 擴大到 256 bytes。
- **手機狹窄終端機**：原本的 `r.col = Term->wid - r.width - 1` 與 `r.page_rows = m->count` 未考慮中文標籤比終端更寬或列數超出高度，可能造成負值座標或選單越界。改由既有 `menu_dynamic_calc_location(m, Term->wid - 1, -1)` 處理右緣錨定／最上列、標籤 CJK 欄寬、選單可見行數與邊界；終端過小直接結束這次選單，不進入無效狀態；側邊詳情欄不足 14 格時不繪製側欄，仍保留頂部物品提示。取消、選取和畫面儲存／還原仍由原本選單機制處理。
- **避免無效快捷鍵查詢**：原先在 `switch(selected)` 之前，即使 `selected == -1`（取消）或 `MENU_VALUE_INSPECT`／`MENU_VALUE_DROP_ALL`（非 `cmd_code`）也會呼叫 `cmd_lookup_key(selected, mode)`。現在僅在一般 `CMD_*` 命令分支內查詢，仍維持 `key_confirm_command()` 與 `get_item_allow()` 的刻字確認保護，和原本的 `CMD_DROP`／`CMD_STASH`／`CMD_SELL` 堆疊操作。
- **數量輸入保留先前數值**：`quantity-input.h` 的原生嚴格 `angdroid_parse_quantity()` 對無效字串會先將輸出設為 0；原本 `textui_get_quantity()` 直接把 `amt` 當輸出，導致第一次輸入錯誤之後，下一次重試的預設值從 1 變成 0。新增 `angdroid_parse_quantity_preserving_default()`，只在解析成功時才更新 `amt`；原本 `0`、`*`、超過上限自動限制、Esc 返回、Android `TERM_CONTROL_QUANTITY` 開始／結束事件均保留。
- **回歸測試**：新增 `tests/native/test_item_context_quantity.c`，測試 8～80 欄、4～32 列、0～超大顯示名稱、短／長選單的右緣邊界，以及成功／失敗的數量重試。加入 `scripts/test-native-utf8.sh`；新增 `tests/test_item_context_quantity_locale_source.py` 檢查遊戲實際接線、中文詳情、無效命令安全和數量控制流程；更新既有 `tests/test_quantity_input_source.py`，一起由 `scripts/verify-zh-tw.sh` 執行。所有 GitHub commits 均有 `[skip ci]`，**不使用 GitHub Actions**。完整 Android NDK／Gradle 與手機操作及中文字形渲染需本機驗收。


## 第 110 批 — 穿戴／卸下／刻字的繁體中文提示與 UTF-8 確認防呆

- **物品指令提示補齊**：`cmd-obj.c` 的 `do_cmd_wield()`／`do_cmd_takeoff()` 仍有英文「Wear or wield which item?」、「Take off or unwield which item?」及空清單錯誤。這次在 `zh-tw-ui.h` 補上四組中英文對照，繪製前由 `angdroid_game_ui_text()` 根據語言開關取得翻譯；沒有更動 `cmd_get_item()` 的 `obj_can_wear`／`obj_can_takeoff` 過濾器、箭袋／地面／背包來源、裝備槽／戒指選擇、行動能量或物品資料。
- **長中文物品名稱安全**：`do_cmd_inscribe()` 與 `do_cmd_wield()` 的 `o_name` 由 80 擴充至 256 bytes；選定物品、已裝備物品的提示及裝備替換時 `!t` 卸下確認使用 `ODESC_ZH_TW` 顯示玩家可辨識的名稱，原本的 `ODESC_PREFIX`／`ODESC_FULL`、`ODESC_BASE`、知識條件都保留。標題「Inscribing %s.」與確認「Really take off %s?」透過字典顯示繁中；按下否／Esc 後仍由原版 `get_check()` 取消，不會呼叫 `inven_wield()`。刻字資料 `quark_add()`、`!t` 的次數、存檔結構均不變。
- **實際發現的共用確認問題**：原本 `ui-input.c` 的 `textui_get_check()` 使用 `strnfmt(buf, 78, "%.70s[y/n] ", prompt)`，`get_char()` 使用 `"%.70s[%s] "`。`%.70s` 按 **UTF-8 位元組** 裁切，遇到長中文物品名可能切進三位元組字元中央，也沒有依手機實際終端欄數限制。新增純 C99 `confirmation-prompt-display.h`，重用終端的 Unicode 欄寬與 `angdroid_menu_copy_columns()`，**先保留 `[y/n]`／`[選項]` 的字元及位置，再在完整 UTF-8 邊界裁切提示**，並將視窗顯示用 buffer 擴為 256 bytes。原有 `inkey_m()`、`inkey()`、只有 `y/Y` 才表示同意、滑鼠按下不會誤同意、預設否及 Esc 取消規則均不變；英文輸出保留原文。
- **測試與交付邊界**：新增 `tests/native/test_gear_confirmation_locale.c`，以純 C99 測試 6～96 欄、極窄螢幕、損壞 UTF-8、長中英文名稱、緩衝區 1～96 bytes、回應選項保留及 NULL。既有 `tests/native/test_game_ui_locale.c` 增加六組翻譯的中英文對照測試；新增 `tests/test_gear_confirmation_locale_source.py` 驗證原始遊戲確認流程及其 `!t`／刻字資料仍相同，並加入 `scripts/test-native-utf8.sh`／`scripts/verify-zh-tw.sh`。不使用 GitHub Actions（commit 皆有 `[skip ci]`），完整 Android／NDK／Gradle 與手機上字形的端到端驗收仍需本機執行。


## 第 111 批 — 卷軸／食物／藥水／通用使用及補充燃料提示繁中化

- **補齊原本仍是英文的 8 個物品選擇提示**：`cmd-obj.c` 的 `do_cmd_read_scroll()`、`do_cmd_eat_food()`、`do_cmd_quaff_potion()`、`do_cmd_use()`，各有一組「要選哪件物品」及「沒有可選物品」的英文訊息。現在接上既有 `angdroid_game_ui_text(..., angdroid_zh_tw_enabled())`；繁中分別顯示「要閱讀哪張卷軸？／你沒有可以閱讀的卷軸」、「要吃哪種食物？／你沒有可以吃的食物」、「要喝哪瓶藥水？／你沒有可以喝的藥水」、「要使用哪件物品？／你沒有可使用的物品」。英文模式仍用原句，不修改遊戲指令、真正的物品資料、按鍵或可選清單。
- **補充燃料英文文法修正**：`do_cmd_refill()` 的舊字串 `Refuel with with fuel source?` 重複了 `with`。改成 `Refuel with which fuel source?`，字典中對應的新鍵繁中翻為「要使用哪種燃料補充光源？」；其 `obj_can_refill`、`USE_INVEN | USE_FLOOR | USE_QUIVER`、先檢查是否有可補充光源的規則、`refill_lamp(light, obj)`、半回合時間完全不變。
- **使用後的剩餘物品名稱中文化**：`use_aux()` 原本已針對「你看見 %s」、「你持有 %s」翻譯整句，但 `object_desc(...)` 並未設定 `ODESC_ZH_TW`，因此中文句中物品名仍可能是英文。本批只在該段**玩家可見訊息**加上 `ODESC_ZH_TW`，並將 `char name[80]` 擴至 256 bytes，降低中文 UTF-8 名稱截斷風險。保留 `ODESC_PREFIX | ODESC_FULL | ODESC_ALTNUM`、原本的剩餘數量及 `used && use == USE_SINGLE` 計算、鑑定門檻、`gear_to_label()`、充能訊息與物品刪除／回合耗時；沒有使用 `ODESC_SPOIL`，不改存檔或物品身份。
- **離線驗證與已知限制**：新增 `tests/native/test_item_use_locale.c`，明確驗證新 9 組字串的繁中與英文模式、無映射時回退和 NULL 安全；新增 `tests/test_item_use_prompts_locale_source.py` 檢查原生選擇器的 `tval_is_scroll`／`tval_is_edible`／`tval_is_potion`／`obj_is_useable` 過濾、可選來源、消耗與分派、剩餘數量／知識保護、補充燃料的指令不變；分別加入 `scripts/test-native-utf8.sh` 和 `scripts/verify-zh-tw.sh`。GitHub commits 均標註 `[skip ci]`，**不使用 GitHub Actions**。新增測試與 Android NDK／Gradle、模擬器／實機仍須在完整原始碼環境執行。


## 第 112 批 — 魔法道具使用後的原始效果訊息中文化，保留取消與充能回復流程

- **找到未中文化的來源**：`cmd-obj.c` 的 `use_aux()` 在成功操作道具時，對 `obj->kind->effect_msg`、`obj->kind->vis_msg` 直接呼叫 `msgt(snd, "%s", ...)`，未經任何中文對照。因此卷軸、食物、藥水、魔法道具的動態效果字串仍有英文。審查原版 `variant_lib/angband/gamedata/object.txt`，其中 **20 筆 `msg:`／`vis-msg:`，共 16 種不同訊息**，涵蓋味道、身體反應、陷阱生成聲音、生命力恢復與藍光特效。本批在新純 C99 `zh-tw-item-use-messages.h` 對這 16 種**實際存在的字串**建立完整的繁體中文對照。
- **保持動態訊息的先後與感知限制**：只有原本 `obj->kind->effect_msg` 及非失明時的 `obj->kind->vis_msg` 文字在送入 `msgt()` 前經過 `angdroid_zh_tw_item_use_message(..., angdroid_zh_tw_enabled())`；原本的 `obj->activation`／`activation_message()` 分支優先權、`!player->timed[TMD_BLIND]`、`msgt` 的聲音種類 `snd`、`sound(snd)` 備援、原生遊戲資料 `object.txt` 均保持不變。英文模式、沒有映射的未知／未來字串、NULL 都安全回傳原文或 NULL，**沒有修改遊戲資料、物品記錄、效果 ID 或存檔格式**。
- **審查取消和充能的真正流程**：`use_aux()` 的瞄準取消 `cmd_get_target(..., &dir) != CMD_OK` 本來就會在 `check_devices()`、`effect_do()` 和充能扣除之前返回；這次保留。地面物品在 `effect_do()` 前暫扣的單次使用數量／魔杖或法杖充能／魔棒冷卻，在 `!used` 時也有原本的回復分支，沒有改成「取消仍扣一次」。原本成功使用、失敗嘗試、首次辨識取得知識、消耗道具、回合花費的判定均保持一致。這項程式檢查不是實機端到端驗收。
- **離線驗證**：新增 `tests/native/test_item_use_effect_messages.c`，測試 16 個中英訊息、未知字串和 NULL 回退，加入 `scripts/test-native-utf8.sh`；新增 `tests/test_item_use_effect_messages_source.py`，會直接從 `object.txt` 抽取原始 `msg:`／`vis-msg:`，驗證 20 筆／16 種全部覆蓋、無重複，以及原本瞄準取消、地面充能復原、失明限制與 `effect_do()` 分支未變，加入 `scripts/verify-zh-tw.sh`。
- **Host 已實際執行**：把 GitHub 新增的 C99 helper 及 native test 逐位元組重建到獨立目錄並以 `git hash-object` 核對 Blob SHA（`bb1451ba5c2a7e6bb42207166d1e6151a33bf4a2`、`19868619161fed3c6a740d16e5a403a80be3f3a2`）。以 GCC、Clang 的嚴格 C99 `-Wall -Wextra -Werror -pedantic` 搭配 AddressSanitizer／UndefinedBehaviorSanitizer 執行，**各 69 個斷言全部通過**。尚未在完整專案環境執行新 Python 測試、Android NDK／Gradle 或模擬器／實機；不能將獨立測試視為完整 APK 驗收。GitHub commits 均含 `[skip ci]`，不使用 GitHub Actions。


## 第 113 批 — 神器啟動的動態訊息、物品名稱與英文動詞標記繁中化

- **找出原本的訊息產生鏈**：`cmd-obj.c::activation_message()` 原本從 `obj->activation->message` 取出訊息；若神器有 `obj->artifact->alt_msg` 則先採用神器專屬版本，再交由 `obj-util.c::print_custom_message()` 展開 `{name}`／`{kind}`／`{s}`／`{is}`。原先這些模板全是英文，因此即使其他道具效果已中文化，神器啟動仍會顯示英文文字。
- **只翻譯正式資料中的模板**：從 `variant_lib/angband/gamedata/activation.txt` 找到 **48 筆 msg**，從 `artifact.txt` 找到 **5 筆神器專屬 msg**，去重共 **50 種動態訊息**。新建 `zh-tw-activation-messages.h`，所有 50 種都按原始英文逐字匹配並提供繁體中文；未收錄的新訊息、英文模式及 NULL 都回退至原文／NULL。沒有修改 `activation.txt`、`artifact.txt`、神器 ID、效果、冷卻或遊戲存檔。
- **名稱與文法安全**：中文模板保留原生 `{name}` 展開，並一律把該標記放在句首，避免舊版 `print_custom_message()` 名稱填充覆蓋前綴文字；中文模板不含英文 `{kind}`／`{s}`／`{is}` 單複數標記。原有 `print_custom_message()` 的英文占位詞處理保留；只在繁中顯示模式為物品描述額外加入 `ODESC_ZH_TW`，讓可被玩家辨識的神器名稱用繁體中文呈現，不使用 `ODESC_SPOIL` 或修改任何鑑定結果。
- **操作／費用維持不變**：`do_cmd_activate()` 的冷卻檢查與正常形態檢查、`use_aux()` 的瞄準取消早退、`check_devices()`、`effect_do()`、原本的地面物品充能暫扣及回復分支、回合耗時和 `activation_message(obj, player)` 先後順序均未改。此批只是語言輸出路徑；尚未實際在 Android 遊戲中完成操作驗收。
- **回歸測試**：新增 `tests/native/test_activation_message_locale.c`（50 個模板的中英文、無效標記、未知字串、NULL 安全）與 `tests/test_activation_messages_locale_source.py`（資料驅動檢查 48+5=53 筆原始訊息／50 種模板、唯一性、中文文法占位詞、原生展開器接線和取消／充能不變）。分別接入 `scripts/test-native-utf8.sh`、`scripts/verify-zh-tw.sh`。全部 GitHub 提交附 `[skip ci]`，不使用 GitHub Actions。完整 Android Gradle／NDK、模擬器／實機測試及原生整合驗收仍須在本機執行。


## 第 114 批 — 法術效果數值摘要與傷害屬性繁體中文化

- **法術摘要的真實缺口**：`player-spell.c::spell_effect_append_value_info()` 顯示用的 `effect_info()` 鍵值（`hurt`、`heal`、`dam`、`dur`、`power`、`range`、`blows`）及 `random`、`rad`、`len` 仍為英文。新增純 C99 的 `zh-tw-spell-effect-info.h`，只在組合玩家看得到的摘要時翻譯。整個效果資料、法術 ID、骰子與 RNG、傷害分類使用的原始英文 `effect_info()` 均不修改。
- **數值與英文模式相容**：半徑／長度的 `printf` 模板保留 `%d`，英文模式原樣顯示；未知、空及 NULL 值安全回傳，原本重複效果折疊與數值計算完全不變。
- **法術瀏覽**：平均傷害的傷害屬性使用既有 55 種投射屬性中文詞彙表；將 UI 的提示暫存緩衝區加大，並於繪圖前以 `angdroid_utf8_clip_columns()` 按顯示欄寬截取摘要，避免中文字被切成半個 UTF-8 字元。`spell->text` 的原始長篇英文敘述仍未改動，屬下一階段工作。
- **回歸測試**：新增 `tests/native/test_spell_effect_info_locale.c`，GCC、Clang 嚴格 C99 獨立測試各 31 項通過；新增 `tests/test_spell_effect_info_locale_source.py`，以原始 `list-effects.h` 驗證全部七種分類鍵值、格式 `%d` 不變、顯示層接線、投射屬性與安全裁切，接入 `scripts/test-native-utf8.sh` 及 `scripts/verify-zh-tw.sh`。完整 Python 套件、Android NDK/Gradle 及模擬器／實機仍待本機驗證。此批使用 `[skip ci]`，不運行 GitHub Actions。

## 第 115 批 — 原版法師（Mage）30 種法術詳細說明繁體中文化

- **真實來源與覆蓋範圍**：`variant_lib/angband/gamedata/class.txt` 共有 163 筆職業法術，法師有 30 筆、30 種不重複名稱；其中 7 種法術也在 Rogue 出現且原英文描述完全相同，因此會自然共用已翻譯的內容。其他職業尚未翻譯的說明安全保留英文，不宣稱全部 163 筆完成。
- **資料不可變**：`ui-spell.c::spell_menu_browser()` 原本直接 `text_out("\\n%s", spell->text)`，現在只在文字顯示前執行 `angdroid_zh_tw_spell_description(spell->name, spell->text, angdroid_zh_tw_enabled())`。翻譯表逐筆核對法術英文名稱與 `class.txt` 所有 `desc:` 行串接後的精確英文內容，僅兩者都符合才回傳繁中。英文模式、更新後描述、未知法術與 NULL 都保留原樣。
- **遊戲效果保持不變**：不修改 `class.txt`、`spell->text`、法術索引、MP、失敗率、骰子、隨機數、瞄準取消、施法回合或存檔資料；保留法術瀏覽原有平均傷害資訊、非施法閱讀操作與文字輸出方式。
- **輸出與小螢幕注意事項**：仍使用 `text_out_to_screen` 的原生換行路徑，未擅自修改終端機的寬字元換行機制。實際手機窄螢幕的換行、捲動與中文顯示仍需 emulator/實機驗證。
- **驗證**：新增 `tests/native/test_spell_descriptions_locale.c` 對 30 組中英文字串、原文回退進行純 C99 測試（預期共 66 assertions），新增 `tests/test_spell_descriptions_locale_source.py` 直接解析官方 `class.txt`，比對 30 條精確映射、7 條 Rogue 共用描述，驗證顯示層接線、數值資訊與英文字串回退，分別納入 `scripts/test-native-utf8.sh` 與 `scripts/verify-zh-tw.sh`。以上測試已註冊但完整執行、Android NDK/Gradle 及實機測試仍待本機完成；本批不使用 GitHub Actions（`[skip ci]`）。


## 第 116 批 — 德魯伊 27 種法術詳細說明繁體中文化

- 對照 `variant_lib/angband/gamedata/class.txt` 的全部 27 種 Druid 法術，為 `zh-tw-spell-descriptions.h` 補上完整原始英文＋繁體中文對照，既有 Mage 30 種維持原樣。累計 57 種原文對照；不修改遊戲資料、法術消耗、角色狀態、隨機判定或存檔。
- Druid 和 Ranger 共用 7 種法術名稱：其中 6 種原始文字完全相同，可以共用翻譯。獨立保護 Herbal Curing：Ranger 的描述不包含額外的營養恢復，不能套用 Druid 版本的文字；未知描述仍回退英文。
- 數值與危險警告逐項保留：狐狸變身不能使用物品、未指定目標的閃電會擊中玩家、地震在城鎮無效且不會摧毀神器、各種半徑/回合/傷害數字與流星爆炸範圍均保持原文資訊。
- 新增 `tests/native/test_druid_spell_descriptions_locale.c`（27×2+8=62 個預期斷言）與 `tests/test_druid_spell_descriptions_locale_source.py`，同步修正既有 Mage Python 驗證，只針對 Mage 30 組進行核對，讓表格安全擴增。註冊到 `scripts/test-native-utf8.sh`、`scripts/verify-zh-tw.sh`。
- 原始文字的精確匹配、英文回退機制、遊戲效果與 `ui-spell.c` 顯示接線不改；完整 Native/Python、Android Gradle/NDK、模擬器與實機仍需本機驗證，且未執行 GitHub Actions（`[skip ci]`）。


## 第 117 批 — 牧師 28 種法術詳細說明繁體中文化

- 依照 `variant_lib/angband/gamedata/class.txt` 的 28 筆 Priest 法術資料，為原有 `zh-tw-spell-descriptions.h` 新增 28 組名稱＋英文完整說明＋繁體中文的精確映射。翻譯表累計 Mage 30 + Druid 27 + Priest 28 = **85 種**。僅使用原有 `ui-spell.c` 顯示層接線；沒有變更遊戲資料與施法結果。
- 牧師與 Paladin **15 個同名法術**中，14 個原始說明逐字完全相同，會自然共用繁中翻譯；**Heroism** 明顯不同：Priest 需角色等級 20、Paladin 只需 15。因精確比對原英文描述，Paladin 此條會維持正確的原文，禁止套用錯誤的牧師版本。
- 保留重要限制與數值：召回延遲 14+1d20 回合、可再次施法取消、限制樓梯／召回選項與魔苟斯存亡的例外；大毀滅半徑 15 格、失明 10+1d10 回合、抗光/抗盲與城鎮無效；守護符文的破壞規則；恢復 2000 HP 及其他治療／光明法術的數值。
- `tests/test_druid_spell_descriptions_locale_source.py` 已更新為支持 85 筆新表，持續檢查原先 27 筆 Druid、30 筆 Mage 與 Ranger 的 Herbal Curing 差異；原有 Mage 回歸測試無需更動。
- 新增 `tests/native/test_priest_spell_descriptions_locale.c`（28×2+8 = **64 項預期斷言**）與 `tests/test_priest_spell_descriptions_locale_source.py`，並註冊到 `scripts/test-native-utf8.sh`、`scripts/verify-zh-tw.sh`。
- 完整 C99/Python、Android Gradle/NDK、模擬器、實機測試仍須本機執行，並以 `[skip ci]` 避免使用 GitHub Actions。


## 第 118 批 — 死靈法師 26 種法術詳細說明繁體中文化

- 直接讀取 `variant_lib/angband/gamedata/class.txt`：Necromancer 共 **26 種法術**，與 Priest 共用 `Sense Invisible` 的相同完整原文，因此只需新增 **25 組**對照資料，翻譯表由 85 增至 **110 個唯一法術名稱**，完整涵蓋 Mage 30、Druid 27、Priest 28、Necromancer 26（計 111 筆職業法術紀錄）。雙條件完全匹配法術原名和官方完整英文說明；未匹配情況依然回退英文。
- 針對危險副作用保留具體資訊：`Crush` 只瞬殺生命值低於角色等級四倍的怪物且會傷害玩家；`Grond's Blow` 半徑 15 格、失明 10+1d10 回合並以**抗失明或抗黑暗**減免，城鎮無效；`Power Sacrifice` 以 50 HP 換 50 MP；`Vampire Form` 犧牲**目前生命值的一半**並有暫時撕咬吸血效果；`Curse` 付出 100 HP；`Bat Form` 無法使用物品；`Sleep Evil` 需留意**抵抗混亂**的怪物不受影響；`Command` 保留 d/m/r 原始指令字母。
- 沒有修改 `class.txt` 或任何角色/怪物/魔法效果邏輯，所有字串只在既有 `ui-spell.c` 顯示層套用。英語模式及原文異動仍保留英文。
- 新增 `tests/native/test_necromancer_spell_descriptions_locale.c`（26×2+6 = **58 項預期 C99 斷言**）、`tests/test_necromancer_spell_descriptions_locale_source.py`（核對 110 組原文、共用法術無重複、21 組副作用關鍵字、完整 C99 對照）。修正既有 Druid/Priest 資料測試的固定表格數量，仍保持完整覆蓋，並註冊兩個新測試至既有離線腳本。
- 本批不使用 GitHub Actions，commit 包含 `[skip ci]`；只有 GitHub 原始碼靜態檢查可在連線端驗證，完整 Python/C99、Android NDK/Gradle、emulator/實機的窄螢幕與遊戲行為測試仍待本機執行。


## 第 119 批 — 聖騎士 Paladin（16 筆）與盜賊 Rogue（10 筆）法術說明

- 對照原始 `variant_lib/angband/gamedata/class.txt` 後，Paladin 16 筆中 14 筆已與 Priest 有完全相同英文描述；Paladin 的 `Heroism` 不同，需 **角色等級 15** 才取得英雄氣概，而 Priest 是等級 **20**；另一個 `Single Combat` 為專屬法術。Rogue 10 筆中 7 筆與 Mage 相同，其餘 `Object Detection`、`Detect Stairs`、`Hit and Run` 需新增翻譯。
- 本批新增 **5 筆名稱＋完整原文的顯示文字對照**，累計 **115 筆精確原文對照**，對應 **114 個不同法術名稱**；`Heroism` 會依不同完整英文描述顯示不同職業適用的內容。Mage、Druid、Priest、Necromancer、Paladin、Rogue 六個職業的全部法術詳細說明均有匹配。
- 新增 `tests/native/test_paladin_rogue_spell_descriptions_locale.c`，逐筆核對兩個職業 **26 筆**中英文切換與 8 個差異/NULL/未知/英文回退條件，共 **60 項預期 native 斷言**。
- 新增 `tests/test_paladin_rogue_spell_descriptions_locale_source.py`，直接解析原始資料，檢查 Paladin 15 個同名之中 14 個完全相同＋一個 `Heroism` 不同，及 Rogue 7 個共用＋3 個獨有，並檢驗新的五組說明與原文逐字對應。
- 調整 Druid、Priest、Necromancer 既有 Python 檢查，避免因翻譯表成長至 115 筆與 `Heroism` 出現兩個不同原文時誤報。沒有修改遊戲規則、`class.txt`、法術效果、存檔或施法指令。
- 已註冊兩個測試至 `scripts/test-native-utf8.sh` 與 `scripts/verify-zh-tw.sh`；本批禁止 GitHub Actions、Commit 採 `[skip ci]`。原始碼靜態核對不代表完整 Python/C99、Android Gradle/NDK、emulator 與實機驗證已執行。


## 第 120 批 — 遊俠 Ranger 的 11 種法術詳細說明完整繁體中文化

- 對照 `variant_lib/angband/gamedata/class.txt`：Ranger 共 11 種法術，其中 6 種與 Druid 完整說明相同而可共用；補上另 5 組 `Herbal Curing`、`Cover Tracks`、`Create Arrows`、`Decoy`、`Brand Ammunition`。合計 120 組精確原文對照、118 個不同名稱；前七種職業合計 148 筆紀錄完整涵蓋，Blackguard 15 種待下一批。
- 重要差異：Druid `Herbal Curing` 除了治癒狀態還會補充營養，Ranger 不會。舊 Druid native/source 測試此前要求 Ranger 的版本回退英文；這批兩者都有正確獨立繁體中文版本，已同步修改原本的回退斷言，並新增互不混用的比較。
- `Brand Ammunition` 保留火焰／寒冷／毒素隨機附加與命中／傷害強化，以及附魔、剋制、損壞、詛咒四種不生效情況。箭矢品質依法杖等級和角色等級決定；掩蓋蹤跡可避免留下氣味追蹤。
- 新增 Ranger 11 組原文資料 Native C99 測試（預期 30 項斷言），新增來源 Python 測試，並登錄既有 scripts 驗證流程。前面 Druid/Priest/Necromancer/Paladin+Rogue 測試更新 120 組配對／118 名稱與 Rangers 全覆蓋；另修正 Priest 測試過時的「Paladin Heroism 尚未翻譯」假設。
- 僅改 `zh-tw-spell-descriptions.h` 與相關測試／文件，保留原文完整比對、防止不同職業同名效果誤譯；不修改原始法術資料、角色、戰鬥、存檔。Commit `[skip ci]`，不使用 GitHub Actions。C99/Python、Gradle/NDK、模擬器／實機仍需本機驗證。


## 第 121 批 — 黑衛士（Blackguard）15 種法術說明繁體中文化，全八職業完成

- 直接核對 `variant_lib/angband/gamedata/class.txt` 的 **163 筆職業法術紀錄（8 種職業）**：新增 Blackguard 全部 15 筆專屬法術長篇說明，翻譯表現在共有 **135 組不重複的（法術名稱、完整原始英文說明）配對**，涵蓋原版 **133 個不重複法術名稱**與 **163/163 筆職業紀錄**。`Heroism` 與 `Herbal Curing` 各保留 2 種同名但原文不同的安全翻譯。
- 重要副作用與數值：`Berserk Strength` 近戰 +75、防禦 AC -10、抗恐懼但禁止隨時間治療；`Whirlwind Attack` 等級 5/20/35/50 分別 1/2/3/4 擊；`Leap into Battle` 最多 4 格、25/40 級各增加攻擊、每移動一格攻擊次數 -25% 然後取整；`Shatter Stone` 20+1d30 傷害；`Grim Purpose` 12+d12 回合；`Maim Foe` 成功傷害造成 6 回合暈眩；`Venom` 18+d18 回合；`Bloodlust` 會帶來危險副作用；`Unholy Reprieve` 消耗 66 HP 換 STR/INT/CON 和經驗恢復；`Quake` 半徑 13 格。
- 新增 `tests/native/test_blackguard_spell_descriptions_locale.c`（15 筆×2 種語言顯示＋8 個安全／fallback 檢查，共 **38 項預期 native 斷言**）、`tests/test_blackguard_spell_descriptions_locale_source.py`（解析正式 class.txt，對比全部 163 筆紀錄與 135 組原文配對、25 組關鍵字，並驗證跨職業同名效果）。同步登錄離線 runner。
- Mage、Druid、Priest、Necromancer、Paladin+Rogue、Ranger 六組原本的 Python 檢查均將完整映射表更新至 135 配對、133 個名稱及 Blackguard 資料；嚴格原文相符與英文回退機制保持不變。
- 只修改 `app/src/main/cpp/angband/zh-tw-spell-descriptions.h` 的顯示文字和測試／文件，不修改 `class.txt`、遊戲法術效果、職業數據、存檔、施法流程。使用 `[skip ci]` 不觸發 GitHub Actions；本機 C99/Python、Android Gradle／NDK、模擬器及實機的真實執行／窄螢幕換行仍需驗證。


## 第 122 批 — 長篇法術說明繁體中文螢幕換行修正

- 找到實際問題：`ui-output.c::text_out_to_screen()` 每讀一個 `wchar_t` 都將本地游標 `x` 加 1，但 `ui-term.c::Term_addch()` 對中、日、韓等雙寬字會消耗兩個實際欄位。修正使用 `angdroid_is_wide_text()` 計算字元寬度及 `angdroid_screen_text_needs_wrap()` 決定是否換行，預留終端最後一欄。
- 保留原本英文遇到空格回流整個單字的效果；若準備搬移的文字包含 CJK 雙欄字或後續佔位格，則直接換到下一列，不會把半個中文字當成空白搬移。
- `text_mbstowcs()` 限制為 1023 個字元並強制陣列最後位置 NUL，避免轉換長文字時缺少字串結束符號；輸出超出終端高度時不繼續發出非法列操作。法術說明由第 0 欄起始時，也不再設定非法的 `text_out_indent=-1`。
- 新增原生 C99 `tests/native/test_screen_text_wrap.c`，測試 9／10／12／16 欄的 CJK、英文、混合數字與換行計算以及保留行尾欄位。新增 `tests/test_spell_screen_wrap_source.py` 檢查實際原始碼是否正確接線，並登錄至 `scripts/test-native-utf8.sh` 和 `scripts/verify-zh-tw.sh`。
- 限制：當說明超出可見視窗高度時，目前只安全停止輸出，並未實作完整的法術說明捲動檢視。C99／Python、Android Gradle／NDK、模擬器／實機仍須在完整 checkout 執行驗收。
- 未動到 `class.txt`、163 種職業法術紀錄、翻譯表、法術施放邏輯、角色資料或存檔。`[skip ci]`，不使用 GitHub Actions。


## 第 123 批 — 法術說明全螢幕分頁閱讀（Android／原版 Angband）

- 既有法術選單的 `?` 仍切換選單下方簡介。新增獨立命令鍵 `]` 開啟完整文字閱讀器，**不覆蓋原本字母法術標籤、方向鍵、Space 換頁、Enter 選取／施法**；關閉閱讀器後會回到原選單。
- 使用完整原始 `spell->text` 與現有的 `angdroid_zh_tw_spell_description()` 嚴格雙重匹配，獨立閱讀器不更動法術資料／遊戲規則，英文及繁中皆能閱讀。
- 新增純 C99、無 JNI 依賴的 `zh-tw-spell-reader-pages.h`：透過既有 `unicode_display.h` 依 Unicode 顯示欄寬安全拆分文字，避免切斷 UTF-8 位元組／中文字；遇英文空格優先斷字、保留原文換行，依螢幕實際可用高度做上下移動與分頁。轉向改變尺寸時重新計算總行數並夾住捲動位置。
- 全螢幕閱讀時可用上下鍵或 `j/k` 逐行、PgUp/PgDn（或 `[/]`）翻頁、Home/End 跳頭尾、Esc／q 返回。手機觸控點標題返回；點頁面上半向前一頁／下半往後一頁。
- 另新增 `tests/native/test_spell_reader_pages.c` 與 `tests/test_spell_reader_source.py`，涵蓋中文、英中混排、連續空格、原文換行、窄欄、UTF-8 完整性、頁面上下限、選單按鍵不被偷走；整合既有 Native / Python 離線驗證入口。
- 完整 C99/Python、Gradle/NDK、模擬器與實機驗收仍需要在本機執行，GitHub 原始碼靜態核對不可當作實際 UI 驗收；無 GitHub Actions，本批提交含 `[skip ci]`。


## 第 124 批 — 法術說明閱讀器：旋轉定位、觸控誤翻頁與窄螢幕可讀性

- 延續第 123 批的獨立 `]` 全螢幕法術說明閱讀器，修正 **螢幕旋轉時閱讀位置跳動**：原本只保留第幾個畫面行（row index），直／橫螢幕寬度不同時會把原文重新斷行，於是相同行號指向不同文字。現在每次渲染記錄螢幕頂端所顯示行的**原始 UTF-8 位元組起點**；收到 `EVT_RESIZE` 時，根據新寬度的文字分行定位到包含該位元組的行，再限制到合法頁面範圍，不會將 UTF-8 字元切成一半。
- 新增可直接由 C99 測試的 `angdroid_spell_reader_line_for_offset()`，並處理空字串、超過文末的偏移量及極窄終端尺寸。
- 原本任何 `EVT_MOUSE` 都可能以相對高度決定翻頁，甚至觸及終端範圍外。本批新增 `angdroid_spell_reader_tap_action()` 統一檢查 `x/y` 有效範圍和可用尺寸，讓標題列與底部提示列都可以觸控退出，本文上／下半部仍是向上／向下翻頁；點擊範圍外則完全忽略。
- 閱讀器標題與底部操作提示統一使用 `angdroid_utf8_clip_columns()` 裁至目前的實際終端顯示欄寬，避免長文字覆蓋本文；保留 Esc、q、方向鍵、PgUp/PgDn、Home/End 以及選單專用的 `]` 快捷鍵。若收到 `EVT_ESCAPE` 事件可直接安全退出。退出後仍會 `screen_load()` 恢復選單，不修改施法流程或任何存檔資料。
- 在 `tests/native/test_spell_reader_pages.c` 追加 **21 項 C99 預期斷言**，測試寬度不同時相同字元位元組位置的行號轉換、超過文末偏移、有效／無效觸控、標題／底部退出及極窄尺寸；擴充 `tests/test_spell_reader_source.py` 確認實際 UI 事件有使用相同的純函式、提示字串有依 Unicode 顯示欄裁切，以及舊選單的施法／選擇鍵依然未動。
- 原有 Native/Python 驗證腳本會自動執行這兩個既有測試，不新增 GitHub Actions。沒有修改 `class.txt`、163 筆法術原始紀錄、135 組翻譯對照、法術耗魔／成功率／傷害、角色資料或存檔。
- GitHub 源碼靜態核對不代表完整 Android／NDK／模擬器或實機驗收；請在本機執行 `sh scripts/verify-zh-tw.sh --android zh-tw-validation-124.log`，實際確認開啟長文、捲動一半後旋轉、點上／下半部與標題／底部返回，以及返回後正常選擇和施放法術。


## 第 125 批 — 法術全文閱讀器反覆開關與旋轉後的選單狀態

- 審核發現**真正的事件遞送問題**：在 `ui-spell.c::spell_menu_show_full_description()` 裡收到的 `EVT_RESIZE` 被閱讀器自行處理，外層 `ui-menu.c::menu_select()` 沒機會收到，因此法術選單在返回後可能沿用旋轉前的 `menu->active` 區域／欄寬，造成選單跑位、裁切與下一次觸控座標錯誤。
- 新增 `spell_menu_relayout(struct menu *m)`，在閱讀器退出後**無論有沒有旋轉都從目前 `Term_get_size()` 重新建立完整 `region` 並呼叫 `menu_layout()`**，保留原本 `m->cursor`、`selected_spell`、法術清單及施法狀態。原本的法術選單首次建立也改用同一個排版函式，避免新建與退出後採用不同計算。
- 將原本 `MIN/MAX(Term->wid - 15, 80)` 的寬度計算抽出為純 C99 `angdroid_spell_reader_menu_width()`：終端寬度不足 16 欄時回傳 0，其他情況保留 15 欄旁邊空間、最大不超過 80 欄。回到選單時不會重用橫螢幕舊寬度。
- 閱讀器原本 Esc／q 提前呼叫 `screen_load()` 並 `return`，其他出口在結尾恢復畫面。改為唯一 `screen_save()`／`screen_load()` 配對：任何正常結束（鍵盤、點標題、轉向遇過小終端、觸控底部）都走同一個還原出口，避免未來修改遺漏恢復。
- `tests/native/test_spell_reader_pages.c` 新增 15 項 C99 預期斷言，驗證 0／15／16／20／40／94／95／96／500 欄的合法選單寬度，以及直橫直反覆尺寸變化時重新計算的結果。`tests/test_spell_reader_source.py` 新增來源測試，確認 `menu_layout()` 在閱讀器返回後一定呼叫、選單首次建立與返回使用同一函式、原始選單選擇結果不被修改，以及閱讀器的畫面儲存／還原各一處、無存檔後的提早 return。
- **尚待驗收**：需在真實 Android NDK／Gradle 編譯及 emulator／實機上反覆開啟、翻頁、旋轉、關閉、選取／施放法術，確認頁面／遊戲狀態一致。GitHub 靜態程式碼核對不表示實機行為或完整本機測試已通過；本批提交使用 `[skip ci]`，不執行 GitHub Actions。


## 第 126 批 — 手機點擊法術列無法選取的事件相容性修正

- 審核 `ui-menu.c::menu_select()` 發現：手機點擊有效的法術列時，`menu_handle_mouse()` 先更新 `menu->cursor` 並設定待輸出的 `EVT_SELECT`，但為防止觸控事件重複觸發，先將**原始 `EVT_MOUSE`** 送到 `menu_handle_action()`，並以 `touch_select_dispatched` 禁止同一次點擊再次派送 `EVT_SELECT`。
- `ui-spell.c::spell_menu_handler()` 原本**只接受 `EVT_SELECT` 與 `EVT_KBRD`**，因此有效觸控可能僅讓游標移動，卻不會將所點法術儲存為 `selected_spell`；在施法模式中尤其可能回傳未選取。其他種類的選單可能依靠原始 `EVT_MOUSE` 做不同操作，因此本批**只修法術選單，不改全域 `ui-menu.c` 的派送機制**。
- 現在法術選單可接受由 `ui-menu.c` 正常派送的 `EVT_SELECT`，或是已更新游標、觸控位於 `m->active` 且非 `button=2` 返回手勢的 `EVT_MOUSE`；使用 `angdroid_spell_menu_touch_selectable()` 核對，並確認 `oid` 在 `[0,n_spells)` 內，避免清單外／標題列觸控錯誤選取上一個法術。
- 對有效選取一律只寫入一次 `d->selected_spell = d->spells[oid]`；瀏覽模式 `d->browse` 仍回傳 true 以繼續瀏覽，選擇／施法模式則回傳 false，讓外層同一次 `EVT_SELECT` **正常返回而不重複執行 row handler**。原本鍵盤法術標籤、Enter、Esc、`?`、`]` 完整說明、旋轉後選單重新排版均保留。
- 在現有 Native C99 `tests/native/test_spell_reader_pages.c` 加入 **12 項**法術列觸控資格斷言（有效內部座標、不同法術索引、外部與標題、右鍵返回、無效選取索引）。`tests/test_spell_reader_source.py` 新增來源驗證，檢查 `ui-menu.c` 更新游標後原樣派送 `EVT_MOUSE`、防重複呼叫、`ui-spell.c` 新的單次選取邏輯及非法觸控排除。沿用現有 `scripts/test-native-utf8.sh` 和 `scripts/verify-zh-tw.sh`，沒有新增 GitHub Actions。
- 本批程式碼靜態核對與局部 C99 測試的通過不等於真實 Android 觸控或施法已驗收。仍需在 Android emulator／實機檢查：點不同法術列是否選到點擊目標、僅執行一次施法、清單外點擊不選取、選完能返回、閱讀器 `]`／旋轉操作無回歸。


## 第 127 批 — 防止手機快速連點與單次誤施法：觸控雙擊確認

- 針對第 126 批的法術觸控選取進一步核對：`ui-menu.c` 雖提供 `MN_DBL_TAP`，但法術選單的 `m->flags` 只有 `MN_CASELESS_TAGS`，因此**一次有效觸控就會結束選擇/施法選單**。直接套用 `MN_DBL_TAP` 仍存在「點到原本已反白的法術就直接確認」的例外，無法保證第一下永不施法。
- 本批只在 `ui-spell.c` 的法術選單增加 `touch_pending_oid` 與 C99 無引擎依賴的 `angdroid_spell_menu_confirm_touch()`。施法、學習等會離開選單的模式，**第一次觸控只標記該法術、第二次連續點擊同一列才真正寫入 `selected_spell`**；點另一個法術則重新開始確認。純瀏覽模式仍可一次觸控選列，鍵盤 Enter／字母選取仍是原版單次確認。
- 不依賴原始游標是否已在該列，所以即使初次打開時第一個法術已反白，也不能靠單次觸控就施放。第一次點擊走既有 `menu_select()` 的「事件已處理」分支，第二次確認則只寫入**一次** `selected_spell` 並讓外層單次 `EVT_SELECT` 結束選單，保留全域 `touch_select_dispatched` 的防重複機制。
- 若觸控無效、點選單外／標題、使用其他鍵盤命令（例如 `?` 與 `]`）、離開全文閱讀器、旋轉裝置，或用方向鍵移到別列，會**取消前一次觸控確認**。後者利用既有每次選單重畫都呼叫的 `spell_menu_browser()` 偵測游標位置差異；另把 `spell_menu_relayout()` 註冊為法術選單自己的 resize callback，在重排時清除待確認狀態。原本的畫面儲存/還原與 135 組法術說明表不變。
- 施法選擇提示會顯示「**觸控同列兩次確認**」，英文顯示 `Tap row twice`，以免玩家誤以為第一次點擊沒有反應。手機 `TermView.java::onSingleTapUp()` 的事件會在完成一次點擊時呼叫 `sendMousePress()`，不會把 ACTION_DOWN/ACTION_UP 當兩次獨立遊戲觸控。
- 既有 Native C99 `tests/native/test_spell_reader_pages.c` 新增 **29 個直接 CHECK**，覆蓋初始已反白、同列兩次確認、更換列、瀏覽單次觸控、NULL/非法列、明確取消後不能繼續前次確認。既有 Python `tests/test_spell_reader_source.py` 新增 `test_two_taps_required_for_casting_and_studying`，檢查 UI 事件與原生函式一致、真正 Android 單擊事件來源，並更新第 126 批已不適用的「一次觸控即寫入」靜態斷言。
- 本批不修改 `ui-menu.c` 的通用事件分派，避免影響商店、背包或其他選單；不修改法術效果、遊戲規則、原文翻譯與存檔。完整 Android NDK/Gradle、模擬器/實機操作（尤其第二次點擊同一列才施法）仍待本機驗證。不使用 GitHub Actions，commit `[skip ci]`。

- 第 127 批同時把選擇提示緩衝區從 80 bytes 擴為 **192 bytes**，在 `prt()` 顯示前以 `angdroid_utf8_clip_columns()` 限制到手機終端欄寬，避免新增的雙擊說明被 UTF-8 位元組截斷或跨行覆蓋法術選單。


## 第 128 批 — Android 遊戲畫面手勢取消／多指／長按防止法術誤觸

- **真實觸控路徑審查**：`TermView.java::onTouchEvent()` 原本只有 `ACTION_DOWN/MOVE/CANCEL/UP` 的主要處理，沒有明確追蹤 `ACTION_POINTER_DOWN/UP` 或 `getPointerCount()>1`。如果先加入第二根手指、再依序抬起，剩餘手指最後的 `ACTION_UP` 仍可能走 `ScreenView.executeAction() → onSingleTapUp() → sendMousePress() → state.addMousePress()`。這種多指後的單擊不能用於法術清單第二次觸控確認。
- 新增 **無 Android 依賴**的純 Java `ScreenTapGuard`，只在起始位置屬於 `ScreenView` 時啟用。單指畫面觸控仍照常送出原生 mouse press；若多指加入／離開、收到 `ACTION_CANCEL`、長按執行或 `onSizeChanged()` 旋轉時，會把該手勢標記取消，最終 `ACTION_UP` 不再變成額外一次畫面單擊。釋放位置若已滑出 `ScreenView`、落到 D-pad/其他覆蓋元件，也不算畫面單擊。
- **保留其他浮動按鈕**：多指阻擋只適用於 `ScreenView` 起始的手勢；`ScreenTapGuard.begin(false)` 不干擾虛擬方向鍵、重複按鈕、拖曳視窗等既有行為。取消中的畫面手勢不會意外觸發右下／左下的快捷選單，因為新防護在角落觸控判定前執行。
- `TermView.java::createTimers()` 的 `LONG_PRESS` 分支先取消畫面單擊，**但仍然執行既有 `lastView.executeLongPress(null)`**；沒有拿掉原本長按功能。取消／多指時使用原有 `stopTimers()`，不發出新的原生按鍵或滑鼠事件。
- 新增 `tests/java/org/rephial/xyangband/ScreenTapGuardTest.java`，直接使用 JDK 21 編譯和執行 **18 項斷言**，確認單指正常、雙指最終抬起被拒、取消後可恢復、長按釋放不重複點擊、畫面滑向浮動按鈕不觸發，以及 D-pad 維持原本操作。完整測試檔與 Java helper 已以 `git hash-object` 對照 GitHub blob SHA，檔案內容一致。
- 兩個 Java 原始檔已註冊至 `scripts/test-terminal-unicode.sh`，其本來就是 `scripts/verify-zh-tw.sh` 的 Java 測試階段。另新增 `tests/test_screen_tap_guard_source.py`、註冊至 `scripts/verify-zh-tw.sh`，檢查 `TermView.java` 事件遮罩、`ACTION_POINTER_DOWN/UP`、取消、長按、旋轉及快捷鍵順序確實接入。
- **重要驗證界線**：目前直接執行通過的是純 Java 18 項 helper 斷言，尚未在本環境完成 Android `TermView` 的完整編譯、Gradle／NDK 或 emulator／實機手勢驗收。原生法術選單第 127 批同列二次確認仍保留；`class.txt`、原版 163 筆法術、135 組繁中對照、傷害／耗魔／機率及存檔均不變。GitHub Actions 不使用，Commit 標記 `[skip ci]`。


## 第 129 批 — 法術觸控雙次確認的 2 秒有效期與單調時鐘

- 原始第 127 批 `angdroid_spell_menu_confirm_touch()` 只檢查兩次觸控是否為**相同法術列**；若使用者點一次、放下手機很久、再回到原列點一下，仍可能直接確認施法／學習。此問題可從既有 C99 helper 和 `ui-spell.c` 明確重現。
- 新增 **2 秒確認時限** `ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS=2000`，原生法術選單以 `clock_gettime(CLOCK_MONOTONIC)` 取得**經過時間（毫秒）**，不使用可被手機時間設定、時區或 NTP 調整的日曆時鐘。Android 專案的 minSdk 為 22，可使用 `CLOCK_MONOTONIC`。在 ANSI/C99 建置時於 `ui-spell.c` 的所有標頭之前定義 `_POSIX_C_SOURCE=200809L` 以公開 POSIX 時間介面。
- 每次觸控只在 **同一 oid、已保存有效時間、單調時間嚴格遞增（兩個事件不能同一毫秒）、且經過時間 ≤2000 毫秒**時確認；若逾時（2001ms）、換列、單調時鐘倒退或讀取失敗（回傳 0），這一下都只會**重新記錄為第一次觸控，絕不直接確認**。等於 2000ms 的邊界仍允許確認；確認後會同時清除待確認索引與時間。
- `spell_menu_data` 新增 `uint64_t touch_pending_at_ms`。所有原本清除 `touch_pending_oid` 的位置（旋轉重新排版、閱讀器返回、鍵盤/Enter/問號、點選單外、改列、進入瀏覽模式與初始狀態）都改為共用 `angdroid_spell_menu_cancel_touch()` **同時清除 oid 與時間**，避免上一輪計時洩漏到下一次施法操作。瀏覽模式與鍵盤字母／Enter 仍維持原版單次選擇。
- UI 提示改為繁中「**2 秒內觸控同列兩次確認**」、英文 `Tap same row twice within 2s`，延用原本的 UTF-8 顯示欄裁切，窄螢幕不覆蓋法術列表。未更改 `ui-menu.c` 的所有選單事件派送，也不更動遊戲法術資料、施法計算、角色存檔或所有 135 筆翻譯表。
- `tests/native/test_spell_reader_pages.c` 更新第 127 批觸控確認測試：同列 2000ms 可確認、2001ms 不可確認、逾時重新準備、換列重計、時間 0／倒退／逼近 `UINT64_MAX`、瀏覽模式與安全清除。仍使用既有 `scripts/test-native-utf8.sh`。同步更新 `tests/test_spell_reader_source.py` 驗證 C99 時鐘來源、欄位、時間計算及全部清除路徑，避免過時的舊 helper 簽名斷言造成誤判。
- 靜態來源比對與隔離 C99 測試不能取代完整 Android 專案、模擬器或實機驗收；請在手機確認「點一次 → 等待超過 2 秒 → 再點相同法術」**不能施法**，必須在時限內再點第三下才確認；正常連續兩下、換列、瀏覽及鍵盤操作保持正常。不使用 GitHub Actions；commit `[skip ci]`。


## 第 130 批 — Android 背景／前景及焦點切換的殘留抬手事件保護

- 審查 `TermView.java`、`ScreenTapGuard.java` 與 `GameActivity.java` 的實際生命週期後發現：`TermView.onPause()` 原本只送出 `SaveGame` 並停止長按／方向鍵重複計時器，**沒有清除手勢追蹤**。同時 `ScreenTapGuard.finish()` 若 `screenGesture=false`，會直接接受 `ACTION_UP`，即使沒有對應的 `ACTION_DOWN`；背景返回、對話框失焦、取消後補送抬手以及重複釋放都有可能進入舊的 `lastView.executeAction()`，造成無意義的原生滑鼠事件。
- `ScreenTapGuard` 新增 `activeGesture`，由每次 `begin()` 設為有效；`finish()` **必須先見過真正的 ACTION_DOWN**，否則不接受畫面／虛擬按鈕的孤立 ACTION_UP。成功或拒絕結束時都透過 `abortGesture()` 清理全部旗標，避免一次點擊重複送出兩次；對一般 overlay／D-pad，仍只要有效 `begin(false)` + `finish(false)` 即能正常操作。
- `TermView.onPause()` 先取消手勢、停止 timer，再透過原本的 `GameThread.Request.SaveGame` 儲存遊戲。對照既有 `KeyBuffer.signalSave()`：儲存訊號會清掉尚未被 native 消費的鍵盤／滑鼠輸入，然後送出儲存 marker；本批**不另行清空 KeyBuffer**，避免破壞儲存或其他插件的輸入機制。
- `TermView.onResume()` 在恢復前要求新手勢一定要有新的 ACTION_DOWN；`onWindowFocusChanged(false)` 也取消目前手勢並停止重複操作，用於沒有觸發 Activity pause 的通知欄／對話框切換。`ACTION_CANCEL` 現在直接結束 ScreenView 和 overlay 的舊手勢，不再留下有活動狀態的 overlay 按鍵。
- 保留第 127–129 批的法術列**同列兩次、2 秒內**確認，以及原有 `?`、`]`、鍵盤選取和選單返回行為；沒有修改全域 `ui-menu.c`、原版法術計算、163 筆法術紀錄、135 組繁中說明配對或存檔格式。
- 新增 `tests/java/org/rephial/xyangband/ScreenTapGuardTest.java` **16 項回歸斷言**，現在總計 **34 項**，由 `scripts/test-terminal-unicode.sh` 執行；測試涵蓋無 DOWN 的 UP、背景／焦點失去／恢復、overlay 釋放、重複 UP、取消後新手勢。**JDK 21 `javac -Xlint:all -Werror` 實際編譯並執行全部 34 項通過**，且兩份 Java source 的 `git hash-object` 與 GitHub Blob SHA 完全一致。
- `tests/test_screen_tap_guard_source.py` 增加生命週期和 `ACTION_CANCEL` 來源契約檢查（如取消／停止先於儲存、正常虛擬控制不被阻擋、`KeyBuffer.signalSave()` 的原有 FIFO 清理順序），原本已透過 `scripts/verify-zh-tw.sh` 註冊。
- **驗證限制**：目前只實際執行純 Java helper 測試；完整 `TermView` Android 編譯、離線套件整體、NDK／Gradle、emulator、實機與 Android 在背景時本地 native 法術選單已經準備的第一下觸控狀態，仍須端到端驗收。本批不宣稱 Java 層手勢取消可清除所有 C 層已記錄的第一次點擊。沒有使用 GitHub Actions，commit `[skip ci]`。

### 第 130 批本機驗收指令

```bash
git pull --ff-only origin master
sh scripts/verify-zh-tw.sh --android zh-tw-validation-130.log
```

手動驗證：先按下遊戲畫面但尚未抬手就切到背景／通知欄、再返回並抬手，不得產生滑鼠點擊；一般點擊仍應可選法術、需要合法的同列第二下才確認；連續快速切換、畫面取消、右下角快捷選單、D-pad 長按及方向鍵重複都須確認。


## 第 131 批 — Android 暫停／恢復直接撤銷 Native 法術首次觸控

- **根因追蹤**：第 129 批的 2 秒確認時限只能避免久置後的第一次點擊；第 130 批取消 Java 側未完成的按下／抬手，但 **Native `ui-spell.c::spell_menu_data` 仍保留 `touch_pending_oid` 與 `touch_pending_at_ms`**。如果在第一次有效點擊後於 2 秒內迅速切背景又返回，再點同一法術，原生端仍可能直接完成施法。Java `GameThread.Request.SaveGame` 會由 `KeyBuffer.signalSave()` 清空排隊的輸入，但不會重置已存入 `ui-spell.c` 的結構欄位。
- **不能用額外 Esc 或特殊鍵修正**：共用 `KeyBuffer` 依遊戲狀態排序一般按鍵、儲存 marker、控制命令。把生命週期通知混成遊戲按鍵可能造成多個遊戲變體的非預期行為。改用跨執行緒、只讀查詢的 **InputLifecycleEpoch**：
  - `InputLifecycleEpoch.java` 使用 `AtomicInteger` CAS 遞增世代，預設 1、避免負數／0，累積到 `Integer.MAX_VALUE` 後回到 1；不插入遊戲指令。
  - `StateManager` 持有世代物件，`TermView.onPause()`、`onResume()` 與 `onWindowFocusChanged(false)` 在取消 Java 手勢時同步增加世代。**onPause 先通知、再停止 timer 並發出 SaveGame**；畫面焦點短暫丟失但未進入 Activity pause，也會通知。
  - `NativeWrapper.getInputLifecycleEpoch()` 回傳當下世代；`curses.c` 在 native 遊戲啟動時快取 `getInputLifecycleEpoch:()I` JNI 方法識別碼，透過 `angdroid_input_lifecycle_epoch()` 從遊戲執行緒直接讀值。JNI 不可用、發生例外或讀到無效世代時回傳 -1，**不得直接確認觸控施法**。
  - `ui-spell.c` 的 `spell_menu_data` 記住最近觀察的世代；在法術清單重畫和每次處理法術列事件時比較目前世代。若世代改變、JNI 故障或無效，就以 `angdroid_spell_menu_touch_epoch_changed()` 同時撤銷待確認法術與時間戳；如果變動恰好發生在重畫與處理點擊之間，則此次觸控**直接吃掉**，不得當作第二次確認。只有未改變、有效的世代才繼續原有的 2 秒雙擊確認。
- **範圍**：不修改 `ui-menu.c` 共用事件分派、Java `KeyBuffer`、任何法術施放與學習規則、角色存檔格式，以及已完成的 163 筆原版法術資料和 135 組繁中長說明。
- **測試**：新增獨立 JDK `InputLifecycleEpochTest.java`，包含暫停／恢復世代變更和 4 條執行緒各遞增 1,000 次，確保沒有丟失更新；在 `tests/native/test_spell_reader_pages.c` 新增生命週期變動、JNI 回傳 -1／0、重新同步與無效指標的 C99 回歸測試；`tests/test_spell_touch_lifecycle_source.py` 檢查 UI→StateManager→NativeWrapper→JNI→Native 法術選單連線及只讀、無特殊按鍵的事件保護。新增 Java 測試已接入 `scripts/test-terminal-unicode.sh`；來源檢查接入 `scripts/verify-zh-tw.sh`，C99 沿用已有的 Native 測試腳本。
- **驗收限制**：獨立主機測試／GitHub 原始碼核對不等同 Android 的 JNI、NDK／Gradle、模擬器／實機端到端驗證。尤其焦點喪失／恢復時 native 與 Java 執行緒相互交錯的事件次序仍需在手機測試。提交 `[skip ci]`，**不使用 GitHub Actions**。

### 第 131 批 Mac 本機驗證

```bash
git pull --ff-only origin master
sh scripts/verify-zh-tw.sh --android zh-tw-validation-131.log
```

驗收場景：開啟施法選單 → 點一次 A（未施法）→ **2 秒內切到背景又返回** → 再點 A，必須只當新的第一次觸控、不能立即施法；然後合法再點 A 才確認。相同場景需測試通知欄失焦、旋轉畫面、瀏覽模式、Enter／法術字母及其他虛擬按鈕。

## 第 238 批 — FAangband 物品操作與野外探索繁體中文（2026-11-13）

- `faangband/cmd-obj.c`：將既有字典實際接入 36 處背包／裝備／使用道具／學習法術的提示與錯誤訊息，並接通 12 種一般回饋及一項啟動回饋。新增對物品使用失敗、未能卸下裝備、燃料與刻字、法力不足、施法學習失敗、物品使用後敘述等動態句子的繁中分支；英文語系仍採原有字串及格式。補正原本英文的燃料來源提示拼字重複。
- `faangband/cmd-cave.c`：把 39 組既有翻譯接入 56 處可見探索訊息，包括門鎖、蜘蛛網、路線提示、障礙、怪物與地城階梯；另新增 16 組城鎮搬家、野外路徑、上下樓梯、離開特殊樓層確認及設陷阱的繁中字典。九種帶參數的怪物／陷阱事件保留原始英文格式，並有獨立繁中格式字串。
- 挖掘指令：針對原版 `with your hands`、`with your weapon`、`with your swap digger` 三種工具提示加上繁中語序，讓清除碎石、挖掘、發現物品、挖通通道與失敗時的七種敘述不必混入英文介系詞。挖掘成功率、工具切換、地形變更及物品生成邏輯沒有改動。
- 所有中文只在 `angdroid_zh_tw_enabled()` 開啟時顯示；英文模式、`cmd_get_item` 選擇條件、快捷鍵、裝備與陷阱 ID、角色歷史紀錄及存檔格式保持不變。尚未翻譯的動態效果描述仍保留英文回退。
- 新增 `tests/test_faangband_item_command_messages_locale_source.py`、`tests/test_faangband_exploration_command_locale_source.py` 和純 C99 雙語字典測試 `tests/native/test_fa_item_command_translation.c`，由 `scripts/test-fa-item-command-translation.sh` 和 `scripts/verify-zh-tw.sh` 串接。
- **驗證限制**：已在 GitHub Draft PR #146 提交並進行來源與接線核對；尚未在完整 repository checkout 執行這些新測試，也尚未確認最新 Android NDK／Gradle 編譯、模擬器或實機遊戲畫面。這些修改不能當作正式版驗收完成。

## 第 239 批 — FAangband 地城寶物／危險感覺繁體中文（2026-11-13）

- **遊戲內原有資料維持英文**：`faangband/cmd-cave.c` 中 `obj_feeling_text[11]`（寶物）與 `mon_feeling_text[10]`（怪物危險）陣列的 21 個英文來源字串和感覺代碼、顯示時機均保留；僅在 UI 輸出時使用共用 `angdroid_game_ui_text()` 對照，避免影響存檔、判斷、其他遊戲變體或英文模式。
- **新增 21 組繁體中文句子**：涵蓋強大物品、上等寶物、幾乎沒有寶物、怪物極危險與安全寧靜等地城感覺。單獨發現寶物時顯示「你感覺到：…。」；探索不足時的怪物危險訊息直接顯示「…。」；兩類已知時按照原本的 `yet/and` 條件，以中文「，但／，而且」連接兩個詞句，避免顯示英文介系詞或英式標點。城鎮提示及主題樓層的 `level->typ` 來源內容保持既有行為。
- **順序安全修正**：把寶物／危險索引範圍檢查移到 `obj_only`、探索不足分支**之前**，避免異常感覺代碼在檢查前被索引。沒有改變探索門檻 `cave->feeling_squares < z_info->feeling_need`、`disturb(player)`、角色狀態或遊戲規則。
- **離線驗證檔案**：`tests/native/test_fa_dungeon_feelings_locale.c` 以 C99 直接包含正式 `zh-tw-ui.h`，對照 21 組中英字典、未知字串回退、獨立與組合範例；`scripts/test-fa-dungeon-feelings-locale.sh` 使用 `-std=c99 -Wall -Wextra -Werror -pedantic`；`tests/test_faangband_dungeon_feelings_locale_source.py` 對照正式感覺陣列與字典／C99 資料，檢查英文保留、中文輸出、合併條件、索引檢查順序，已接入 `scripts/verify-zh-tw.sh`。
- **目前驗證限制**：程式碼及測試已提交到原 Draft PR #146，但尚未在完整 checkout 上執行本批 Python／C99 腳本、完整 Android NDK／Gradle 建置與手機實測。未取得實機畫面確認前，不視為已完成發行驗收。

## 第 240 批 — FAangband 九種主題樓層提示繁體中文（2026-11-13）

- **確認來源**：`variant_lib/faangband/gamedata/themed.txt` 共 9 組 `name:`／`message:`。原始 `message:` 經 `generate.c::parse_themed_message()` 存入 `struct vault::typ`，並由 `cmd-cave.c::display_feeling()` 顯示。一般 vault 的 `typ` 另有用途，因此只改此顯示點。
- **九組繁中譯文**：元素戰場、巨龍、凶狼荒野、烏頓烈焰與惡魔、哥布林戰鼓、軍閥爭戰、巴拉希爾、伊甸人、Haudh-en-Ndengin。特定地名與人名保留可辨識的專名；展示文字透過 `angdroid_game_ui_text(level->typ, angdroid_zh_tw_enabled())` 查表，**沒有更改 themed.txt、樓層識別、房間模板、生成機率、存檔格式或英文顯示**。未命中的訊息原樣回退英文。
- **回歸守護**：擴充 `test_fa_dungeon_feelings_locale.c`，以正式翻譯 header 對照 9 組並檢查英文回退；`test_faangband_dungeon_feelings_locale_source.py` 會從 `themed.txt` 自動擷取全部 `message:`，確認字典和 C99 測試一一對應，並檢查 `generate.c` 的解析器及真正顯示端。沿用既有 `scripts/test-fa-dungeon-feelings-locale.sh` 和 `scripts/verify-zh-tw.sh`。
- **驗證界線**：本批需區分 GitHub 來源比對與實際測試執行；尚未進行完整 Android NDK／Gradle 建置或手機實測。PR #146 維持 Draft，不合併 master，不主動執行 GitHub Actions。

## 第 241 批 — FAangband 怪物行動與世界環境提示繁體中文（2026-11-13）

- **來源與範圍**：只修改 `faangband/mon-move.c`（怪物混亂撞地形、撞門、推擠、撿拾／壓碎物品、幽靈發言、驚醒）、`game-world.c`（物品充能通知、日夜、黑息、飢餓、回城／傳送／深層下降）、`cave-square.c`（符文、樓梯建立失敗、誘餌消失）的玩家可見訊息。
- **字典**：共新增 **19 組** `angdroid_game_ui_text()` 英文來源對照；包含 `terrain.txt` 中的 3 組 `confused-msg:` 與原本預設的 `stumbles`。資料檔中的英文 `confused-msg:` 不改動，僅顯示端轉成中文。未知地形描述保持英文句型，不會硬套中文標點。
- **含變數訊息**：保留原本怪物／物品名稱、拾取判斷、門鎖處理、更新旗標、聲音類別、掉落與消除操作；僅以繁中條件格式翻譯動作和詞序。原本玩家幽靈自訂說話內容保留原文。
- **相容性**：未修改 `terrain.txt`、怪物 ID、RNG 呼叫、時間推移、效果數值、存檔資料、模式或遊戲判定；英文模式仍保留既有格式字串。
- **測試**：`test_fa_monster_world_locale.c` 使用正式共用翻譯 header 檢查全部 19 組、英文原樣回退及動態格式範例；`test_faangband_monster_world_messages_locale_source.py` 核對資料檔、顯示端、關鍵英文格式、分支守護及測試接線。已接入 `scripts/verify-zh-tw.sh`。
- **驗證界線**：GitHub 來源比對不等同完整 C99／Python 或 Android APK／模擬器測試。維持原 Draft PR #146、不執行 GitHub Actions、不合併 master。


## 第 242 批 — FAangband 近戰後果、偷竊與幽靈挑戰繁體中文（2026-11-13）

- **13 組玩家受擊／偷竊固定字串**：`mon-blows.c` 的酸、電、火、冰傷害提示；生命力維持與流失；背包能量吸收；錢袋、偷竊未成功與所有錢幣被偷；背包成功防護等，透過 `angdroid_game_ui_text()` 顯示中英文字。保留英文來源與狀態邏輯。
- **14 組幽靈挑戰**：`game-world.c` 的 `challenge_text[14]` 保持英文原有順序，僅顯示時翻譯；仍呼叫 `randint0(14)` **恰好一次**，沒有更動隨機序列，幽靈名稱與種族名稱保留原始值。英文格式 `"%s, the %s %s"` 不變。
- **動態傷害／偷竊／吃掉食物**：修正 `%d` 傷害與被偷錢幣顯示，以及分堆或整件物品被偷、怪物偷竊失敗、食物被吃掉等帶 `%s`／`%c` 的訊息。遊戲中原本的 `context->obvious`、`gear_object_for_use()`、`monster_carry()`、`player_exp_lose()`、效果與傷害計算、損失金額及背包標籤都不動。帶有 `monster_blow_method_action()` 的動態戰鬥動詞仍需另行依遊戲資料來源審查，未擅自改寫。
- **離線檢查**：新增 `test_faangband_melee_ghost_locale.c`、`test-fa-melee-ghost-locale.sh` 與 `test_faangband_melee_ghost_locale_source.py`，驗證正式字典 27 組英文來源、原英文模式、未知值回退、14 組幽靈來源、動態格式與 RNG 呼叫位置；接入 `verify-zh-tw.sh`。
- **限制**：純原始碼比對與來源回歸檔案不等於完整 C99／Python 套件已在 checkout 執行，也不是 Android NDK／Gradle／實機驗收；仍維持 Draft PR #146，未合併 master、未執行 GitHub Actions。

## 第 243 批 — FAangband 怪物近戰動態動詞與抵抗訊息（2026-11-13）

- 已核對 `variant_lib/faangband/gamedata/blow_methods.txt`，確認 `act:` 共 **57** 條且沒有重複。涵蓋 17 種基本近戰、8 種羞辱與 32 種怪物牢騷；把這 **57** 組模板逐一納入共用顯示字典，保留每條 `{target}`、`{oftarget}` 標記，**不修改遊戲原始資料**。此外翻譯恐懼／麻痺抵抗成功的 **2** 條 `save_msg`。
- 將 `monster_blow_method_action()` 的實作抽成內部顯示格式函式，正式公開 API 仍回傳原英文動詞；只有繁中 UI 顯示時翻譯抽出的模板，再替換目標怪物名、玩家的「你／你的」代碼。未命中的字典保留原英文動作與原英文空白／句點。
- 繁中句子移除英文動詞前的空白，使用中文句號；已包含句尾「！」「？」「。」的模板不額外加句號。傷害顯示 `(%d)`、怪物訊息種類、`string_free(act)` 釋放路徑保留。
- `randint0(method->num_messages)` 仍只在所選動作函式呼叫一次；怪物招式候選數目、選擇順序、傷害／效果／AI／存檔資料均維持。
- 加入來源覆蓋與實際翻譯表 C99 測試，接入 `scripts/verify-zh-tw.sh`。離線靜態來源核對不等於完整 C99/Android APK 測試已執行；維持原 Draft PR #146，不執行 Actions、也不合併 `master`。

## 第 244 批 — FAangband 怪物訊息與受傷叫聲（2026-11-13）

- 已直接比對 `list-mon-message.h` 的 **58 條非空訊息**與 `pain.txt` 的 **84 筆痛苦訊息（其中 65 條唯一）**，完整新增 **123 個**中文顯示對照；`list-mon-message.h`、`pain.txt` 的怪物 ID、訊息編號與英文原文一律保留。
- `mon-msg.c` 在 `get_message_text()` 中查找原始英文模板，接著使用既有的單複數 `[singular|plural]` 展開器；摩瑞亞特殊死亡訊息保留「一聲／數聲」數量區分。資料未命中翻譯時回退原英文句型，避免將英文動作與中文主詞硬湊在一起。
- `get_subject()` 僅在該筆訊息成功翻譯時使用中文主詞、怪物數量及「（畫面外）」標記；原英文路徑仍保留 `The`、複數拼法、空格及 `(offscreen)`。不更動已決定的可見性、訊息堆疊、傷害與聲音種類。中文群體傷害數字改顯示「（平均 %d）」；原英文依舊是 `(average %d)`。
- 顯示文字緩衝區由 60 bytes 增至 256 bytes，以容納已核對的 UTF-8 翻譯，避免中文在一般句子中被截成半個字；怪物名稱、種族名稱、回合處理與狀態不變。
- 新增 C99 正式翻譯表雙語回歸測試，以及 Python 對照原始資料完整覆蓋的來源檢查，並接入 `scripts/verify-zh-tw.sh`。此回合的 GitHub 靜態核對不等於已完整執行 C99、Python 或 Android 實機驗證。

## 第 245 批 — FAangband 怪物清單窄螢幕及回憶顯示（2026-11-13）

- **確認原有問題**：`faangband/ui-mon-list.c` 原先直接使用 `max_width - 2 - utf8_strlen(location) - 1` 及 `full_width - utf8_strlen(asleep)`；在窄視窗時，負欄數轉為 `size_t` 會造成無號下溢，導致名稱截斷與 `printf` 欄寬異常。現在採用專案已實作的 `angdroid_monster_row_fit()` 和 `angdroid_monster_sleep_fits()`：位置與睡眠／中立標記都是可省略的，優先保留怪物名稱。
- **互動視窗**：原本的 `safe_width = MIN(width - 40, ...)` 在手機少於 40 欄時可能為負；改採 `angdroid_monster_list_popup_fit()`，在小於 12 欄或 7 行時安全不開啟；靜態怪物子窗小於 7 欄時不繪製，避免不完整符號欄位。
- **UTF-8 與欄寬**：距離以 `angdroid_list_relative_location()` 完整輸出「北／南／東／西」；`angdroid_utf8_display_columns()` 計算真正終端欄位、`angdroid_utf8_clip_columns()` 不切斷漢字、`angdroid_utf8_printf_field_bytes()` 把顯示欄寬轉為 C `printf` field bytes。英文位置格式不變，無法放下完整距離時只省略距離而非截半。
- **繁中清單資訊**：可見／感知區段標題、怪物數量、睡眠或中立狀態、幻覺訊息、隱藏其他怪物數量與按 `x` 切換經驗排序提示改為繁中；未變動怪物是否可見、排序計算、數量統計、顏色或圖示。
- **名稱與回憶**：`mon-desc.c::get_mon_name()` 在繁中模式產生「%d 隻」與玩家幽靈 `[U] 姓名（種族）` 字串，利用 `angdroid_utf8_copy_complete()` 確保狹小輸出緩衝區不截斷中文字；英文模式維持原格式。`ui-mon-lore.c` 的一般怪物／幽靈回憶標題改用適當繁中格式，並新增 2 組「完全了解怪物」及任務怪物提示翻譯。遊戲資料、怪物唯一性標記、知識判定、紫色獨特怪物設定均保留。
- **測試**：新增 `tests/native/test_faangband_monster_list_locale.c` 與 `scripts/test-fa-monster-list-locale.sh`，以正式共用 layout、方向、顯示欄函式測試短寬度、狹窄視窗、中文字元及英文回退；新增 `tests/test_faangband_monster_list_locale_source.py` 保護分支接線與回憶顯示邏輯，並接入 `scripts/verify-zh-tw.sh`。
- **驗證界線**：GitHub 來源檢查不是已在完整 checkout 執行 C99／Python 套件、Android SDK/NDK APK 或實機驗證。維持原 Draft PR #146，不執行 Actions，也不合併 `master`。

## 第 246 批 — FAangband 怪物回憶動態資訊中文化（2026-11-13）

- **不修改遊戲資料和知識判定**：在 `app/src/main/cpp/faangband/mon-lore.c` 對繁體中文畫面新增顯示分支，原有英文組句保留；不改 `lore->flags`、`lore->armour_known`、`lore->drop_known`、`lore->sleep_known`、`lore->pkills`／`deaths`、怪物種族、AI、掉落設定、回合與存檔。英文模式仍走原來的分支。
- **擊殺紀錄**：唯一怪物生死、祖先死亡、玩家本次或歷代擊殺、從未交戰等分支以完整中文句子表示；維持 `RF_UNIQUE`、`race->max_num` 與 `lore` 紀錄的判斷與紅色警示。
- **血量、護甲與命中率**：僅在 `lore->armour_known` 時顯示；保留 `race->avg_hp`／`race->ac`、`hit_chance()`、`random_chance_scaled(..., 100)` 及原本的藍色數值。唯一怪物與普通怪物區分「生命值／平均生命值」。
- **經驗值**：繼續在 `race->rarity` 合法時描述，沿用原本的整數與百分位小數公式；繁中版不強行組合英文的 `a/an`、`st/nd/rd/th`，而顯示「擊敗這隻怪物可獲得 X 點經驗值（以 N 級角色計算）」。`player->lev` 和經驗值公式不變。
- **掉落物品**：僅在 `lore->drop_known` 時輸出，沿用原本 `mon_create_drop_count(..., &nspec)`、`RF_ONLY_ITEM`／`RF_ONLY_GOLD`、`RF_DROP_GOOD`／`RF_DROP_GREAT`。1 件、1 至 2 件、最多 N 件、特定物品數量分開顯示；維持原數量、品級及來源資訊。
- **警戒、朋友與移動**：只有已知 `sleep_known` 時顯示警戒／聽力距離（原 `10 * race->hearing` 不變）；成群狩獵仍由 `RF_GROUP_AI` 決定。移動描述依既有 `RFT_RACE_A`／`RFT_RACE_N`、`RF_FORCE_DEPTH`、`RF_RAND_25/50`、`RF_NEVER_MOVE`、`OPT(player, effective_speed)` 決定，新增完整中文句子並保留速度倍數與玩家速度比較。
- **30 個顯示詞彙**：共 **11** 條警戒程度、**9** 條速度描述與 **10** 條怪物類型或形容詞加入 `angband/zh-tw-ui.h`。仍使用 `angdroid_game_ui_text()` 做顯示替換，沒有變更 `list-mon-race-flags.h` 原始資料與數值門檻。
- **離線測試**：新增 `tests/native/test_faangband_lore_display_locale.c`／`scripts/test-fa-lore-display-locale.sh` 的嚴格 C99 正式字典測試，以及 `tests/test_faangband_lore_display_locale_source.py` 的原始資料對照／公式／可見性／英文分支檢查，已接入 `scripts/verify-zh-tw.sh`。
- **後續未涵蓋**：`lore_append_abilities()` 的旗標能力串列、`lore_append_spells()`、`lore_append_attack()` 的法術與近戰詳細文法仍須逐個來源核對；本批不宣稱所有怪物知識全文已翻譯。
- **驗證界線**：可從 GitHub 執行定向原始碼一致性核對；這不同於完整 C99／Python 回歸、Android NDK／Gradle 或實機／模擬器建置驗證。維持同一 Draft PR #146，不觸發 GitHub Actions，不合併 `master`。

## 第 247 批 — FAangband 怪物能力、弱點、抗性與免疫（2026-11-13）

- 直接核對 `app/src/main/cpp/faangband/list-mon-race-flags.h`：`RFT_ALTER`、`RFT_DET`、`RFT_VULN`、`RFT_VULN_I`、`RFT_RES`、`RFT_PROT` 共有 **35 筆英文描述，33 條唯一詞彙**（`fire`、`cold` 同時作為弱點及抗性，須共用字典鍵），全部加入正式 `app/src/main/cpp/angband/zh-tw-ui.h` 顯示對照。原始遊戲旗標、數值與資料檔完全不改。
- `lore_append_clause()` 保留 `rf_count()`、`rf_next()` 與原始色彩顯示，僅在繁中模式透過 `angdroid_game_ui_text(describe_race_flag(flag), zh_tw)` 對已選取的旗標翻譯，按中文句法選擇「、」「與」「或」，將句尾 `.  ` 轉為「。  」；英文模式的逗號、and/or、空格與原英文描述保留。
- 新增純顯示 `faangband/mon-lore-display.h`：`angdroid_fa_lore_separator()` 與 `angdroid_fa_lore_sentence_end()`，可由不依賴遊戲引擎的 C99 測試直接檢驗中文字串串接及後備分支。
- `lore_append_abilities()` 沿用全部 `known_flags` 和 `lore->flags` 資料過濾：開門／穿牆／拾取物品、隱形／冷血／心靈感應、快速繁殖／再生／飛行／領地性、發光／散播黑暗、火／寒冷／毒與其他元素的弱點、抗性、不抗性，以及恐懼／暈眩／混亂／睡眠等防護均以中文句型顯示。
- **保護原始推理與資訊揭露**：保留 `RFT_VULN`、`RFT_VULN_I`、`RFT_RES`、`RFT_PROT` 與 `rf_inter(..., known_flags)` 的完整順序，對原始 `describe_race_flag()` 的字串相等比較、非弱點視作抗性、已知不抗性、`RF_UNDEAD` 移除 `RF_IM_NETHER` 等特殊規則不變。不會因繁中顯示提前揭露未知抗性。
- `start` 緩衝區由 40 增至 96 bytes，容納中文句子片語而不因 UTF-8 編碼長度超出；只有訊息顯示發生變化，不影響戰鬥、怪物學識、AI、存檔與能力運作。
- 新增 `tests/native/test_faangband_lore_abilities_locale.c`（正式字典全部 33 組中英回退及「、／與／或」邊界）、`scripts/test-fa-lore-abilities-locale.sh`（主機嚴格 C99）、`tests/test_faangband_lore_abilities_locale_source.py`（35 筆原始旗標、33 組翻譯對照、知識旗標過濾、亡靈例外、回歸測試接線），並更新 `scripts/verify-zh-tw.sh`。
- **驗證限制**：本批 GitHub 靜態原始碼一致性核對不等於已在完整 checkout 執行 C99／Python 回歸、NDK／Gradle 或 Android 實機測試。維持原 Draft PR #146，不執行 GitHub Actions、不合併 `master`。

## 第 248 批 — FAangband 法術與近戰回憶中文化（2026-11-13）

- **實際資料覆蓋**：依 `variant_lib/faangband/gamedata/monster_spell.txt` 的 **116 條不重複 `lore:` 法術回憶文字**、`blow_methods.txt` 的 **19 條 `desc:` 動作文字**及 `blow_effects.txt` 的 **26 條 `desc:` 攻擊效果**核對，三份資料合計 **156 個不重複英文字串**（交叉重複 5 組）。由於同名 `attack`、`fire` 等詞可能出現在其他遊戲畫面，這批新建 **`faangband/mon-lore-terms.h` 的專用顯示詞表**，而不是把通用詞放進共用 `angband/zh-tw-ui.h`；遊戲原始資料檔維持英文，供解析器與遊戲規則使用。
- **法術與吐息**：`mon-lore.c::lore_append_spell_clause()` 在原有 `rsf_next()`、`spell_color()`、`mon_spell_lore_damage(..., know_hp)` 之後翻譯已選定的回憶描述；中文「或／、」連接多個招式，傷害顯示為中文括號，英文模式保留原空格與 `(%d)`。
- **施法頻率與知識限制**：`lore_append_spells()` 的先天攻擊、吐息、其他法術三組仍依 `lore->spell_flags`、`RST_INNATE`、`RST_BREATH` 與 `rf_has(known_flags, RF_SMART)` 篩選。對 `lore->innate_freq_known`、`lore->spell_freq_known` 仍顯示確定頻率，對只觀測到施法的 `lore->cast_innate`、`lore->cast_spell` 仍顯示估計頻率，計算 `100 / race->freq_*` 與 `100 / approx_frequency` 不變。新增顯示層 `lore_append_spell_frequency()`，中文說「每 N 次施放 1 次」或「約每 N 次施放 1 次」，英文原 `1 time in N` 格式不動。缺少先天施法頻率但已有招式資料時，繁中補句尾，避免與下一句黏接；`ref_race` 仍在退出前還原。
- **近戰回憶**：`lore_append_attack()` 的 `RF_NEVER_BLOW`、`lore->blow_known[i]`、招式定義檢查、已知／總攻擊數量完全保留。顯示「牠能擊打／咬……，效果為……（2d6，75%）」等文字；`dice.base`、`dice.dice`、`dice.sides`、`dice.m_bonus`、`hit_chance()`、`random_chance_scaled(..., 100)`、`randcalc(..., AVERAGE)` 與 `total_centidamage / 100` 不變。當仍有未知招式時，平均傷害的「至少」標記保留，避免將局部已知資訊誤說成完整資訊。
- **雙語與色彩隔離**：同一個 `angdroid_fa_lore_term()` 僅在繁中模式讀取 FAangband 專用對照，英文模式回傳原指標；未知來源不會被錯誤套用別處的中文翻譯。原數值及語句的 `COLOUR_L_GREEN`、`COLOUR_L_RED`、`COLOUR_L_BLUE` 與 `blow_color()` 色彩運算保留。
- **回歸測試**：新增 `tests/native/test_faangband_lore_spells_attacks_locale.c`（156 條正式 C99 翻譯字典、英文原字串回退、中文句型）、`scripts/test-fa-lore-spells-attacks-locale.sh`、`tests/test_faangband_lore_spells_attacks_locale_source.py`（三份正式 gamedata 完整來源集合、未知攻擊保護、傷害／施法頻率公式、英文文字回退）。已接入 `scripts/verify-zh-tw.sh`。
- **驗證界線**：Github 重新讀取程式碼後的靜態一致性核對與資料來源交叉對照，**不是**完整 checkout 實際執行的 C99／Python 回歸、NDK／Gradle 或模擬器／實機驗證。PR #146 維持 Draft，不觸發 Actions，不合併 `master`。

## 第 249 批 — FAangband 怪物實際施法通知中文化（2026-11-13）

- **精確核對正式來源**：`variant_lib/faangband/gamedata/monster_spell.txt` 目前有 **116 條 message-vis**、**116 筆 message-invis（80 條唯一）**、**11 條 message-miss** 與 **15 筆 message-save（14 條唯一）**，總計 **221 條唯一訊息模板**。四種通知分別建立完整繁體中文顯示對照，所有 `{name}`、`{pronoun}`、`{target}`、`{type}`、`{oftype}` 佔位符完整保留。未變更 `monster_spell.txt`、`mon-init.c` 或怪物法術數值與效果。
- **局部詞表避免汙染其他 UI**：`faangband/mon-spell-messages-visible-zh-tw.h` 包含 116 條可見施法模板；`faangband/mon-spell-messages-other-zh-tw.h` 包含 80 條視野外、11 條未命中、14 條抵抗成功模板；`faangband/mon-spell-message-locale.h` 將兩份字典合成僅供 FAangband 施法通知使用的 `angdroid_fa_spell_message_locale()`，無對照或語言模式為英文時直接回傳原始字串，種族自訂的未知 `MON_ALTMSG` 也不會被猜測翻譯。
- **正確的時序**：`mon-spell.c::spell_message()` 仍先依 `seen`、`hits` 及 `MON_ALTMSG_UNSEEN`／`MON_ALTMSG_MISS`／`MON_ALTMSG_SEEN` 選擇正式訊息；原始空字串／無法顯示的訊息仍直接返回。**選擇完成後、展開動態佔位符前**才對整條模板做精確翻譯；只有成功命中翻譯的訊息，才使用中文「這隻」、目標「你」、所有格「牠的」及 `{oftype}` 的中文括號，未翻譯的自訂英文仍走原語法。
- **不洩漏視野外資訊**：保留 `if (!seen && t_mon) return;` 的「視野外對其他怪物施法不顯示」分支；其餘視野外訊息仍以「某個東西」「你聽見……」等未指明施法者的句子呈現。可見施法者和攻擊目標仍由原本的 `monster_desc()` 及 `MDESC` 旗標提供，僅一般非唯一、非幽靈名稱可在已翻譯的句型下去除英文 `the` 冠詞。唯一怪物、玩家幽靈名字與來源識別資訊不改。
- **命中、傷害、抵抗不變**：`spell_tag_lookup()`、`msgt(spell->msgt, ...)`、`check_hit()`／`test_hit()`、`randint0(100) < player->state.skills[SKILL_SAVE]`、`spell_check_for_fail_rune()`、`effect_do()` 全部保留。成功抵抗的 `save_message` 只在既有擲骰成功分支內經翻譯後顯示；未命中仍採用原本的 `message-miss` 優先順序和傷害規則。鞭擊的 `{type}`／`{oftype}` 維持原 `projections[type].lash_desc` 來源，只在已翻譯的訊息做 FAangband 專屬顯示對照。
- **防止未來資料拼接失效**：`mon-init.c` 的 `message-*` parser 使用字串追加，若同個怪物法術 power-cutoff 出現多次相同欄位，實際執行期模板可能和單行來源不一致。新增 Python 來源檢查要求每個 `name`／`power-cutoff` 之間，每個 `message-*` 類別最多一筆，來源更新後不能默默失去翻譯。
- **回歸測試**：新增 `tests/native/test_faangband_spell_messages_locale.c`（221 條正式中英精確對照、未知字串指標回退、一般怪物主詞、可見／未命中／抵抗範例）、`scripts/test-fa-spell-messages-locale.sh`（host C99 `-std=c99 -Wall -Wextra -Werror -pedantic`）、`tests/test_faangband_spell_messages_locale_source.py`（四種資料的完整來源比對、佔位符一致性、視野與空字串遮蔽、特殊覆寫、抵抗與法術效果先後順序），已接進 `scripts/verify-zh-tw.sh`。
- **驗證範圍**：GitHub 原始碼／資料的靜態核對不能取代完整本機 checkout 執行 C99／Python、Android NDK／Gradle 建置或實機驗收。沿用 Draft PR #146，所有本批 commit 帶 `[skip ci]`，未執行 GitHub Actions，未合併 `master`。

## 第 250 批 — FAangband 種族專屬施法訊息與長訊息 UTF-8 安全（2026-11-13）

- **已確認覆寫資料來源**：`variant_lib/faangband/gamedata/monster.txt` 目前有 **49 筆**種族專屬 `message-*` 施法覆寫（`message-vis` 3 筆、`message-invis` 46 筆、`message-miss` 0 筆），合併相同文字後為 **17 條唯一字串**。來源示例：`Something slurps.`、`{name} caws three times.`、`The baleful presence of {name} inspires your enemies.`。其中包括視野外只能聽到的咆哮、嘶嘶聲與其他含糊描述。
- **避免重複翻譯字典**：既有 `app/src/main/cpp/angband/zh-tw-mon-spell-alt.inc` 已有完全對應這 **17 個精確來源**的中文對照。本批新增 `faangband/mon-spell-messages-alternate-zh-tw.h`，透過 `#include "../angband/zh-tw-mon-spell-alt.inc"` 直接重用；`mon-spell-message-locale.h` 在上批的 221 條通用施法模板都無匹配時才查種族專屬字典。沒有修改 `monster.txt` 或建立兩份各自維護的翻譯清單，未知／空白自訂模板仍按原文／原本遮蔽規則處理。
- **保留完整顯示時序**：`mon-spell.c::spell_message()` 繼續先由 `seen`、`hits`、`MON_ALTMSG_SEEN/UNSEEN/MISS` 與 `level->message/blind_message/miss_message` 選出唯一有效模板，且視野外對其他怪物施法的隱藏規則不變。選完原模板後才翻譯，然後由原有的 `spell_tag_lookup()` 展開 `{name}`、`{pronoun}`、`{target}`、`{type}`、`{oftype}`。原來源資料、訊息種類 `spell->msgt`、傷害與命中規則不變。
- **中文主詞與 UTF-8**：`angdroid_fa_spell_subject()` 改用 `angdroid_utf8_copy_complete()`／`angdroid_utf8_append_complete()` 製作「這隻」與怪物名稱，以字元為單位限制 byte buffer，避免 UTF-8 漢字從中間截斷。相應的 `mon-spell.c` 暫存怪物名稱從 80 bytes 增至 256 bytes；在已成功翻譯的中文模板中，不再套入為英文 `RF_NAME_COMMA` 設計的尾端英文逗號。
- **長訊息完整字元輸出**：原施法通知以 `strnfcat()` 寫入 1024-byte buffer，若最終長字串在漢字中間達到限制，可能把不完整 UTF-8 送入 `msgt()`。現在只對**已成功翻譯**的中文訊息在輸出前使用另一份 `safe_buf[sizeof(buf)]` 並以 `angdroid_utf8_copy_complete()` 整理；未翻譯的英文（包括未知種族專屬通知）維持原輸出分支。此保護針對 byte 邊界；終端換行與可見欄寬仍由原文字介面負責。
- **測試接線**：新增 `tests/native/test_faangband_spell_alternates_utf8.c`（正式來源的 17 條中文對照、英文與未知字串回退、狹窄 3／4／9／10-byte 主詞與完整 UTF-8 複製）、`scripts/test-fa-spell-alternates-utf8.sh`（host 嚴格 C99）及 `tests/test_faangband_spell_alternates_utf8_source.py`（49 筆原資料、17 條來源鍵及相同佔位符、`MON_ALTMSG` 選擇／隱藏分支、名稱／UTF-8、訊息順序、原 C99 runner），並納入 `scripts/verify-zh-tw.sh`。同步更新上批 Python guard，避免把舊的 `snprintf()` 誤判為唯一合格的主詞輸出。
- **驗證界線**：本批從 GitHub 重新讀取正式來源後可做靜態一致性核對；這仍**不是**已在完整 checkout 執行 C99/Python 腳本、Android Gradle／NDK 編譯或模擬器／實機驗證。維持原 Draft PR #146、所有提交帶 `[skip ci]`、未執行 GitHub Actions、不合併 `master`。
