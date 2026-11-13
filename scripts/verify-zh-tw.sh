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
run_check "Android screen tap long-press and multi-pointer cancellation" python3 "$ROOT/tests/test_screen_tap_guard_source.py"
run_check "Android to JNI spell touch lifecycle generation" python3 "$ROOT/tests/test_spell_touch_lifecycle_source.py"
run_check "Filtered native menu keyboard hint integration" python3 "$ROOT/tests/test_menu_key_hints_source.py"
run_check "Visible-page native menu shortcut consistency" python3 "$ROOT/tests/test_menu_visible_tags_source.py"
run_check "Finite invalid-row menu navigation" python3 "$ROOT/tests/test_menu_navigation_source.py"
run_check "Inventory and equipment UTF-8 display safety" python3 "$ROOT/tests/test_object_list_text_source.py"
run_check "Visible object list narrow-screen bounds" python3 "$ROOT/tests/test_object_list_compact_layout_source.py"
run_check "Visible object list UTF-8 name prefixes" python3 "$ROOT/tests/test_object_list_name_utf8_source.py"
run_check "Core item grammar UTF-8 bounds" python3 "$ROOT/tests/test_obj_desc_name_utf8_source.py"
run_check "Player item inscription UTF-8/closing brace" python3 "$ROOT/tests/test_obj_desc_inscription_utf8_source.py"
run_check "Artifact and ego suffix UTF-8 atomicity" python3 "$ROOT/tests/test_obj_desc_suffix_utf8_source.py"
run_check "Known scroll/flavor effect UTF-8 and punctuation" python3 "$ROOT/tests/test_obj_desc_effect_utf8_source.py"
run_check "Chest trap and money UI UTF-8 localization" python3 "$ROOT/tests/test_obj_desc_chest_money_utf8_source.py"
run_check "Atomic numeric item details and status" python3 "$ROOT/tests/test_obj_desc_numeric_utf8_source.py"
run_check "Inventory and equipment bilingual captions and selection invariants" python3 "$ROOT/tests/test_inventory_list_locale_source.py"
run_check "Equipment mention 14-column and UTF-8 safety" python3 "$ROOT/tests/test_inventory_equip_mention_source.py"
run_check "Rotated item-selection and compact shop prompts" python3 "$ROOT/tests/test_item_selection_rotation_source.py"
run_check "Knowledge glyph strict Unicode input and bilingual autoinscriptions" python3 "$ROOT/tests/test_knowledge_glyph_codepoint_locale_source.py"
run_check "Item picker switching, selection, cancellation clear stale soft keyboard" python3 "$ROOT/tests/test_item_picker_soft_key_exit_source.py"
run_check "FAangband item examine/ignore zh-TW labels" python3 "$ROOT/tests/test_faangband_object_ignore_locale_source.py"
run_check "FAangband knowledge category/Ego labels and nested ignore popup keys" python3 "$ROOT/tests/test_faangband_knowledge_ego_locale_source.py"
run_check "FAangband object/rune/terrain/trap knowledge display locale" python3 "$ROOT/tests/test_faangband_knowledge_detail_locale_source.py"
run_check "FAangband daily commands, store, input and options bilingual UI" python3 "$ROOT/tests/test_faangband_daily_ui_locale_source.py"
run_check "FAangband item commands bilingual dictionary C99" sh "$ROOT/scripts/test-fa-item-command-translation.sh"
run_check "FAangband item commands prompts and formatted message locale contracts" python3 "$ROOT/tests/test_faangband_item_command_messages_locale_source.py"
run_check "FAangband town, wilderness, stairs and trap event translations" python3 "$ROOT/tests/test_faangband_exploration_command_locale_source.py"
run_check "FAangband dungeon treasure and monster danger C99 zh-TW" sh "$ROOT/scripts/test-fa-dungeon-feelings-locale.sh"
run_check "FAangband dungeon feeling message and fallback source guard" python3 "$ROOT/tests/test_faangband_dungeon_feelings_locale_source.py"
run_check "FAangband option colours, visual menus and quality-ignore zh-TW" python3 "$ROOT/tests/test_faangband_options_visual_locale_source.py"
run_check "FAangband context menus, target help, maps and common commands zh-TW" python3 "$ROOT/tests/test_faangband_context_target_map_locale_source.py"
run_check "FAangband character sheet, spell browser and savefile picker zh-TW" python3 "$ROOT/tests/test_faangband_player_spell_save_locale_source.py"
run_check "FAangband death, ghost, birth and item-language source regression" python3 "$ROOT/tests/test_faangband_death_birth_locale_source.py"
run_check "FAangband world map, birth portrait UI and inscription confirmation" python3 "$ROOT/tests/test_faangband_birth_map_touch_locale_source.py"
run_check "FAangband character-history Unicode wrapping in host C99" sh "$ROOT/scripts/test-fa-birth-history-utf8.sh"
run_check "FAangband CJK birth-history scroll and map choice source guard" python3 "$ROOT/tests/test_faangband_birth_history_editor_source.py"
run_check "FAangband compact race/class details and bounded map chooser" python3 "$ROOT/tests/test_faangband_birth_compact_map_source.py"
run_check "FAangband race/class trait paging and touch controls" python3 "$ROOT/tests/test_faangband_birth_traits_paging_source.py"
run_check "FAangband race/class ability labels and portrait hints" python3 "$ROOT/tests/test_faangband_birth_ability_labels_source.py"
run_check "FAangband specialty browsers and shape knowledge display" python3 "$ROOT/tests/test_faangband_ability_browser_locale_source.py"
run_check "FAangband portrait ability browser C99 layout and description pages" sh "$ROOT/scripts/test-fa-ability-layout.sh"
run_check "FAangband native specialty mouse and selection dispatch" sh "$ROOT/scripts/test-fa-ability-action.sh"
run_check "FAangband native Android ability modal hint lifecycle" sh "$ROOT/scripts/test-fa-ability-hints.sh"
run_check "FAangband native UTF-8 Yes/No prompt and resize handling" sh "$ROOT/scripts/test-fa-confirm-check.sh"
run_check "FAangband generic get_char UTF-8 choices and resize handling" sh "$ROOT/scripts/test-fa-choice-input.sh"
run_check "FAangband UTF-8 command prompt redraw and resize cancellation" sh "$ROOT/scripts/test-fa-command-prompt.sh"
run_check "FAangband direction and targeting C99 input/resize" sh "$ROOT/scripts/test-fa-direction-target.sh"
run_check "FAangband direction and target help source regression" python3 "$ROOT/tests/test_faangband_direction_target_source.py"
run_check "FAangband compact targeting help and UTF-8 monster/object/terrain details" python3 "$ROOT/tests/test_faangband_target_detail_source.py"
run_check "FAangband target detail resize-safe native input" sh "$ROOT/scripts/test-fa-target-description-resize.sh"
run_check "FAangband target detail input and rotation source contract" python3 "$ROOT/tests/test_faangband_target_description_resize_source.py"
run_check "FAangband monster and object recall modal returns without double input" python3 "$ROOT/tests/test_faangband_target_modal_return_source.py"
run_check "FAangband long textblock and rotated floor list host C99" sh "$ROOT/scripts/test-fa-textblock-floor.sh"
run_check "FAangband pager and floor-list text/shortcut source guard" python3 "$ROOT/tests/test_faangband_textblock_floor_source.py"
run_check "FAangband ignored-item popup fits rotated Android terminal" python3 "$ROOT/tests/test_faangband_ignore_popup_geometry_source.py"
run_check "FAangband legacy C99 screen text wrap terminal bounds" sh "$ROOT/scripts/test-fa-text-out-screen-bounds.sh"
run_check "FAangband legacy text output geometry source contract" python3 "$ROOT/tests/test_faangband_text_out_screen_bounds_source.py"
run_check "FAangband native wide-cell overwrite and erase C99" sh "$ROOT/scripts/test-fa-terminal-wide-overwrite.sh"
run_check "FAangband native CJK partner-cell and map-safety source guard" python3 "$ROOT/tests/test_fa_terminal_wide_overwrite_source.py"
run_check "FAangband native CJK partial redraw row safety C99" sh "$ROOT/scripts/test-fa-redraw-section.sh"
run_check "FAangband partial redraw bounds and pending dirty source guard" python3 "$ROOT/tests/test_fa_redraw_section_source.py"
run_check "FAangband typed ability PF/OF labels with C99" sh "$ROOT/scripts/test-fa-birth-ability-labels.sh"
run_check "FAangband elemental resistance/vulnerability source contract" python3 "$ROOT/tests/test_faangband_birth_elements_source.py"
run_check "FAangband elemental resistance/weakness native C99" sh "$ROOT/scripts/test-fa-birth-elements.sh"
run_check "FAangband monster totals, footer keys and narrow CJK columns" python3 "$ROOT/tests/test_faangband_knowledge_monster_locale_source.py"
run_check "Clear stale item picker soft keys after invalid Android rotation" python3 "$ROOT/tests/test_item_picker_rotation_hints_source.py"
run_check "Empty bag quiver footer repaint and rotated bounds" python3 "$ROOT/tests/test_empty_quiver_footer_source.py"
run_check "Empty item picker can switch tabs without selecting a fake object" python3 "$ROOT/tests/test_empty_item_picker_source.py"
run_check "Prefer populated starting item tabs in Angband and FAangband" python3 "$ROOT/tests/test_item_picker_start_source.py"
run_check "Picker available tabs and FA bilingual Unicode prompt geometry" python3 "$ROOT/tests/test_item_picker_switch_source.py"
run_check "Item picker numeric quiver tag, localized cancel hint and CJK recall titles" python3 "$ROOT/tests/test_item_picker_tag_locale_source.py"
run_check "UI-only item name localization and export isolation" python3 "$ROOT/tests/test_visible_item_name_localization.py"
run_check "Known-item classifiers and translated artifact/ego grammar" python3 "$ROOT/tests/test_item_name_grammar_localization.py"
run_check "FAangband 38 shared artifact variant source-exact stories" python3 "$ROOT/tests/test_faangband_shared_artifact_lore_source.py"
run_check "FAangband 41 exclusive artifact lore stories and UI guard" python3 "$ROOT/tests/test_faangband_artifact_lore_source.py"
run_check "FAangband replaces Android soft keyboard hints across paging and rotation" python3 "$ROOT/tests/test_faangband_soft_kbd_replace_source.py"
run_check "FAangband filtered column repaint and CJK popup bounds" python3 "$ROOT/tests/test_faangband_menu_repaint_popup_source.py"
run_check "Angband and FA stock taps versus item picker single SELECT dispatch" python3 "$ROOT/tests/test_menu_mouse_dispatch_source.py"
run_check "Shopkeeper header touch, explicit Exit and mouse Back safety" python3 "$ROOT/tests/test_menu_header_back_source.py"
run_check "FAangband stock reorder, empty shop and purchase cancellation" python3 "$ROOT/tests/test_faangband_store_stock_lifecycle_source.py"
run_check "FAangband store touches and inventory cursor dispatch" python3 "$ROOT/tests/test_faangband_store_touch_source.py"
run_check "FAangband hidden/disabled relative shortcut visibility parity" python3 "$ROOT/tests/test_faangband_menu_visibility_source.py"
run_check "FAangband bounded keyboard direction and compact relative key tags" python3 "$ROOT/tests/test_faangband_menu_direction_source.py"
run_check "FAangband blank menu touches and rotated scroll/column viewport" python3 "$ROOT/tests/test_faangband_menu_touch_viewport_source.py"
run_check "FAangband filtered menu soft keyboard hints and source indices" python3 "$ROOT/tests/test_faangband_menu_key_guard_source.py"
run_check "FAangband generic menu titles and row texts use Unicode display cells" python3 "$ROOT/tests/test_faangband_menu_cell_text_source.py"
run_check "FAangband Unicode menu resize redraw and invalid rectangle safety" python3 "$ROOT/tests/test_faangband_menu_resize_source.py"
run_check "FAangband short object-list row budgets and immediate store repaint" python3 "$ROOT/tests/test_faangband_object_list_row_budget_source.py"
run_check "FAangband ground object distances and store rotation UI guard" python3 "$ROOT/tests/test_faangband_object_list_location_source.py"
run_check "FAangband dungeon item prefixes and store Unicode terminal columns" python3 "$ROOT/tests/test_faangband_dungeon_store_columns_source.py"
run_check "FAangband inventory CJK terminal columns and selection resize safety" python3 "$ROOT/tests/test_faangband_inventory_columns_source.py"
run_check "FAangband narrow complete-name fallback and chained modifier/status audit" python3 "$ROOT/tests/test_faangband_narrow_desc_chain_source.py"
run_check "FAangband narrow final descriptions, gold and known chest effects" python3 "$ROOT/tests/test_faangband_obj_basic_utf8_source.py"
run_check "FAangband equipment groups and translated status UTF-8 safety" python3 "$ROOT/tests/test_faangband_obj_details_utf8_source.py"
run_check "FAangband bounded object grammar and UTF-8 buffer safety" python3 "$ROOT/tests/test_faangband_obj_desc_name_utf8_source.py"
run_check "FAangband 155 flavors, scroll titles and unknown-item secrecy" python3 "$ROOT/tests/test_faangband_flavor_ui_source.py"
run_check "FAangband 359 item kinds and 118 ego UI label source guards" python3 "$ROOT/tests/test_faangband_item_ego_ui_source.py"
run_check "FAangband all 126 artifact names, safe UI and source coverage" python3 "$ROOT/tests/test_faangband_artifact_names_source.py"
run_check "FAangband supplemental kind names and known-effect nouns" python3 "$ROOT/tests/test_faangband_item_names_source.py"
run_check "FAangband additional spell names and exact descriptions" python3 "$ROOT/tests/test_faangband_spells_source.py"
run_check "FAangband divergent spell effect descriptions" python3 "$ROOT/tests/test_faangband_variant_spell_descriptions_source.py"
run_check "FAangband dragon and bear shape names and FLYING" python3 "$ROOT/tests/test_faangband_shapes_source.py"
run_check "FAangband shape knowledge, message recall and symbol-query bilingual UI" python3 "$ROOT/tests/test_faangband_knowledge_shape_message_symbol_source.py"
run_check "FAangband 64 race/class/specialty ability descriptions" python3 "$ROOT/tests/test_faangband_abilities_source.py"
run_check "FAangband six resistance-renewal timed messages" python3 "$ROOT/tests/test_faangband_timed_renewals_source.py"
run_check "FAangband projection and dragonfire/hellfire lash names" python3 "$ROOT/tests/test_faangband_projection_source.py"
run_check "FAangband wilderness terrain and cliff/lava notices" python3 "$ROOT/tests/test_faangband_terrain_source.py"
run_check "FAangband monster traps and bilingual activation notifications" python3 "$ROOT/tests/test_faangband_trap_source.py"
run_check "FAangband flying dragon and 7 spell lore/14 message translations" python3 "$ROOT/tests/test_faangband_monster_spells_source.py"
run_check "FAangband monster names and source-exact divergent lore" python3 "$ROOT/tests/test_faangband_monster_flavor_source.py"
run_check "Targeting visible-only monster names and status locale" python3 "$ROOT/tests/test_target_monster_locale_source.py"
run_check "Target-look terrain, visible trap and item-pile descriptions" python3 "$ROOT/tests/test_target_world_locale_source.py"
run_check "Angband/FAangband shape combat verbs" python3 "$ROOT/tests/test_shape_combat_verbs_source.py"
run_check "Class-book Chinese names and shape lore" python3 "$ROOT/tests/test_class_book_names_source.py"
run_check "Equipment combat and inscription UTF-8 safety" python3 "$ROOT/tests/test_item_details_localization.py"
run_check "Equipment comparison two-item detail zh-TW isolation" python3 "$ROOT/tests/test_equipment_comparison_detail_locale_source.py"
run_check "Equipment comparison screen-only CJK short names, stable sort/export" python3 "$ROOT/tests/test_equip_cmp_name_display_source.py"
run_check "Equipment compare tiny-terminal BAIL and label coordinate guards" python3 "$ROOT/tests/test_equip_cmp_tiny_terminal_source.py"
run_check "Equipment compare filtered touch selection, rotation and zh-TW hints" python3 "$ROOT/tests/test_equip_cmp_touch_selection_source.py"
run_check "Flavored noun grammar and awareness isolation" python3 "$ROOT/tests/test_flavored_object_localization.py"
run_check "Unmapped flavor zh-TW fallback and complete scroll titles" python3 "$ROOT/tests/test_visible_flavor_fallback_source.py"
run_check "Generated scroll title display and RNG isolation" python3 "$ROOT/tests/test_scroll_title_localization.py"
run_check "Item lore and character export isolation" python3 "$ROOT/tests/test_item_story_localization.py"
run_check "Ego knowledge names, clipping and recall titles" python3 "$ROOT/tests/test_ego_knowledge_screen.py"
run_check "Artifact knowledge title and recall UTF-8 bounds" python3 "$ROOT/tests/test_artifact_knowledge_display_source.py"
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
run_check "Native enchant, uncurse, recall and item-effect prompts in zh-TW" python3 "$ROOT/tests/test_general_effect_prompts_locale_source.py"
run_check "Object use-effect message translations and cancellation/charge rollback" python3 "$ROOT/tests/test_item_use_effect_messages_source.py"
run_check "Activation and artifact dynamic templates, name rendering and rollback" python3 "$ROOT/tests/test_activation_messages_locale_source.py"
run_check "Character panel byte-limit and UTF-8-safe CJK display" python3 "$ROOT/tests/test_character_panel_utf8_source.py"
run_check "Spell effect summary labels, radius, length, projection and UTF-8" python3 "$ROOT/tests/test_spell_effect_info_locale_source.py"
run_check "Mage spell detailed descriptions and exact original text" python3 "$ROOT/tests/test_spell_descriptions_locale_source.py"
run_check "Druid spell details and shared Ranger exception" python3 "$ROOT/tests/test_druid_spell_descriptions_locale_source.py"
run_check "Priest prayer descriptions and Paladin Heroism exception" python3 "$ROOT/tests/test_priest_spell_descriptions_locale_source.py"
run_check "Necromancer spells, self-damage, shapechanges and original matching" python3 "$ROOT/tests/test_necromancer_spell_descriptions_locale_source.py"
run_check "Paladin and Rogue original spells, Heroism thresholds and source matching" python3 "$ROOT/tests/test_paladin_rogue_spell_descriptions_locale_source.py"
run_check "Ranger spell descriptions, Druid Herbal Curing and restrictions" python3 "$ROOT/tests/test_ranger_spell_descriptions_locale_source.py"
run_check "Blackguard original spells and all 163 descriptions covered" python3 "$ROOT/tests/test_blackguard_spell_descriptions_locale_source.py"
run_check "Spell long descriptions CJK terminal wrapping source guards" python3 "$ROOT/tests/test_spell_screen_wrap_source.py"
run_check "Full spell description reader pagination and safe casting keys" python3 "$ROOT/tests/test_spell_reader_source.py"
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
run_check "Angband/FA multi-column direction and hidden-row navigation" python3 "$ROOT/tests/test_menu_column_navigation_source.py"
run_check "Rotated Angband scroll viewports and FA disabled menu actions" python3 "$ROOT/tests/test_menu_rotation_scroll_source.py"
run_check "Selected native menu action state" python3 "$ROOT/tests/test_menu_action_guard_source.py"
run_check "Native menu CJK column clipping" python3 "$ROOT/tests/test_menu_text_fit_source.py"
run_check "Compact menu layout on Android rotation" python3 "$ROOT/tests/test_menu_compact_layout_source.py"
run_check "Native blank-cell touch safety" python3 "$ROOT/tests/test_menu_hit_test_source.py"
run_check "Narrow native menu column safety" python3 "$ROOT/tests/test_menu_column_width_source.py"
run_check "Native checker Python syntax and parser integrity" python3 "$ROOT/tests/test_zh_tw_checker_integrity.py"
run_check "Current native localization display guards and negative mutations" python3 "$ROOT/tests/test_native_checker_current_display_contracts.py"
run_check "Native translated message branch scope (dangling else)" python3 "$ROOT/tests/test_native_locale_branch_braces.py"
run_check "Native projection terrain, knockback, and chest feedback zh-TW" python3 "$ROOT/tests/test_projection_terrain_feedback_locale_source.py"
run_check "Object projection damage, immunity and destruction zh-TW" python3 "$ROOT/tests/test_projection_item_feedback_locale_source.py"
run_check "Player recalculation, inventory, weapon and spell feedback zh-TW" python3 "$ROOT/tests/test_player_calcs_feedback_locale_source.py"
run_check "Native monster elemental attacks, life drain and theft zh-TW" python3 "$ROOT/tests/test_mon_blow_feedback_locale_source.py"
run_check "UTF-8 editor cursor/deletion and lossless default text" python3 "$ROOT/tests/test_ui_input_utf8_source.py"
run_check "Top-line Unicode message wrap safety" python3 "$ROOT/tests/test_message_wrap_utf8_source.py"
run_check "Monster list, item origin and dynamic UTF-8 display strings" python3 "$ROOT/tests/test_dynamic_names_utf8_source.py"
run_check "Item origin monster identity and UTF-8 label safety" python3 "$ROOT/tests/test_item_origin_monster_name_source.py"
run_check "Monster hallucination and sort hint localized UI" python3 "$ROOT/tests/test_monster_list_special_locale_source.py"
run_check "Monster list narrow-screen layout and row budget" python3 "$ROOT/tests/test_monster_list_compact_layout_source.py"
run_check "Visible monster/object list distances in zh-TW" python3 "$ROOT/tests/test_list_relative_location_locale_source.py"
run_check "Monster spell exact tags and UTF-8 message assembly" python3 "$ROOT/tests/test_mon_spell_render_utf8_source.py"
run_check "Monster action templates and saving throw zh-TW" python3 "$ROOT/tests/test_mon_blow_actions_locale_source.py"
run_check "Ranged out-of-range prompt and monster item-crush bilingual feedback" python3 "$ROOT/tests/test_combat_feedback_edge_locale_source.py"
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
