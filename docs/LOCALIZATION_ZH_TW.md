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
