#!/bin/sh
# Check wide-character UTF-8 transport using the host C compiler.
# No Android SDK/NDK is required for this standalone validation.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM
CC="${CC:-cc}"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_utf8.c" \
    -o "$BUILD_DIR/test_unicode_utf8"

"$BUILD_DIR/test_unicode_utf8"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_width.c" \
    -o "$BUILD_DIR/test_unicode_width"

"$BUILD_DIR/test_unicode_width"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_layout.c" \
    -o "$BUILD_DIR/test_unicode_layout"

"$BUILD_DIR/test_unicode_layout"

# Streamed screen prose uses terminal columns (especially CJK wide glyphs).
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_screen_text_wrap.c" \
    -o "$BUILD_DIR/test_screen_text_wrap"
"$BUILD_DIR/test_screen_text_wrap"

# Top-line message wrap preserves Chinese glyphs and original words.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_message_wrap_utf8.c" \
    -o "$BUILD_DIR/test_message_wrap_utf8"
"$BUILD_DIR/test_message_wrap_utf8"

# Full-screen localized spell description pages; no JNI needed.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_spell_reader_pages.c" \
    -o "$BUILD_DIR/test_spell_reader_pages"
"$BUILD_DIR/test_spell_reader_pages"

# UTF-8 editor: deletion byte spans, display columns and safe prefills.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_ui_input_utf8.c" \
    -o "$BUILD_DIR/test_ui_input_utf8"
"$BUILD_DIR/test_ui_input_utf8"

# CJK display columns differ from Unicode scalar counts.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_display.c" \
    -o "$BUILD_DIR/test_unicode_display"

"$BUILD_DIR/test_unicode_display"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_death_locale.c" \
    -o "$BUILD_DIR/test_death_locale"

"$BUILD_DIR/test_death_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_locale.c" \
    -o "$BUILD_DIR/test_monster_locale"

"$BUILD_DIR/test_monster_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unique_monster_locale.c" \
    -o "$BUILD_DIR/test_unique_monster_locale"

"$BUILD_DIR/test_unique_monster_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_lore_locale.c" \
    -o "$BUILD_DIR/test_monster_lore_locale"
"$BUILD_DIR/test_monster_lore_locale"

# Named original Angband monster status/damage messages.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_messages_locale.c" \
    -o "$BUILD_DIR/test_monster_messages_locale"
"$BUILD_DIR/test_monster_messages_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_pain_locale.c" \
    -o "$BUILD_DIR/test_monster_pain_locale"
"$BUILD_DIR/test_monster_pain_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_item_locale.c" \
    -o "$BUILD_DIR/test_item_locale"
"$BUILD_DIR/test_item_locale"

# Known and unidentified display names may be clipped on small buffers.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_kind_utf8_copy.c" \
    -o "$BUILD_DIR/test_item_kind_utf8_copy"
"$BUILD_DIR/test_item_kind_utf8_copy"

# Identified ego suffixes are translated only in opted-in UI descriptions.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_ego_names_locale.c" \
    -o "$BUILD_DIR/test_ego_names_locale"
"$BUILD_DIR/test_ego_names_locale"

# Known artifact names retain canonical IDs and English fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_artifact_names_locale.c" \
    -o "$BUILD_DIR/test_artifact_names_locale"
"$BUILD_DIR/test_artifact_names_locale"

# Unknown item appearance labels; no canonical item kind change.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_item_flavor_locale.c" \
    -o "$BUILD_DIR/test_item_flavor_locale"
"$BUILD_DIR/test_item_flavor_locale"

# Combat labels and modifiers: exact numeric values and bounded UTF-8 text.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_details_locale.c" \
    -o "$BUILD_DIR/test_item_details_locale"
"$BUILD_DIR/test_item_details_locale"

# Chinese classifiers and known artifact/ego suffix punctuation.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_name_grammar_locale.c" \
    -o "$BUILD_DIR/test_item_name_grammar_locale"
"$BUILD_DIR/test_item_name_grammar_locale"

# Unknown/custom appearances still get Chinese nouns without revealing effects.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_visible_flavor_fallback_locale.c" \
    -o "$BUILD_DIR/test_visible_flavor_fallback_locale"
"$BUILD_DIR/test_visible_flavor_fallback_locale"

# Twenty class-book titles from both variants' class.txt files.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_class_book_names_locale.c" \
    -o "$BUILD_DIR/test_class_book_names_locale"
"$BUILD_DIR/test_class_book_names_locale"

# FAangband-specific descriptions for two shared-name spells.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_variant_spell_descriptions.c" \
    -o "$BUILD_DIR/test_faangband_variant_spell_descriptions"
"$BUILD_DIR/test_faangband_variant_spell_descriptions"

# Exact FAangband-only spell name and description display fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_spells_locale.c" \
    -o "$BUILD_DIR/test_faangband_spells_locale"
"$BUILD_DIR/test_faangband_spells_locale"

# Exact FAangband-only item kind additions (screen-only fallback).
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_item_names_locale.c" \
    -o "$BUILD_DIR/test_faangband_item_names_locale"
"$BUILD_DIR/test_faangband_item_names_locale"

# Seven flavored noun phrases: count, identification and UTF-8 safety.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_flavored_object_locale.c" \
    -o "$BUILD_DIR/test_flavored_object_locale"
"$BUILD_DIR/test_flavored_object_locale"

# Generated scroll titles: preserve original identifiers and safe CJK boundaries.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_scroll_title_locale.c" \
    -o "$BUILD_DIR/test_scroll_title_locale"
"$BUILD_DIR/test_scroll_title_locale"

# Known artifact and ego story text, including untranslated fallbacks.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_item_story_locale.c" \
    -o "$BUILD_DIR/test_item_story_locale"
"$BUILD_DIR/test_item_story_locale"

# Identified object-kind lore preserves English fallback and original IDs.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_kind_lore_locale.c" \
    -o "$BUILD_DIR/test_kind_lore_locale"
"$BUILD_DIR/test_kind_lore_locale"

"$BUILD_DIR/test_item_locale"

# Native UI translations: compile without Android SDK or the game engine.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_game_ui_locale.c" \
    -o "$BUILD_DIR/test_game_ui_locale"
"$BUILD_DIR/test_game_ui_locale"

# Shared Angband confirmation and get_char prompt UTF-8 clipping on phones.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_gear_confirmation_locale.c" \
    -o "$BUILD_DIR/test_gear_confirmation_locale"
"$BUILD_DIR/test_gear_confirmation_locale"

# Original Angband item-use pickers: scroll, food, potion, generic item, fuel.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_use_locale.c" \
    -o "$BUILD_DIR/test_item_use_locale"
"$BUILD_DIR/test_item_use_locale"

# Real object.txt item-use messages: English, zh-TW and unknown fallbacks.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_use_effect_messages.c" \
    -o "$BUILD_DIR/test_item_use_effect_messages"
"$BUILD_DIR/test_item_use_effect_messages"

# Artifact and activation dynamic messages use exact data-driven zh-TW templates.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_activation_message_locale.c" \
    -o "$BUILD_DIR/test_activation_message_locale"
"$BUILD_DIR/test_activation_message_locale"

# Display-only spell effect labels, radius, length and unknown fallbacks.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_spell_effect_info_locale.c" \
    -o "$BUILD_DIR/test_spell_effect_info_locale"
"$BUILD_DIR/test_spell_effect_info_locale"

# Original Mage spell descriptions: exact canonical text, safe bilingual fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_spell_descriptions_locale"
"$BUILD_DIR/test_spell_descriptions_locale"

# Druid spells, including the different Ranger Herbal Curing description.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_druid_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_druid_spell_descriptions_locale"
"$BUILD_DIR/test_druid_spell_descriptions_locale"

# Priest prayer descriptions, including the Paladin Heroism level difference.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_priest_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_priest_spell_descriptions_locale"
"$BUILD_DIR/test_priest_spell_descriptions_locale"

# Necromancer spells: exact-source translation and dangerous side effects.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_necromancer_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_necromancer_spell_descriptions_locale"
"$BUILD_DIR/test_necromancer_spell_descriptions_locale"

# Paladin (16) / Rogue (10) original descriptions and the dual Heroism entries.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_paladin_rogue_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_paladin_rogue_spell_descriptions_locale"
"$BUILD_DIR/test_paladin_rogue_spell_descriptions_locale"

# Ranger original spell descriptions and distinct Druid/Ranger Herbal Curing.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_ranger_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_ranger_spell_descriptions_locale"
"$BUILD_DIR/test_ranger_spell_descriptions_locale"

# Blackguard: all original spell descriptions, dangerous penalties and source fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_blackguard_spell_descriptions_locale.c" \
    -o "$BUILD_DIR/test_blackguard_spell_descriptions_locale"
"$BUILD_DIR/test_blackguard_spell_descriptions_locale"

# Knowledge categories are display-only and must measure UTF-8 glyph cells.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_knowledge_groups_locale.c" \
    -o "$BUILD_DIR/test_knowledge_groups_locale"
"$BUILD_DIR/test_knowledge_groups_locale"

# Monster list-count labels keep [U]/counts and complete CJK glyphs.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_monster_list_render_utf8.c" \
    -o "$BUILD_DIR/test_monster_list_render_utf8"
"$BUILD_DIR/test_monster_list_render_utf8"

# Localized hallucination warning and unchanged 'x' sort shortcut.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_monster_list_special_locale.c" \
    -o "$BUILD_DIR/test_monster_list_special_locale"
"$BUILD_DIR/test_monster_list_special_locale"

# Narrow-screen monster list geometry, optional distance and sleep tags.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_monster_list_compact_layout.c" \
    -o "$BUILD_DIR/test_monster_list_compact_layout"
"$BUILD_DIR/test_monster_list_compact_layout"

# Monster knowledge labels reuse canonical non-unique names and clip CJK glyphs.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_knowledge_monster_label.c" \
    -o "$BUILD_DIR/test_knowledge_monster_label"
"$BUILD_DIR/test_knowledge_monster_label"

# Fixed-column knowledge rows: clip whole CJK glyphs and retain fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_knowledge_column_text.c" \
    -o "$BUILD_DIR/test_knowledge_column_text"
"$BUILD_DIR/test_knowledge_column_text"

# Refuse lossy edits of existing object/rune notes before askfor_aux.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_knowledge_note_prefill.c" \
    -o "$BUILD_DIR/test_knowledge_note_prefill"
"$BUILD_DIR/test_knowledge_note_prefill"

# Glyph editor strict Unicode input and bilingual rune/object inscriptions.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_knowledge_glyph_codepoint.c" \
    -o "$BUILD_DIR/test_knowledge_glyph_codepoint"
"$BUILD_DIR/test_knowledge_glyph_codepoint"

# Native rune detail names/grammar are presentation-only; guard NDK fmt.
"$CC" -std=c99 -Wall -Wextra -Werror -Wformat -Werror=format-security -pedantic \
    "$ROOT/tests/native/test_rune_lore_locale.c" \
    -o "$BUILD_DIR/test_rune_lore_locale"
"$BUILD_DIR/test_rune_lore_locale"

# All terrain and trap knowledge names/lore: source-matched display-only locale.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_terrain_trap_locale.c" \
    -o "$BUILD_DIR/test_terrain_trap_locale"
"$BUILD_DIR/test_terrain_trap_locale"

# FAangband-only monster trap knowledge and all canonical trap notifications.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_trap_locale.c" \
    -o "$BUILD_DIR/test_faangband_trap_locale"
"$BUILD_DIR/test_faangband_trap_locale"

# FAangband wilderness terrain names, lore and cliff/lava prompts.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_terrain_locale.c" \
    -o "$BUILD_DIR/test_faangband_terrain_locale"
"$BUILD_DIR/test_faangband_terrain_locale"

# Symbol lookup is bilingual for all canonical tval classes and known terrain.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_symbol_query_locale.c" \
    -o "$BUILD_DIR/test_symbol_query_locale"
"$BUILD_DIR/test_symbol_query_locale"

# All original monster base symbol descriptions; English and unknown fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_symbol_monster_base_locale.c" \
    -o "$BUILD_DIR/test_symbol_monster_base_locale"
"$BUILD_DIR/test_symbol_monster_base_locale"

# Knowledge action names and store labels: verify locale and exact shortcuts.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_knowledge_store_menu_locale.c" \
    -o "$BUILD_DIR/test_knowledge_store_menu_locale"
"$BUILD_DIR/test_knowledge_store_menu_locale"

# Native generic spell formatted messages: canonical stat/brand/device terms.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_general_effect_terms_locale.c" \
    -o "$BUILD_DIR/test_general_effect_terms_locale"
"$BUILD_DIR/test_general_effect_terms_locale"

# Native generic effect messages: magic detection, teleport and recovery.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_general_effect_messages_locale.c" \
    -o "$BUILD_DIR/test_general_effect_messages_locale"
"$BUILD_DIR/test_general_effect_messages_locale"

# Player warnings, death notices, shape prompts and misc commands.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_player_feedback_locale.c" \
    -o "$BUILD_DIR/test_player_feedback_locale"
"$BUILD_DIR/test_player_feedback_locale"

# Player-calcs: inventory rearrangement, weapon/armor statuses.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_player_calcs_feedback_locale.c" \
    -o "$BUILD_DIR/test_player_calcs_feedback_locale"
"$BUILD_DIR/test_player_calcs_feedback_locale"

# Monster spell rendering: exact tags, UTF-8 fragments, English fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_mon_spell_render_utf8.c" \
    -o "$BUILD_DIR/test_mon_spell_render_utf8"
"$BUILD_DIR/test_mon_spell_render_utf8"

# Remaining 77 FAangband exclusive monsters and 76 complete source descriptions.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_monster_flavor_remaining.c" \
    -o "$BUILD_DIR/test_faangband_monster_flavor_remaining"
"$BUILD_DIR/test_faangband_monster_flavor_remaining"

# FAangband first 15 exclusive monster names and eight divergent descriptions.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_monster_flavor.c" \
    -o "$BUILD_DIR/test_faangband_monster_flavor"
"$BUILD_DIR/test_faangband_monster_flavor"

# FAangband extra monster base, lore and spell messages.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_monster_spells.c" \
    -o "$BUILD_DIR/test_faangband_monster_spells"
"$BUILD_DIR/test_faangband_monster_spells"

# All canonical monster spell messages (and original race overrides).
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_mon_spell_message_locale.c" \
    -o "$BUILD_DIR/test_mon_spell_message_locale"
"$BUILD_DIR/test_mon_spell_message_locale"

# Original attack effect handler and all 55 projection.txt display terms.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_attack_feedback_locale.c" \
    -o "$BUILD_DIR/test_attack_feedback_locale"
"$BUILD_DIR/test_attack_feedback_locale"

# FAangband projection spell summaries and monster lash descriptions.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_projection_locale.c" \
    -o "$BUILD_DIR/test_faangband_projection_locale"
"$BUILD_DIR/test_faangband_projection_locale"

# Projection feedback and monster direct attack messages.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_projection_feedback_locale.c" \
    -o "$BUILD_DIR/test_projection_feedback_locale"
"$BUILD_DIR/test_projection_feedback_locale"

# Monster melee damage, elemental effects, theft and pack events.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_mon_blow_feedback_locale.c" \
    -o "$BUILD_DIR/test_mon_blow_feedback_locale"
"$BUILD_DIR/test_mon_blow_feedback_locale"

# Original monster action templates and resistance saving-throw messages.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_mon_blow_actions_locale.c" \
    -o "$BUILD_DIR/test_mon_blow_actions_locale"
"$BUILD_DIR/test_mon_blow_actions_locale"

# Monster mimic, theft and spawning feedback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_monster_events_locale.c" \
    -o "$BUILD_DIR/test_monster_events_locale"
"$BUILD_DIR/test_monster_events_locale"

# Original monster movement, terrain and victory events.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_move_quest_locale.c" \
    -o "$BUILD_DIR/test_move_quest_locale"
"$BUILD_DIR/test_move_quest_locale"

# Dynamic shape-change effects: source-matched display-only status/projection.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_shape_effect_locale.c" \
    -o "$BUILD_DIR/test_shape_effect_locale"
"$BUILD_DIR/test_shape_effect_locale"

# Runtime timed-condition notification messages, source matched and bilingual.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_timed_messages_locale.c" \
    -o "$BUILD_DIR/test_timed_messages_locale"
"$BUILD_DIR/test_timed_messages_locale"

# FAangband-specific on-increase messages for temporary resistances.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_timed_renewals.c" \
    -o "$BUILD_DIR/test_faangband_timed_renewals"
"$BUILD_DIR/test_faangband_timed_renewals"

# FAangband-only 14 shape names and FLYING flag, standalone C99.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_shapes_locale.c" \
    -o "$BUILD_DIR/test_faangband_shapes_locale"
"$BUILD_DIR/test_faangband_shapes_locale"

# Player shapechange attacks: all 14 Angband/FAangband blow verbs.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_shape_combat_verbs.c" \
    -o "$BUILD_DIR/test_shape_combat_verbs"
"$BUILD_DIR/test_shape_combat_verbs"

# FAangband-only ability names and source-matched descriptions.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_faangband_abilities_locale.c" \
    -o "$BUILD_DIR/test_faangband_abilities_locale"
"$BUILD_DIR/test_faangband_abilities_locale"

# Shape knowledge: bilingual display names and restriction text, C99.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_shape_lore_locale.c" \
    -o "$BUILD_DIR/test_shape_lore_locale"
"$BUILD_DIR/test_shape_lore_locale"

# Inventory and equipment feedback (status messages, both locales).
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_gear_messages_locale.c" \
    -o "$BUILD_DIR/test_gear_messages_locale"
"$BUILD_DIR/test_gear_messages_locale"

# Character sheet labels, ratings, prompts and original locale fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_character_sheet_locale.c" \
    -o "$BUILD_DIR/test_character_sheet_locale"
"$BUILD_DIR/test_character_sheet_locale"

# Native history and Hall of Fame: record labels, original data keys and fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_history_score_locale.c" \
    -o "$BUILD_DIR/test_history_score_locale"
"$BUILD_DIR/test_history_score_locale"

# Original Angband equipment comparison: display instructions and menu labels.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_equip_cmp_locale.c" \
    -o "$BUILD_DIR/test_equip_cmp_locale"
"$BUILD_DIR/test_equip_cmp_locale"

# Equipment comparison on-screen CJK row labels: suffixes and cell widths.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_equip_cmp_name_display.c" \
    -o "$BUILD_DIR/test_equip_cmp_name_display"
"$BUILD_DIR/test_equip_cmp_name_display"

# Equipment compare BAIL state and tiny/rotated terminal coordinate guards.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_equip_cmp_tiny_terminal.c" \
    -o "$BUILD_DIR/test_equip_cmp_tiny_terminal"
"$BUILD_DIR/test_equip_cmp_tiny_terminal"

# Equipment comparison: touch mapping, empty lists and resized selection.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_equip_cmp_touch_guard.c" \
    -o "$BUILD_DIR/test_equip_cmp_touch_guard"
"$BUILD_DIR/test_equip_cmp_touch_guard"

# Original Angband curse.txt labels and exact display-only translations.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_curse_locale.c" \
    -o "$BUILD_DIR/test_curse_locale"
"$BUILD_DIR/test_curse_locale"

# Player-property abilities: exact source-matched name/description locale.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_player_ability_locale.c" \
    -o "$BUILD_DIR/test_player_ability_locale"
"$BUILD_DIR/test_player_ability_locale"

# Monster targeting and look prompts: visible-only names and status.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_target_monster_locale.c" \
    -o "$BUILD_DIR/test_target_monster_locale"
"$BUILD_DIR/test_target_monster_locale"

# Target selection help only localizes descriptive fragments, not key bindings.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_target_help_locale.c" \
    -o "$BUILD_DIR/test_target_help_locale"
"$BUILD_DIR/test_target_help_locale"

# Shop/home localized help and purchase/sale prompt text.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_store_text_locale.c" \
    -o "$BUILD_DIR/test_store_text_locale"
"$BUILD_DIR/test_store_text_locale"

# Exact canonical shop titles and !key / !* confirmation prefixes.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_store_inscription_labels.c" \
    -o "$BUILD_DIR/test_store_inscription_labels"
"$BUILD_DIR/test_store_inscription_labels"

# Shopkeeper greetings preserve %s formatting and English fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_store_greetings_locale.c" \
    -o "$BUILD_DIR/test_store_greetings_locale"
"$BUILD_DIR/test_store_greetings_locale"

# Curated canonical Angband shopkeeper hints (English fallback preserved).
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_store_hints_locale.c" \
    -o "$BUILD_DIR/test_store_hints_locale"
"$BUILD_DIR/test_store_hints_locale"

# Native help-browser menu captions and prompt labels.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_help_browser_locale.c" \
    -o "$BUILD_DIR/test_help_browser_locale"
"$BUILD_DIR/test_help_browser_locale"

# Object information property and brand/slay labels.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic "$ROOT/tests/native/test_object_info_locale.c" -o "$BUILD_DIR/test_object_info_locale"
"$BUILD_DIR/test_object_info_locale"

# All source class.txt spell names, display-only C99 lookup and English fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic "$ROOT/tests/native/test_spell_names_locale.c" -o "$BUILD_DIR/test_spell_names_locale"
"$BUILD_DIR/test_spell_names_locale"

# Original Angband class progression titles; game records unchanged.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic "$ROOT/tests/native/test_class_titles_locale.c" -o "$BUILD_DIR/test_class_titles_locale"
"$BUILD_DIR/test_class_titles_locale"

# Filtered menu soft-keyboard hints must use visible-row indexes and remain bounded.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_key_hints.c" \
    -o "$BUILD_DIR/test_menu_key_hints"
"$BUILD_DIR/test_menu_key_hints"

# Scrolling relative-tag input and Android keyboard hints must use the visible page.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_visible_tags.c" \
    -o "$BUILD_DIR/test_menu_visible_tags"
"$BUILD_DIR/test_menu_visible_tags"

# Horizontal movement across invalid menu rows must terminate.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_valid_cursor.c" \
    -o "$BUILD_DIR/test_menu_valid_cursor"
"$BUILD_DIR/test_menu_valid_cursor"

# Blank-cell taps must not select the last menu item.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_hit_test.c" \
    -o "$BUILD_DIR/test_menu_hit_test"
"$BUILD_DIR/test_menu_hit_test"

# Screen rotation and narrow terminals must preserve at least one menu row.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_compact_layout.c" \
    -o "$BUILD_DIR/test_menu_compact_layout"
"$BUILD_DIR/test_menu_compact_layout"

# Chinese menu labels must clip by terminal cells, not decoded codepoints.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_text_fit.c" \
    -o "$BUILD_DIR/test_menu_text_fit"
"$BUILD_DIR/test_menu_text_fit"

# Verify selected menu action disabled state and filtered cursor bounds.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_action_guard.c" \
    -o "$BUILD_DIR/test_menu_action_guard"
"$BUILD_DIR/test_menu_action_guard"

# Stock context menus must track scrolling and work when the shop is empty.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_store_menu_flow.c" \
    -o "$BUILD_DIR/test_store_menu_flow"
"$BUILD_DIR/test_store_menu_flow"

# Visible object list must keep its popup in bounds and prioritize item names.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_object_list_compact_layout.c" \
    -o "$BUILD_DIR/test_object_list_compact_layout"
"$BUILD_DIR/test_object_list_compact_layout"

# Numeric, charging and modifier details must never leave half punctuation.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security     "$ROOT/tests/native/test_obj_desc_numeric_utf8.c"     -o "$BUILD_DIR/test_obj_desc_numeric_utf8"
"$BUILD_DIR/test_obj_desc_numeric_utf8"

# Known chest trap labels and money-kind descriptions are UTF-8 complete.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_obj_desc_chest_money_utf8.c" \
    -o "$BUILD_DIR/test_obj_desc_chest_money_utf8"
"$BUILD_DIR/test_obj_desc_chest_money_utf8"

# Scroll effect parentheses and fallback 'of' names must stay complete.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_obj_desc_effect_utf8.c" \
    -o "$BUILD_DIR/test_obj_desc_effect_utf8"
"$BUILD_DIR/test_obj_desc_effect_utf8"

# Artifact and ego suffixes must be added as complete UTF-8 units.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_obj_desc_suffix_utf8.c" \
    -o "$BUILD_DIR/test_obj_desc_suffix_utf8"
"$BUILD_DIR/test_obj_desc_suffix_utf8"

# Visible inscriptions: complete player UTF-8, atomic status tags and brace.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_obj_desc_inscription_utf8.c" \
    -o "$BUILD_DIR/test_obj_desc_inscription_utf8"
"$BUILD_DIR/test_obj_desc_inscription_utf8"

# Core object description grammar must stay within buffers on CJK text.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_obj_desc_name_utf8.c" \
    -o "$BUILD_DIR/test_obj_desc_name_utf8"
"$BUILD_DIR/test_obj_desc_name_utf8"

# Object list article/count prefixes and CJK names must remain complete.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_object_list_name_utf8.c" \
    -o "$BUILD_DIR/test_object_list_name_utf8"
"$BUILD_DIR/test_object_list_name_utf8"

# Inventory, equipment and quiver text must not be truncated mid-UTF8.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_object_list_text.c" \
    -o "$BUILD_DIR/test_object_list_text"
"$BUILD_DIR/test_object_list_text"

# Inventory numeric columns, burden and quiver headings in both locales.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_inventory_list_display.c" \
    -o "$BUILD_DIR/test_inventory_list_display"
"$BUILD_DIR/test_inventory_list_display"

# Dynamic quality-ignore labels and screen-export prompts: both locales.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_ignore_quality_action_locale.c" \
    -o "$BUILD_DIR/test_ignore_quality_action_locale"
"$BUILD_DIR/test_ignore_quality_action_locale"

# Native bilingual version title and complete UTF-8 buffer boundary.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_version_header_text.c" \
    -o "$BUILD_DIR/test_version_header_text"
"$BUILD_DIR/test_version_header_text"

# Rotated item selection and scroll-safe quiver footer coordinates.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_selection_layout.c" \
    -o "$BUILD_DIR/test_item_selection_layout"
"$BUILD_DIR/test_item_selection_layout"

# Original item picker quiver @0..@9 bounds and zh-TW Esc cancellation hint.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_picker_tag_guard.c" \
    -o "$BUILD_DIR/test_item_picker_tag_guard"
"$BUILD_DIR/test_item_picker_tag_guard"

# Item picker English/Traditional Chinese Esc prompt is display-only.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_picker_cancel_locale.c" \
    -o "$BUILD_DIR/test_item_picker_cancel_locale"
"$BUILD_DIR/test_item_picker_cancel_locale"

# Store columns must remain safe when a phone's terminal is narrower than
# the optional price/weight/gold fields.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_store_column_layout.c" \
    -o "$BUILD_DIR/test_store_column_layout"
"$BUILD_DIR/test_store_column_layout"

# Store bilingual price/weight text must not truncate real numeric values.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_store_detail_text.c" \
    -o "$BUILD_DIR/test_store_detail_text"
"$BUILD_DIR/test_store_detail_text"

# A cancelled purchase cannot select a stale or out-of-range stock item.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_store_stock_guard.c" \
    -o "$BUILD_DIR/test_store_stock_guard"
"$BUILD_DIR/test_store_stock_guard"

# Confirmation must never accept a touch or an accidental Enter.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_store_confirmation_text.c" \
    -o "$BUILD_DIR/test_store_confirmation_text"
"$BUILD_DIR/test_store_confirmation_text"

# Generic quantity parser rejects accidental letter aliases and overflow.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_quantity_input.c" \
    -o "$BUILD_DIR/test_quantity_input"
"$BUILD_DIR/test_quantity_input"

# Dynamic popup keys such as shop g must choose a tagged action, never
# accidentally execute whichever entry currently has focus.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_dynamic_choice.c" \
    -o "$BUILD_DIR/test_menu_dynamic_choice"
"$BUILD_DIR/test_menu_dynamic_choice"

# Dynamic store popups stay inside the terminal during rotation and on
# very narrow portrait displays, with scrolling for long menus.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_popup_layout.c" \
    -o "$BUILD_DIR/test_menu_popup_layout"
"$BUILD_DIR/test_menu_popup_layout"

# Right-anchored item context menus and quantity retry state, without Android.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic -Wformat -Werror=format-security \
    "$ROOT/tests/native/test_item_context_quantity.c" \
    -o "$BUILD_DIR/test_item_context_quantity"
"$BUILD_DIR/test_item_context_quantity"

# Multi-column menu layout on tiny terminal widths must not divide by zero.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_column_width.c" \
    -o "$BUILD_DIR/test_menu_column_width"
"$BUILD_DIR/test_menu_column_width"
