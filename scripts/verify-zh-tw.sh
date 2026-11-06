#!/bin/sh
# Run offline localization checks and save a shareable combined log.
# Requires Python 3, a JDK (javac/java), and a host C compiler (cc).
# Optional --android also runs a Gradle/NDK debug APK build when the local
# Android SDK, NDK and Gradle wrapper are installed on the workstation.
# Neither mode runs emulator/physical-device gameplay acceptance tests.
set -u

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
RUN_ANDROID=0
if [ "${1:-}" = "--android" ]; then
    RUN_ANDROID=1
    shift
fi
if [ "$#" -gt 1 ]; then
    printf 'Usage: sh scripts/verify-zh-tw.sh [--android] [report-file]\n' >&2
    exit 2
fi
REPORT="${1:-$ROOT/zh-tw-validation.log}"
: > "$REPORT" || exit 1
failed=0

run_check() {
    label="$1"
    shift
    printf '\n===== %s =====\n' "$label" >> "$REPORT"
    if "$@" >> "$REPORT" 2>&1; then
        printf 'PASS: %s\n' "$label" >> "$REPORT"
        printf 'PASS: %s\n' "$label"
    else
        printf 'FAIL: %s\n' "$label" >> "$REPORT"
        printf 'FAIL: %s\n' "$label"
        failed=1
    fi
}

printf 'Angband zh-TW offline verification\n' >> "$REPORT"
printf 'Repository: %s\n' "$ROOT" >> "$REPORT"
run_check "JDK 21 / Gradle 9.1 / AGP 9 build configuration" python3 "$ROOT/scripts/check_jdk21_build.py"
run_check "Android SDK/NDK preflight regression" python3 "$ROOT/tests/test_android_sdk_preflight.py"
run_check "Android string resources" python3 "$ROOT/scripts/check_zh_tw_resources.py"
run_check "Independent app-language source wiring" python3 "$ROOT/scripts/check_app_language_wiring.py"
run_check "Java Unicode terminal" sh "$ROOT/scripts/test-terminal-unicode.sh"
run_check "Native C UTF-8 and column-width helpers" sh "$ROOT/scripts/test-native-utf8.sh"
run_check "Legacy NPP bool headers across supported C standards" sh "$ROOT/scripts/test-native-bool-headers.sh"
run_check "Native shared-library symbol ownership" python3 "$ROOT/scripts/check_native_link_ownership.py"
run_check "Native/Java Unicode protocol wiring" python3 "$ROOT/scripts/check-native-unicode-contract.py"
run_check "Filtered native menu keyboard hint integration" python3 "$ROOT/tests/test_menu_key_hints_source.py"
run_check "Visible-page native menu shortcut consistency" python3 "$ROOT/tests/test_menu_visible_tags_source.py"
run_check "Finite invalid-row menu navigation" python3 "$ROOT/tests/test_menu_navigation_source.py"
run_check "Inventory and equipment UTF-8 display safety" python3 "$ROOT/tests/test_object_list_text_source.py"
run_check "Inventory and equipment bilingual captions and selection invariants" python3 "$ROOT/tests/test_inventory_list_locale_source.py"
run_check "Rotated item-selection and compact shop prompts" python3 "$ROOT/tests/test_item_selection_rotation_source.py"
run_check "Knowledge glyph strict Unicode input and bilingual autoinscriptions" python3 "$ROOT/tests/test_knowledge_glyph_codepoint_locale_source.py"
run_check "Item picker numeric quiver tag, localized cancel hint and CJK recall titles" python3 "$ROOT/tests/test_item_picker_tag_locale_source.py"
run_check "UI-only item name localization and export isolation" python3 "$ROOT/tests/test_visible_item_name_localization.py"
run_check "Known-item classifiers and translated artifact/ego grammar" python3 "$ROOT/tests/test_item_name_grammar_localization.py"
run_check "Equipment combat and inscription UTF-8 safety" python3 "$ROOT/tests/test_item_details_localization.py"
run_check "Equipment comparison two-item detail zh-TW isolation" python3 "$ROOT/tests/test_equipment_comparison_detail_locale_source.py"
run_check "Equipment comparison screen-only CJK short names, stable sort/export" python3 "$ROOT/tests/test_equip_cmp_name_display_source.py"
run_check "Equipment compare tiny-terminal BAIL and label coordinate guards" python3 "$ROOT/tests/test_equip_cmp_tiny_terminal_source.py"
run_check "Equipment compare filtered touch selection, rotation and zh-TW hints" python3 "$ROOT/tests/test_equip_cmp_touch_selection_source.py"
run_check "Flavored noun grammar and awareness isolation" python3 "$ROOT/tests/test_flavored_object_localization.py"
run_check "Generated scroll title display and RNG isolation" python3 "$ROOT/tests/test_scroll_title_localization.py"
run_check "Item lore and character export isolation" python3 "$ROOT/tests/test_item_story_localization.py"
run_check "Ego knowledge names, clipping and recall titles" python3 "$ROOT/tests/test_ego_knowledge_screen.py"
run_check "Symbol query tval and terrain display-only translation" python3 "$ROOT/tests/test_symbol_query_localization.py"
run_check "Store columns on narrow/rotated terminals" python3 "$ROOT/tests/test_store_column_layout_source.py"
run_check "Canonical shop title and inscription confirmation zh-TW safety" python3 "$ROOT/tests/test_store_inscription_locale_source.py"
run_check "Dynamic store Buy alias and popup text safety" python3 "$ROOT/tests/test_menu_dynamic_choice_source.py"
run_check "Dynamic popup placement on tiny terminals" python3 "$ROOT/tests/test_menu_popup_layout_source.py"
run_check "Store purchase cancel and stock-index safety" python3 "$ROOT/tests/test_store_stock_guard_source.py"
run_check "Explicit store confirmation and Chinese quantity labels" python3 "$ROOT/tests/test_store_confirmation_source.py"
run_check "Strict global quantity parsing and Chinese retry" python3 "$ROOT/tests/test_quantity_input_source.py"
run_check "Object context menu bilingual CJK layout and safe quantity retry state" python3 "$ROOT/tests/test_item_context_quantity_locale_source.py"
run_check "Gear wear/remove bilingual prompts and UTF-8-safe confirmation" python3 "$ROOT/tests/test_gear_confirmation_locale_source.py"
run_check "Scroll, food, potion, generic use and refuel translated with item remainder safety" python3 "$ROOT/tests/test_item_use_prompts_locale_source.py"
run_check "Object use-effect message translations and cancellation/charge rollback" python3 "$ROOT/tests/test_item_use_effect_messages_source.py"
run_check "Activation and artifact dynamic templates, name rendering and rollback" python3 "$ROOT/tests/test_activation_messages_locale_source.py"
run_check "Spell effect summary labels, radius, length, projection and UTF-8" python3 "$ROOT/tests/test_spell_effect_info_locale_source.py"
run_check "Mage spell detailed descriptions and exact original text" python3 "$ROOT/tests/test_spell_descriptions_locale_source.py"
run_check "Druid spell details and shared Ranger exception" python3 "$ROOT/tests/test_druid_spell_descriptions_locale_source.py"
run_check "Priest prayer descriptions and Paladin Heroism exception" python3 "$ROOT/tests/test_priest_spell_descriptions_locale_source.py"
run_check "Necromancer spells, self-damage, shapechanges and original matching" python3 "$ROOT/tests/test_necromancer_spell_descriptions_locale_source.py"
run_check "Paladin and Rogue original spells, Heroism thresholds and source matching" python3 "$ROOT/tests/test_paladin_rogue_spell_descriptions_locale_source.py"
run_check "Ranger spell descriptions, Druid Herbal Curing and restrictions" python3 "$ROOT/tests/test_ranger_spell_descriptions_locale_source.py"
run_check "Blackguard original spells and all 163 descriptions covered" python3 "$ROOT/tests/test_blackguard_spell_descriptions_locale_source.py"
run_check "Spell long descriptions CJK terminal wrapping source guards" python3 "$ROOT/tests/test_spell_screen_wrap_source.py"
run_check "Android quantity popup confirmation / localization wiring" python3 "$ROOT/tests/test_quantity_popup_source.py"
run_check "Atomic Android quantity sequence and queue isolation" python3 "$ROOT/tests/test_quantity_queue_source.py"
run_check "Native quantity session lifetime and asynchronous stale popup guards" python3 "$ROOT/tests/test_quantity_prompt_session_source.py"
run_check "Tagged quantity FIFO and FAangband native completion" python3 "$ROOT/tests/test_quantity_tagged_queue_source.py"
run_check "FrogComposband quantity lifecycle and zh-TW quantity prompts" python3 "$ROOT/tests/test_frog_quantity_locale_source.py"
run_check "NPP710 and Sil-Q quantity completion and zh-TW prompt resources" python3 "$ROOT/tests/test_variant_quantity_session_source.py"
run_check "NPP041 native quantity bridge, language-specific all semantics and host C smoke" python3 "$ROOT/tests/test_npp041_quantity_bridge.py"
run_check "Android quantity slider JDK overflow regression" sh "$ROOT/scripts/test-quantity-popup.sh"
run_check "Queued store input and stale cursor invalidation" python3 "$ROOT/tests/test_store_input_invalidation_source.py"
run_check "Scrolled store popups and empty-stock commands" python3 "$ROOT/tests/test_store_menu_flow_source.py"
run_check "Selected native menu action state" python3 "$ROOT/tests/test_menu_action_guard_source.py"
run_check "Native menu CJK column clipping" python3 "$ROOT/tests/test_menu_text_fit_source.py"
run_check "Compact menu layout on Android rotation" python3 "$ROOT/tests/test_menu_compact_layout_source.py"
run_check "Native blank-cell touch safety" python3 "$ROOT/tests/test_menu_hit_test_source.py"
run_check "Narrow native menu column safety" python3 "$ROOT/tests/test_menu_column_width_source.py"
run_check "Native checker Python syntax and parser integrity" python3 "$ROOT/tests/test_zh_tw_checker_integrity.py"
run_check "Current native localization display guards and negative mutations" python3 "$ROOT/tests/test_native_checker_current_display_contracts.py"
run_check "Native translated message branch scope (dangling else)" python3 "$ROOT/tests/test_native_locale_branch_braces.py"
run_check "Targeting help printf-format security (Clang/GCC)" python3 "$ROOT/tests/test_target_help_printf_security.py"
run_check "Dynamic shape-change effects C99 renderer" python3 "$ROOT/tests/test_shape_effect_render_source.py"
run_check "Option descriptions and CJK menu display safety" python3 "$ROOT/tests/test_options_localization_source.py"
run_check "Ignore menus, subwindow labels and color editor translations" python3 "$ROOT/tests/test_ignore_options_localization_source.py"
run_check "Dynamic quality-ignore and screen export zh-TW prompts" python3 "$ROOT/tests/test_ignore_quality_export_source.py"
run_check "Version screen and ignore-item picker bilingual UI guards" python3 "$ROOT/tests/test_version_ignore_pick_source.py"
run_check "Activation, curse and item-property translation source coverage" python3 "$ROOT/tests/test_activation_effects_coverage.py"
run_check "Activation, curse and item-property C99 smoke compilation" sh "$ROOT/scripts/test-activation-effects-locale.sh"
run_check "Native Angband game zh-TW text isolation" python3 "$ROOT/scripts/check_native_game_localization.py"
run_check "Canonical translation coverage audit" python3 "$ROOT/scripts/audit-zh-tw-coverage.py"
run_check "Coverage audit JSON regression" python3 "$ROOT/tests/test_localization_coverage.py"

android_debug_build() {
    if [ ! -f "$ROOT/gradlew" ]; then
        printf 'Missing gradlew in the project root\n'
        return 1
    fi
    # Android Studio usually creates local.properties for the SDK path.
    # It is no longer required solely for the optional ABI_FILTERS property.
    (
        cd "$ROOT" || exit 1
        printf 'JAVA_HOME=%s\n' "${JAVA_HOME:-<not set>}"
        java -version || return 1
        javac -version || return 1
        sh ./gradlew --version || return 1
        if [ -n "${ABI_FILTERS:-}" ]; then
            printf 'Debug native ABIs (environment override): %s\n' "$ABI_FILTERS"
            sh ./gradlew --no-daemon --warning-mode=summary --stacktrace "-PABI_FILTERS=$ABI_FILTERS" :app:assembleDebug
        else
            printf 'Debug native ABIs: project default arm64-v8a (unless overridden)\n'
            sh ./gradlew --no-daemon --warning-mode=summary --stacktrace :app:assembleDebug
        fi
    )
}

if [ "$RUN_ANDROID" -eq 1 ]; then
    printf '\n===== Android SDK/NDK/CMake preflight =====\n' >> "$REPORT"
    if python3 "$ROOT/scripts/check_android_sdk.py" >> "$REPORT" 2>&1; then
        printf 'PASS: Android SDK/NDK/CMake preflight\n' >> "$REPORT"
        printf 'PASS: Android SDK/NDK/CMake preflight\n'
        run_check "Android Gradle/NDK debug APK build" android_debug_build
    else
        printf 'FAIL: Android SDK/NDK/CMake preflight\n' >> "$REPORT"
        printf 'FAIL: Android SDK/NDK/CMake preflight\n'
        printf 'SKIP: Android Gradle/NDK debug APK build (install SDK components first)\n' >> "$REPORT"
        failed=1
    fi
fi

printf '\nResult: %s\n' "$(if [ "$failed" -eq 0 ]; then echo PASS; else echo FAIL; fi)" >> "$REPORT"
printf 'Verification report: %s\n' "$REPORT"
exit "$failed"
