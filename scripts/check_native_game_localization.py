#!/usr/bin/env python3
"""Static safeguards for display-only native Angband zh-TW localization.

Does not compile C, verify zipangband data, or run an emulator. This only
checks that locale selection and display-only dictionary wiring stay intact.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"


def source(path, errors):
    try:
        return path.read_text(encoding="utf-8")
    except OSError as exc:
        errors.append(f"Missing source {path}: {exc}")
        return ""


def table(source_text):
    return re.findall(r'\{ "([^"]+)", "([^"]+)" \}', source_text)


def main():
    errors = []
    checks = 0

    def expect(condition, label):
        nonlocal checks
        checks += 1
        if not condition:
            errors.append(label)

    java = source(JAVA / "NativeWrapper.java", errors)
    locale_java = source(JAVA / "GameTextLocale.java", errors)
    native = source(CPP / "curses/curses.c", errors)
    droid = source(CPP / "curses/droid.h", errors)
    death = source(CPP / "angband/ui-death.c", errors)
    death_header = source(CPP / "angband/zh-tw-death.h", errors)
    mon = source(CPP / "angband/mon-desc.c", errors)
    mon_header = source(CPP / "angband/zh-tw-monsters.h", errors)
    unique_mon_header = source(CPP / "angband/zh-tw-unique-monsters.h", errors)
    obj = source(CPP / "angband/obj-desc.c", errors)
    obj_header = source(CPP / "angband/zh-tw-items.h", errors)
    monster_data = source(ROOT / "variant_lib/angband/gamedata/monster.txt", errors)
    item_data = source(ROOT / "variant_lib/angband/gamedata/object.txt", errors)
    display_width = source(CPP / "curses/unicode_display.h", errors)
    monster_list = source(CPP / "angband/ui-mon-list.c", errors)
    object_list = source(CPP / "angband/ui-obj-list.c", errors)
    native_tests = source(ROOT / "scripts/test-native-utf8.sh", errors)
    game_ui_header = source(CPP / "angband/zh-tw-ui.h", errors)
    birth_ui = source(CPP / "angband/ui-birth.c", errors)
    player_ui = source(CPP / "angband/ui-player.c", errors)
    class_titles = source(CPP / "angband/zh-tw-class-titles.h", errors)
    class_titles_data = source(ROOT / "variant_lib/angband/gamedata/class.txt", errors)
    menu_ui = source(CPP / "angband/ui-menu.c", errors)
    context_ui = source(CPP / "angband/ui-context.c", errors)
    game_ui = source(CPP / "angband/ui-game.c", errors)
    obj_cmd = source(CPP / "angband/cmd-obj.c", errors)
    obj_info = source(CPP / "angband/obj-info.c", errors)
    item_info_terms = source(CPP / "angband/zh-tw-item-info.h", errors)
    spell_ui = source(CPP / "angband/ui-spell.c", errors)
    spell_names = source(CPP / "angband/zh-tw-spell-names.h", errors)
    class_data = source(ROOT / "variant_lib/angband/gamedata/class.txt", errors)
    magic_terms = source(CPP / "angband/zh-tw-magic.h", errors)
    player_spell = source(CPP / "angband/player-spell.c", errors)
    cave_cmd = source(CPP / "angband/cmd-cave.c", errors)
    player_attack = source(CPP / "angband/player-attack.c", errors)
    options_ui = source(CPP / "angband/ui-options.c", errors)
    object_info = source(CPP / "angband/obj-info.c", errors)
    object_info_terms = source(CPP / "angband/zh-tw-object-info.h", errors)
    object_ui = source(CPP / "angband/ui-object.c", errors)
    target_ui = source(CPP / "angband/ui-target.c", errors)
    combat_header = source(CPP / "angband/zh-tw-combat.h", errors)
    monster_lore = source(CPP / "angband/mon-lore.c", errors)
    monster_lore_ui = source(CPP / "angband/ui-mon-lore.c", errors)
    monster_lore_dict = source(CPP / "angband/zh-tw-lore.h", errors)
    lore_terms = source(CPP / "angband/zh-tw-mon-lore-terms.h", errors)
    blow_methods_data = source(ROOT / "variant_lib/angband/gamedata/blow_methods.txt", errors)
    blow_effects_data = source(ROOT / "variant_lib/angband/gamedata/blow_effects.txt", errors)
    monster_spell_data = source(ROOT / "variant_lib/angband/gamedata/monster_spell.txt", errors)
    store_ui = source(CPP / "angband/ui-store.c", errors)
    knowledge_ui = source(CPP / "angband/ui-knowledge.c", errors)
    input_ui = source(CPP / "angband/ui-input.c", errors)
    java_tests = source(ROOT / "scripts/test-terminal-unicode.sh", errors)

    expect("usesTraditionalChineseGameText()" in java,
           "NativeWrapper does not expose native game locale")
    expect('JAVA_METHOD("usesTraditionalChineseGameText", "()Z")' in native,
           "Native JNI method signature is missing")
    expect("game_text_zh_tw = enabled == JNI_TRUE" in native,
           "Native JNI does not cache the locale choice")
    expect("angdroid_zh_tw_enabled(void)" in droid,
           "Native game lacks locale declaration")
    expect('"zh".equalsIgnoreCase' in locale_java and
           '"Hant".equalsIgnoreCase(script)' in locale_java,
           "Chinese script and language selection changed")
    expect("GameTextLocaleTest" in java_tests,
           "Java locale fallback regression not registered")

    expect('ANGDROID_DEATH_MENU_ITEMS 9' in death_header,
           "Death menu label count changed")
    expect('ANGDROID_DEATH_PROMPT_COUNT' in death_header,
           "Death prompt enum changed")
    expect('angdroid_death_menu_label(' in death,
           "Death menu display labels not localized")
    expect('angdroid_death_prompt(' in death,
           "Death menu prompts not localized")
    expect("death_actions[i].name =" in death,
           "Death menu labels not connected to selection widget")

    for cfile, header, macro, target, count in (
        (mon, mon_header, "ANGDROID_ZH_TW_MONSTER_NAMES",
         "angdroid_zh_tw_monster_list_name(race->name)", 522),
        (obj, obj_header, "ANGDROID_ZH_TW_ITEM_KINDS",
         "angdroid_zh_tw_item_kind_name(kind->name)", 326),
    ):
        mapping = table(header)
        match = re.search(r"#define\s+" + macro + r"\s+(\d+)", header)
        expect(bool(match) and int(match.group(1)) == count,
               f"Wrong dictionary size: {macro}")
        expect(len(mapping) == count,
               f"Wrong number of dictionary entries: {macro}")
        expect(len({name for name, _ in mapping}) == len(mapping),
               f"Duplicate English source name: {macro}")
        expect(all(en.strip() and zh.strip() for en, zh in mapping),
               f"Empty translation: {macro}")
        expect(target in cfile,
               f"Dictionary is not wired to the display-only helper: {macro}")
    # Verify source-name keys against the exact canonical game-data grammar.
    # e.g. "& Main~ Gauche~" differs from "& Main Gauche~".
    for label, mapping, original_data in (
        ("monster", table(mon_header), monster_data),
        ("item", table(obj_header), item_data),
        ("lore", table(monster_lore_dict), monster_data),
    ):
        originals = {
            line.split(":", 1)[1]
            for line in original_data.splitlines() if line.startswith("name:")
        }
        missing = [en for en, _ in mapping if en not in originals]
        expect(not missing,
               "Localized " + label + " lookup keys not in canonical data: "
               + ", ".join(missing[:15]))
    # The four virtual kind markers are not real identified item names.
    real_item_names = {
        line.split(":", 1)[1]
        for line in item_data.splitlines()
        if line.startswith("name:") and
        not line.split(":", 1)[1].startswith("<")
    }
    item_translated_names = {name for name, _ in table(obj_header)}
    expect(real_item_names == item_translated_names,
           "Original Angband item-kind catalog incomplete: "
           + ", ".join(sorted(real_item_names - item_translated_names)[:15]))
    # Every non-unique source monster (excluding the <player> placeholder)
    # must have a display-only Chinese label. Unique monster proper names
    # deliberately retain their canonical name and purple rendering.
    monster_records = []
    current_name = None
    current_unique = False
    for line in monster_data.splitlines():
        if line.startswith("name:"):
            if current_name is not None:
                monster_records.append((current_name, current_unique))
            current_name = line.split(":", 1)[1]
            current_unique = False
        elif current_name is not None and line.startswith("flags:"):
            if re.search(r"\bUNIQUE\b", line):
                current_unique = True
    if current_name is not None:
        monster_records.append((current_name, current_unique))
    regular_names = {
        name for name, unique in monster_records
        if not unique and not name.startswith("<")
    }
    translated_monster_names = {name for name, _ in table(mon_header)}
    expect(regular_names == translated_monster_names,
           "Original Angband non-unique monster catalog incomplete: "
           + ", ".join(sorted(regular_names - translated_monster_names)[:15]))
    for symbol in ("angdroid_utf8_display_columns",
                   "angdroid_utf8_clip_columns",
                   "angdroid_utf8_printf_field_bytes"):
        expect(symbol in display_width,
               f"Missing Unicode column helper: {symbol}")
        expect(symbol in monster_list and symbol in object_list,
               f"Native monster/item lists are not both using {symbol}")
    expect("test_unicode_display.c" in native_tests,
           "CJK UTF-8 clipping regression is not registered")
    expect("angdroid_utf8_skip_columns" in display_width
           and "angdroid_utf8_prefix_columns" in display_width,
           "Message recall display-column navigation helpers are missing")
    expect("angdroid_utf8_skip_columns(msg, (size_t)q)" in knowledge_ui,
           "Message history still slices by UTF-8 byte offset")
    expect("angdroid_utf8_prefix_columns(msg, str)" in knowledge_ui,
           "Message history search highlight still uses byte offsets")
    expect("angdroid_utf8_clip_columns(highlighted" in knowledge_ui,
           "Message search highlight may cut through a Chinese character")

    # Unique monster names only alter UI labels, not engine race identities.
    unique_test = source(
        ROOT / "tests/native/test_unique_monster_locale.c", errors)
    expect("test_unique_monster_locale.c" in native_tests
           and 'ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES == 101' in unique_test
           and 'angdroid_zh_tw_unique_monster_name(NULL) == NULL' in unique_test,
           "Unique monster C99 regression must be registered with fallbacks")
    unique_entries = table(unique_mon_header)
    unique_source_names = {
        name for name, unique in monster_records if unique
    }
    expect(len(unique_entries) == 101 and len(unique_source_names) == 101
           and {name for name, _ in unique_entries} == unique_source_names
           and len({name for name, _ in unique_entries}) == len(unique_entries)
           and all(zh.strip() for _, zh in unique_entries)
           and "#define ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES 101"
               in unique_mon_header,
           "Unique monster names must cover all 101 canonical source names")
    expect('"zh-tw-unique-monsters.h"' in mon
           and 'angdroid_zh_tw_unique_monster_name(race->name)' in mon
           and 'strnfmt(buf, buflen, "[U] %s", translated)' in mon
           and 'strnfmt(buf, buflen, "[U] %s", race->name)' in mon,
           "Unique names must stay display-only in monster lists")
    # Translating an unidentified visual label must not reveal the kind.
    # The 302 flavor/fixed source entries contain 202 distinct nonempty texts.
    flavor_source = source(ROOT / "variant_lib/angband/gamedata/flavor.txt", errors)
    flavor_header = source(CPP / "angband/zh-tw-flavor-names.h", errors)
    flavor_native_test = source(
        ROOT / "tests/native/test_item_flavor_locale.c", errors)
    flavor_names = {
        line.split(":")[-1] for line in flavor_source.splitlines()
        if line.startswith(("flavor:", "fixed:")) and line.split(":")[-1]
    }
    flavor_pairs = table(flavor_header)
    expect(len(flavor_names) == 202 and len(flavor_pairs) == 202
           and {en for en, _ in flavor_pairs} == flavor_names
           and len({en for en, _ in flavor_pairs}) == len(flavor_pairs)
           and all(zh.strip() for _, zh in flavor_pairs)
           and "#define ANGDROID_ZH_TW_FLAVOR_NAMES 202" in flavor_header,
           "All unidentified flavor labels must match canonical flavor.txt")
    expect('"zh-tw-flavor-names.h"' in obj
           and "if (!easy_know && !kind->aware && kind->flavor)" in obj
           and "angdroid_zh_tw_flavor_name(flavor)" in obj
           and "my_strcpy(buf, localized ? localized : flavor, max);" in obj
           and "show_flavor && obj->kind->flavor" in obj
           and "angdroid_zh_tw_flavor_name(obj->kind->flavor->text)" in obj
           and "if (localized_flavor) modstr = localized_flavor;" in obj
           and "test_item_flavor_locale.c" in native_tests
           and 'angdroid_zh_tw_flavor_name(NULL) == NULL' in flavor_native_test,
           "Flavor display translation must preserve unknown-item isolation")
    # Narrative paragraphs are presentation-only.  The complete source
    # ego corpus has 5 described records, and the artifact translation
    # grows in reviewable batches while retaining English fallback.
    artifact_story = source(CPP / "angband/zh-tw-artifact-lore.h", errors)
    ego_story = source(CPP / "angband/zh-tw-ego-lore.h", errors)
    story_test = source(ROOT / "tests/native/test_item_story_locale.c", errors)
    described_artifacts = set()
    current_name = None
    has_description = False
    for line in source(
            ROOT / "variant_lib/angband/gamedata/artifact.txt", errors).splitlines():
        if line.startswith("name:"):
            if current_name is not None and has_description:
                described_artifacts.add(current_name)
            current_name = line[5:]
            has_description = False
        elif line.startswith("desc:") and current_name is not None:
            has_description = True
    if current_name is not None and has_description:
        described_artifacts.add(current_name)
    artifact_lore_entries = table(artifact_story)
    ego_lore_entries = table(ego_story)
    expect(len(described_artifacts) == 138
           and len(artifact_lore_entries) == 138
           and {en for en, _ in artifact_lore_entries} == described_artifacts
           and len({en for en, _ in artifact_lore_entries}) == 138
           and all(zh.strip() for _, zh in artifact_lore_entries)
           and "#define ANGDROID_ZH_TW_ARTIFACT_LORE 138" in artifact_story,
           "Artifact lore descriptions need all 138 canonical source keys")
    expect(len(ego_lore_entries) == 5
           and {en for en, _ in ego_lore_entries} ==
               {"of Accuracy", "of Power", "of Wounding",
                "of Slaying", "of Craftsmanship"}
           and "#define ANGDROID_ZH_TW_EGO_LORE 5" in ego_story
           and all(zh.strip() for _, zh in ego_lore_entries),
           "All five canonical ego lore descriptions need zh-TW mappings")
    expect('"zh-tw-artifact-lore.h"' in obj_info
           and '"zh-tw-ego-lore.h"' in obj_info
           and "angdroid_zh_tw_artifact_flavor(obj->artifact->name)" in obj_info
           and "angdroid_zh_tw_ego_flavor(obj->ego->name)" in obj_info
           and "obj->known->artifact && obj->artifact->text" in obj_info
           and "obj->known->ego != NULL" in obj_info
           and "!(mode & OINFO_SPOIL)" in obj_info
           and "if (translated) flavor = translated;" in obj_info
           and "test_item_story_locale.c" in native_tests
           and 'angdroid_zh_tw_artifact_flavor(NULL) == NULL' in story_test,
           "Artifact/ego narratives need known-item and dump isolation")
    # Original object.txt has 141 described kinds. Translating their lore
    # must not reveal unknown objects or alter chardump/spoiler output.
    kind_story = source(CPP / "angband/zh-tw-kind-lore.h", errors)
    kind_story_test = source(ROOT / "tests/native/test_kind_lore_locale.c", errors)
    described_kinds = set()
    current_kind_name = None
    has_kind_description = False
    for line in item_data.splitlines():
        if line.startswith("name:"):
            if current_kind_name is not None and has_kind_description:
                described_kinds.add(current_kind_name)
            current_kind_name = line[5:]
            has_kind_description = False
        elif line.startswith("desc:") and current_kind_name is not None:
            has_kind_description = True
    if current_kind_name is not None and has_kind_description:
        described_kinds.add(current_kind_name)
    kind_lore_pairs = table(kind_story)
    expect(len(described_kinds) == 141
           and len(kind_lore_pairs) == 141
           and {en for en, _ in kind_lore_pairs} == described_kinds
           and len({en for en, _ in kind_lore_pairs}) == len(kind_lore_pairs)
           and all(zh.strip() for _, zh in kind_lore_pairs)
           and "#define ANGDROID_ZH_TW_KIND_LORE 141" in kind_story,
           "All 141 original item-kind stories must match canonical source")
    expect('"zh-tw-kind-lore.h"' in obj_info
           and "if (obj->kind != obj->known->kind)" in obj_info
           and "object_flavor_is_aware(obj) || ego" in obj_info
           and "if (!ego && obj->kind->text)" in obj_info
           and "angdroid_zh_tw_kind_flavor(obj->kind->name)" in obj_info
           and "if (translated) flavor = translated;" in obj_info
           and "!(mode & OINFO_SPOIL)" in obj_info
           and "test_kind_lore_locale.c" in native_tests
           and 'angdroid_zh_tw_kind_flavor(NULL) == NULL' in kind_story_test,
           "Known-kind narrative display must preserve identity and English export")
    # Only known artifacts and explicit display flags may use translated
    # labels. All 138 names must match canonical artifact.txt exactly.
    artifact_source = source(ROOT / "variant_lib/angband/gamedata/artifact.txt", errors)
    artifact_header = source(CPP / "angband/zh-tw-artifact-names.h", errors)
    artifact_native_test = source(
        ROOT / "tests/native/test_artifact_names_locale.c", errors)
    artifact_names = {
        line[5:] for line in artifact_source.splitlines() if line.startswith("name:")
    }
    artifact_pairs = table(artifact_header)
    expect(len(artifact_names) == 138 and len(artifact_pairs) == 138
           and {en for en, _ in artifact_pairs} == artifact_names
           and len({en for en, _ in artifact_pairs}) == len(artifact_pairs)
           and all(zh.strip() for _, zh in artifact_pairs)
           and "#define ANGDROID_ZH_TW_ARTIFACT_NAMES 138" in artifact_header,
           "Artifact names must cover all 138 canonical Angband keys")
    expect('"zh-tw-artifact-names.h"' in obj
           and 'if (object_is_known_artifact(obj))' in obj
           and 'const char *artifact_name = obj->artifact->name;' in obj
           and 'angdroid_zh_tw_artifact_name(artifact_name)' in obj
           and 'if (localized) artifact_name = localized;' in obj
           and 'strnfcat(buf, max, &end, " %s", artifact_name);' in obj
           and "test_artifact_names_locale.c" in native_tests
           and 'angdroid_zh_tw_artifact_name(NULL) == NULL' in artifact_native_test,
           "Known artifact suffix names must stay display-only with English fallback")
    # The 107 ego records have 79 distinct source names; only identified,
    # opted-in screen descriptions may use their translated suffix.
    ego_source = source(ROOT / "variant_lib/angband/gamedata/ego_item.txt", errors)
    ego_header = source(CPP / "angband/zh-tw-ego-names.h", errors)
    ego_native_test = source(ROOT / "tests/native/test_ego_names_locale.c", errors)
    ego_names = {
        line[5:] for line in ego_source.splitlines() if line.startswith("name:")
    }
    ego_pairs = table(ego_header)
    expect(len(ego_names) == 79 and len(ego_pairs) == 79
           and {en for en, _ in ego_pairs} == ego_names
           and len({en for en, _ in ego_pairs}) == len(ego_pairs)
           and all(zh.strip() for _, zh in ego_pairs)
           and "#define ANGDROID_ZH_TW_EGO_NAMES 79" in ego_header,
           "All canonical ego suffixes need exact Traditional Chinese labels")
    expect('"zh-tw-ego-names.h"' in obj
           and "angdroid_zh_tw_ego_name(ego_name)" in obj
           and 'const char *ego_name = obj->ego->name;' in obj
           and 'if (localized) ego_name = localized;' in obj
           and 'strnfcat(buf, max, &end, " %s", ego_name);' in obj
           and "obj->known->ego && !(mode & ODESC_NOEGO)" in obj
           and "test_ego_names_locale.c" in native_tests
           and 'angdroid_zh_tw_ego_name(NULL) == NULL' in ego_native_test,
           "Ego suffix must remain identified, screen-only and C99 tested")
    # All known item-class classifiers must match the exact original tval keys.
    grammar_header = source(CPP / "angband/zh-tw-item-grammar.h", errors)
    tval_source = source(CPP / "angband/list-tvals.h", errors)
    source_tvals = re.findall(r'^TV\(\w+,\s*"([^"]+)"\)', tval_source,
                              re.MULTILINE)
    # Restrict the count to the canonical tval table; the same header
    # also includes eight effect+noun pairs for identified flavored kinds.
    classifier_body = grammar_header.split(
        "units[ANGDROID_ZH_TW_ITEM_CLASSIFIERS] = {", 1)[1].split(
        "    };", 1)[0]
    grammar_units = re.findall(r'^\s*\{ "([^"]+)", "([^"]+)" \},?$',
                               classifier_body, re.MULTILINE)
    expect(len(source_tvals) == 36 and len(grammar_units) == 36 and
           source_tvals == [name for name, _ in grammar_units] and
           len(set(source_tvals)) == 36 and
           all(unit for _, unit in grammar_units),
           "Known item classifiers must match all 36 original tval labels")
    expect('"zh-tw-item-grammar.h"' in obj and
           "angdroid_zh_tw_known_item_name(" in obj and
           "angdroid_zh_tw_identified_flavor_name(named_effect," in obj and
           "angdroid_zh_tw_item_append_utf8(buf, max, &end," in obj and
           obj.count("angdroid_zh_tw_item_suffix(") == 2 and
           obj.count("if (localized_kind && translated_suffix)") == 2 and
           "test_item_name_grammar_locale.c" in native_tests and
           "test_item_name_grammar_localization.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors),
           "Opted-in item classifier/suffix/UTF-8 regression is not wired")

    # Numeric details are display-only; original knowledge gates are retained.
    detail_header = source(CPP / "angband/zh-tw-item-details.h", errors)
    detail_test = source(ROOT / "tests/test_item_details_localization.py", errors)
    expect('"zh-tw-item-details.h"' in obj and
           "bool zh_tw = (mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled();" in obj and
           "if (!((obj->notice & OBJ_NOTICE_ASSESSED) || spoil)) return end;" in obj and
           "obj_desc_mods(obj->known, buf, max, end, mode)" in obj and
           "angdroid_zh_tw_item_modifiers_text(" in obj and
           "angdroid_zh_tw_item_append_amount(" in obj and
           "angdroid_zh_tw_item_append_whole(buf, max, &end, label);" in obj and
           "test_item_details_locale.c" in native_tests and
           "test_item_details_localization.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors) and
           "test_complete_utf8_append_and_no_persistent_changes" in detail_test and
           "angdroid_zh_tw_item_combat_text" in detail_header,
           "Combat/modifier/annotation display needs UTF-8 and source gate tests")

    # Explicit trade approval must not accept native touch converted to Enter.
    shop_confirm_header = source(
        CPP / "angband/store-confirmation-text.h", errors)
    shop_confirm_test = source(
        ROOT / "tests/test_store_confirmation_source.py", errors)
    expect('"store-confirmation-text.h"' in store_ui and
           "angdroid_store_explicit_yes(true, response.key.code)" in store_ui and
           "if (response.type != EVT_KBRD) return false;" in store_ui and
           "flush((game_event_type)0, NULL, NULL);" in store_ui and
           "angdroid_store_quantity_prompt(" in store_ui and
           "angdroid_store_explicit_yes(" in shop_confirm_header and
           "test_store_confirmation_text.c" in native_tests and
           "test_store_confirmation_source.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors) and
           "test_confirmation_accepts_explicit_y_only" in shop_confirm_test,
           "Store trades require explicit Y and translated quantity prompts")

    # Actual stock reorder invalidates prior item identity even if the same
    # numeric index stays within the new stock count. Queued taps are drained.
    invalidation_source = source(
        ROOT / "tests/test_store_input_invalidation_source.py", errors)
    expect("angdroid_store_clear_stale_selection(" in source(
               CPP / "angband/store-stock-guard.h", errors) and
           "ctx->stock_changed = true;" in store_ui and
           "flush(type, unused, user);" in store_ui and
           "angdroid_store_clear_stale_selection(ctx->stock_changed," in store_ui and
           "if (is_valid_row(menu, menu->cursor))" in menu_ui and
           "test_store_input_invalidation_source.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors) and
           "test_mutation_invalidates_previous_stock_id_and_drains_queue"
               in invalidation_source,
           "Stock changes must invalidate old selection and pending input")

    # Live touch selection must resolve its current hit row before invoking
    # the stock callback, while empty/header and escape touches remain safe.
    menu_hit_source = source(CPP / "angband/menu-hit-test.h", errors)
    menu_cursor_source = source(CPP / "angband/menu-valid-cursor.h", errors)
    touch_test = source(ROOT / "tests/test_menu_hit_test_source.py", errors)
    nav_test = source(ROOT / "tests/test_menu_navigation_source.py", errors)
    store_touch_test = source(
        ROOT / "tests/test_store_menu_flow_source.py", errors)
    expect("angdroid_menu_scrolling_hit(" in menu_hit_source and
           "angdroid_menu_page_next_cursor(" in menu_cursor_source and
           "if (region_inside(&menu->active, &in)) {" in menu_ui and
           "if (out.type == EVT_SELECT && !no_act) {" in menu_ui and
           "!touch_select_dispatched &&" in menu_ui and
           "angdroid_menu_page_next_cursor(menu->cursor," in menu_ui and
           "angdroid_menu_find_valid_cursor(start, count, 1," in menu_ui and
           "region_inside(&m->active, event) &&" in store_ui and
           "test_menu_valid_cursor.c" in native_tests and
           "touch row may dispatch stale selection" in touch_test and
           "Spacebar paging can skip the final menu row" in nav_test and
           "an outside/return touch can buy" in store_touch_test,
           "Mouse dispatch must use the tapped row, and page cursor stay valid")

    # Portrait rotation: item selection region, scrolled quiver footer, and
    # store help must never overlap active stock rows or invalid touch targets.
    selection_layout = source(CPP / "angband/item-selection-layout.h", errors)
    selection_py = source(
        ROOT / "tests/test_item_selection_rotation_source.py", errors)
    expect('"item-selection-layout.h"' in object_ui and
           "angdroid_item_selection_region(width, height," in object_ui and
           "get_item_action, item_menu_resize };" in object_ui and
           "angdroid_item_selection_footer_row(local_area->row," in object_ui and
           "angdroid_item_selection_footer_row(" in selection_layout and
           "test_item_selection_layout.c" in native_tests and
           "test_item_selection_rotation_source.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors) and
           "test_rotated_menu_keeps_click_row_and_footer_separate" in selection_py and
           "Term->wid >= 70 && Term->hgt >= 16" in store_ui,
           "Portrait selection and compact shop help need resize regression tests")

    # Responsive item-list widths and untranslated shop stock are a coupled
    # regression: stock is opted-in to zh-TW, amounts stay full and unmodified.
    inventory_layout = source(CPP / "angband/object-list-text.h", errors)
    store_detail_text = source(CPP / "angband/store-detail-text.h", errors)
    inventory_static_test = source(
        ROOT / "tests/test_object_list_text_source.py", errors)
    store_static_test = source(
        ROOT / "tests/test_store_column_layout_source.py", errors)
    expect("angdroid_object_list_fit_extras(" in inventory_layout and
           "angdroid_object_list_detail_fits(" in inventory_layout and
           "object_list_visible_extras(mode, Term->wid)" in object_ui and
           "object_list_visible_extras(olist_mode, Term->wid)" in object_ui and
           "if (olist_render_mode & OLIST_RECHARGE) ex_width += 10;" in object_ui and
           "angdroid_store_price_text(" in store_detail_text and
           "angdroid_store_weight_text(" in store_detail_text and
           '"store-detail-text.h"' in store_ui and
           "uint32_t desc = ODESC_PREFIX | ODESC_ZH_TW;" in store_ui and
           "test_store_detail_text.c" in native_tests and
           "numeric-column fitting policy" in inventory_static_test and
           "bilingual bounded formatter" in store_static_test,
           "Shop and inventory columns must reserve numeric fields safely")

    # Source-matched appearance grammar only, not hidden item effects.
    flavored_header = source(CPP / "angband/zh-tw-flavored-object.h", errors)
    flavored_types = re.findall(
        r'\{ "(ring|amulet|staff|wand|rod|potion|mushroom)", "[^"]+", "[^"]+" \}',
        flavored_header)
    expect(len(flavored_types) == 7 and len(set(flavored_types)) == 7 and
           '"zh-tw-flavored-object.h"' in obj and
           "if (!aware || visible_effect)" in obj and
           "else if (localized_flavored)" in obj and
           "test_flavored_object_locale.c" in native_tests and
           "test_flavored_object_localization.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors),
           "Flavored item noun grammar must be visible-only and C99 tested")

    # Random Latin-like scroll titles must remain stable identifiers.
    scroll_header = source(CPP / "angband/zh-tw-scroll-title.h", errors)
    scroll_tests = source(ROOT / "tests/test_scroll_title_localization.py", errors)
    expect('"zh-tw-scroll-title.h"' in obj and
           "angdroid_zh_tw_scroll_title(buf, max, flavor, false)" in obj and
           "obj->tval == TV_SCROLL && show_flavor" in obj and
           "obj->kind->flavor->text, true" in obj and
           "!obj->artifact && !obj->ego && obj->kind->flavor" in obj and
           "else if (localized_scroll && aware && !obj->artifact)" in obj and
           'strnfcat(buf, max, &end, "（%s）",' in obj and
           "return false;" in scroll_header and
           "test_scroll_title_locale.c" in native_tests and
           "test_scroll_title_localization.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors) and
           "seed_flavor" in scroll_tests,
           "Random scroll titles must be bilingual and preserve canonical RNG")

    expect("if (!easy_know && !kind->aware && kind->flavor)" in obj,
           "Unidentified item flavors must not be disclosed")
    expect("obj_desc_name_format(buf, max, 0, kind->name, NULL, false)" in obj,
           "Unknown item display names must retain the original formatter")
    expect("zh_tw ? \"%s沒有怪物。" in monster_list and
           "angdroid_zh_tw_enabled()" in monster_list,
           "Monster list Chinese and English formatting not found")
    expect("zh_tw ? \"%s沒有物品。" in object_list and
           "angdroid_zh_tw_enabled()" in object_list,
           "Item list Chinese and English formatting not found")

    for name in ("test_death_locale.c", "test_monster_locale.c",
                 "test_item_locale.c"):
        expect(name in native_tests, f"Native regression not registered: {name}")

    for english, chinese in (("Point-based", "點數分配"),
                             ("New game", "建立新遊戲"),
                             ("Save and quit", "儲存並離開")):
        expect(english in game_ui_header and chinese in game_ui_header,
               "Missing original Angband UI translation: " + english)
    expect("angdroid_game_ui_text(commands[oid].desc" in context_ui,
           "Command description translation is not display-only")
    expect("angdroid_game_ui_text(cmds_all[oid].name" in context_ui,
           "Command category translation is not display-only")
    expect("birth_prompt_column(prompt)" in birth_ui,
           "CJK character creation prompt width is not measured")
    # strnfcat() is a printf-style API: a dynamic translation may only
    # be a value argument, never the format string (NDK -Wformat-security).
    for english in ("['r' to reroll", ", 'p' for previous roll",
                    " or 'Enter' to accept]"):
        expected = '"%s", angdroid_game_ui_text("' + english + '"'
        expect(expected in birth_ui,
               "Localized roller prompt needs a literal %s format: " + english)
    expect(not re.search(
        r"strnfcat\s*\([^;]*?&promptlen,\s*angdroid_game_ui_text\s*\(",
        birth_ui),
        "Unsafe localized strnfcat format string reintroduced")
    expect('angdroid_game_ui_text("New game"' in game_ui,
           "Savegame selector is not translated at display time")
    expect("test_game_ui_locale.c" in native_tests,
           "Missing C99 native UI regression")
    for english, chinese in (("Human", "人類"), ("Half-Elf", "半精靈"),
                             ("Kobold", "狗頭人"), ("Warrior", "戰士"),
                             ("Necromancer", "死靈法師"),
                             ("Blackguard", "黑衛士")):
        expect(('"' + english + '"') in game_ui_header
               and ('"' + chinese + '"') in game_ui_header,
               "Missing original Angband race/class display translation: " + english)
    expect("angdroid_game_ui_text(data->items[oid]" in birth_ui,
           "Race and class menu items are not translated at render time")
    expect("angdroid_game_ui_text(player->race->name" in player_ui
           and "angdroid_game_ui_text(player->class->name" in player_ui,
           "Character sheet is not using display-only race and class labels")
    for english, chinese in (("Your Home", "我的家"),
                             ("Store Inventory", "商店物品"),
                             ("Cancelled.", "已取消。"),
                             ("You have nothing in your inventory.",
                              "背包裡沒有物品。")):
        expect(english in game_ui_header and chinese in game_ui_header,
               "Missing gameplay Chinese display text: " + english)
    expect('msg("%s", angdroid_game_ui_text(' in store_ui,
           "Store messages need a literal printf format")
    expect('msg("%s", angdroid_game_ui_text(' in knowledge_ui,
           "Inventory warnings need a literal printf format")
    expect('msg("%s", angdroid_game_ui_text(' in game_ui,
           "Game save/cancel messages need a literal printf format")
    expect('angdroid_utf8_display_columns(continue_prompt)' in input_ui,
           "Localized pause line must use CJK column width")
    expect('home ? "存放" : "出售"' in store_ui and
           'home ? "取出" : "購買"' in store_ui,
           "Store Buy/Sell must differ from character stat adjustment labels")
    expect("angdroid_utf8_display_columns(entry->text)" in menu_ui,
           "Dynamic menu geometry uses UTF-8 byte length instead of columns")
    # Popup geometry now lives in a reusable bounds helper instead of an
    # inline MIN() expression. Check that both terminal dimensions enter
    # the helper and that its safe region is used for the popup layout.
    expect(re.search(r"angdroid_menu_popup_bounds\(\s*"
                     r"Term->wid,\s*Term->hgt,\s*mx,\s*my,\s*"
                     r"menu_dynamic_longest_entry\(m\)", menu_ui) is not None
           and "r.col = popup.col;" in menu_ui
           and "r.row = popup.row;" in menu_ui
           and "r.width = popup.width;" in menu_ui
           and "r.page_rows = popup.page_rows;" in menu_ui,
           "Dynamic menu geometry needs a screen-width bound")
    expect("angdroid_utf8_display_columns(pl->label)" in player_ui,
           "Character sheet label alignment uses UTF-8 byte length")
    expect("angdroid_utf8_clip_columns(value" in player_ui,
           "Character sheet value clipping may split UTF-8 glyphs")
    expect("Term_putstr(col, row, -1, COLOUR_WHITE, pl->label)" in player_ui,
           "Character sheet labels may be cut at a UTF-8 byte boundary")
    expect('"You cannot browse that."' in game_ui_header
           and '"無法瀏覽這本書。"' in game_ui_header,
           "Missing spell menu translation")
    expect('strnfmt(buf, sizeof buf, "選擇%s（%s）？按 ? 切換說明"' in spell_ui,
           "Spell selection not localized with a literal printf format")
    expect('msgt(MSG_STUDY, "你已學會：%s。"' in player_spell,
           "Spell learning message not localized")
    for english, chinese in (("That wand has no charges.", "這根魔杖已沒有使用次數。"),
                             ("Attempt it anyway? ", "仍要嘗試嗎？ ")):
        expect(english in game_ui_header and chinese in game_ui_header,
               "Missing item/magic translation: " + english)
    expect('msg("%s", angdroid_game_ui_text(' in obj_cmd
           and 'msgt(snd, "%s", angdroid_game_ui_text(' in obj_cmd,
           "Item feedback must keep literal printf formats and sounds")
    expect('strnfmt(prompt, sizeof prompt, "正在為 %s 刻字。"' in obj_cmd
           and 'strnfmt(prompt, sizeof prompt, "Inscribing %s."' in obj_cmd,
           "Inscription prompt must use literal locale-specific format strings")
    for english, chinese in (("There is a wall in the way!", "一面牆擋住了去路！"),
                             ("You clear the web.", "你清除了蜘蛛網。")):
        expect(english in game_ui_header and chinese in game_ui_header,
               "Missing exploration Chinese feedback: " + english)
    expect('msgt(MSG_STAIRS_UP, "%s", angdroid_game_ui_text(' in cave_cmd
           and 'msgt(MSG_HITWALL, "%s", angdroid_game_ui_text(' in cave_cmd,
           "Localized navigation events must preserve sound categories")
    expect('msgt(MSG_MISS, "你沒有擊中 %s。", m_name)' in player_attack
           and 'msgt(MSG_MISS, "You miss %s.", m_name)' in player_attack,
           "Melee miss messages must retain literal English and Chinese format strings")
    expect('msgt(MSG_AFRAID, "你太害怕了，無法攻擊 %s！", m_name)' in cave_cmd
           and 'msgt(MSG_AFRAID, "You are too afraid to attack %s!", m_name)' in cave_cmd,
           "Fear messaging must preserve original event and English fallback")
    expect('msg("%s", angdroid_game_ui_text("You have nothing to fire with."' in player_attack,
           "Ranged weapon warning not translated safely")
    for name in ("angdroid_melee_verb_zh_tw", "angdroid_ranged_verb_zh_tw",
                 "angdroid_hit_quality_zh_tw"):
        expect(name in combat_header and name in player_attack,
               "Combat display translation missing: " + name)
    expect('msgt(msg_type, "You %s %s%s. %s"' in player_attack
           and 'msgt(msg_type, "你%s %s%s。%s"' in player_attack,
           "Melee translation must preserve its English fallback")
    expect('msgt(msg_type, "Your %s %s %s%s. %s"' in player_attack
           and 'msgt(msg_type, "你的 %s %s %s%s。%s"' in player_attack,
           "Ranged translation must preserve its English fallback")
    expect('msgt(MSG_DISARM, "你成功解除「%s」。", trap->kind->name)' in cave_cmd,
           "Trap disarm message must retain event and parameter")
    expect('msg("你觸發了「%s」！", trap->kind->name)' in cave_cmd,
           "Trap triggered message translation not connected")
    expect('msg("你沒能正確%s。", action_zh)' in obj_cmd
           and 'msg("You failed to %s properly.", action)' in obj_cmd,
           "Device usage warning must preserve English format fallback")
    expect('angdroid_game_ui_text("Target Aborted."' in target_ui,
           "Target abort label missing")
    expect('r.col = MAX(0, Term->wid - r.width - 1)' in object_ui,
           "Ignore menu still uses fixed 80-column coordinates")
    expect('angdroid_game_ui_text("This item only"' in object_ui,
           "Ignore action not translated")
    expect('angdroid_game_ui_text("Inven:"' in object_ui,
           "Inventory chooser header not translated")
    # Separate translated display title from the original English title.
    expect('const char *display_title = angdroid_game_ui_text(title,' in
           knowledge_ui
           and 'format("知識圖鑑：%s", display_title)' in knowledge_ui
           and 'format("Knowledge - %s", title)' in knowledge_ui,
           "Knowledge browser heading lacks Chinese display text")
    expect('MIN(79, Term->wid)' in knowledge_ui,
           "Knowledge divider must not extend past narrow screen")
    expect('format("擊殺怪物：%d/%d（本組／總計）"' in knowledge_ui,
           "Monster kill counter translation missing")
    expect('strnfmt(out_val, sizeof out_val, "全部忽略 %s", tmp)' in object_ui
           and 'strnfmt(out_val, sizeof out_val, "All %s", tmp)' in object_ui,
           "Object ignore labels must use separate literal printf format strings")
    expect('strnfmt(out_val, sizeof out_val, "取消忽略全部 %s", tmp)' in object_ui
           and 'strnfmt(out_val, sizeof out_val, "Unignore all %s", tmp)' in object_ui,
           "Unignore group must preserve literal Chinese and English printf formats")
    expect("#define ANGDROID_ZH_TW_MONSTER_LORE 623" in monster_lore_dict,
           "Monster flavor dictionary size changed")
    expect("angdroid_zh_tw_monster_flavor(race->name)" in monster_lore
           and "localized ? localized : race->text" in monster_lore,
           "Monster flavor display-only mapping or fallback missing")
    expect("angdroid_zh_tw_monster_list_name(race->name)" in monster_lore_ui
           and "name_zh ? name_zh : race->name" in monster_lore_ui,
           "Monster lore title translation or unique fallback missing")
    expect("test_monster_lore_locale.c" in native_tests,
           "Monster lore host test not registered")
    for phrase in ("這名敵人曾殺害你的 %d 位先祖",
                   "這類怪物的平均生命值為",
                   "若能看清牠，你的近戰命中率約為",
                   "這隻怪物可能與其他怪物一同出現"):
        expect(phrase in monster_lore,
               "Missing complete zh-TW lore paragraph: " + phrase)
    expect('if (angdroid_zh_tw_enabled() && lore->armour_known)' in monster_lore,
           "HP and armor translation must honor the lore knowledge gate")
    expect('textblock_append(tb, "%s has slain %d of your ancestors"' in monster_lore,
           "Monster lore English kill grammar was removed")
    for term in ('"cast", "施放"', '"recite", "誦念"',
                 '"chant", "吟唱"', '"perform", "舉行"',
                 '"spell", "法術"', '"prayer", "祈禱"',
                 '"verse", "詠唱"', '"ritual", "儀式"'):
        expect(term in magic_terms, "Missing original realm.txt term " + term)
    expect("angdroid_zh_tw_magic_term(noun, true)" in spell_ui
           and "angdroid_zh_tw_magic_term(verb, true)" in spell_ui,
           "Spell menu Chinese still leaks English realm terms")
    expect('msg("法力不足，無法%s這項%s。"' in obj_cmd
           and 'msg("You do not have enough mana to %s this %s."' in obj_cmd,
           "Mana warning must preserve Chinese and English literal formats")
    for english, chinese in (("orc", "獸人"), ("dragon", "龍"),
                             ("natural", "自然"), ("undead", "不死"),
                             ("normal speed", "正常速度"),
                             ("incredibly quickly", "極度迅速")):
        expect(('"' + english + '"') in game_ui_header
               and ('"' + chinese + '"') in game_ui_header,
               "Untranslated monster type or speed label: " + english)
    expect('if (angdroid_zh_tw_enabled()) {' in monster_lore
           and 'lore_describe_speed(race->speed)' in monster_lore,
           "Monster movement paragraph lost its Chinese option")
    expect('OPT(player, effective_speed)' in monster_lore
           and 'extract_energy[player->state.speed]' in monster_lore,
           "Localized movement must preserve relative speed option")
    expect('rf_has(known_flags, RF_NEVER_MOVE)' in monster_lore,
           "Localized movement must preserve known immobilization")
    expect('擊殺一隻這種怪物可獲得 ' in monster_lore
           and 'player->lev' in monster_lore,
           "Monster experience must use equivalent Chinese level calculation")
    expect('if (angdroid_zh_tw_enabled() && (n > 0 || nspec > 0))' in monster_lore
           and 'if (!lore->drop_known) return;' in monster_lore,
           "Monster drop translation must retain known drop gate")
    expect('這隻怪物%s；可能在 ' in monster_lore
           and 'if (lore->sleep_known)' in monster_lore,
           "Monster awareness translation must retain known hearing gate")
    for term in ("prefers to ignore", "is very observant of",
                 "is ever vigilant for"):
        expect(('"'+term+'"') in game_ui_header,
               "Missing monster awareness term: " + term)
    for keyword in ("invisible", "open doors", "pass through walls",
                    "lightning", "nether", "disenchantment", "slowed"):
        expect(('"' + keyword + '"') in game_ui_header,
               "Monster ability label not mapped: " + keyword)
    expect('angdroid_game_ui_text(describe_race_flag(flag), true)' in monster_lore,
           "Monster ability flags not translated at render time")
    expect('if (count && angdroid_zh_tw_enabled())' in monster_lore
           and 'if (end && streq(end, ".  "))' in monster_lore,
           "Chinese monster ability clause and punctuation missing")
    for symbol, data, field in (
        ("angdroid_zh_tw_blow_method", blow_methods_data, "desc:"),
        ("angdroid_zh_tw_blow_effect", blow_effects_data, "desc:"),
        ("angdroid_zh_tw_monster_spell", monster_spell_data, "lore:"),
    ):
        # Each table has a separate exact-match namespace, so identical
        # English strings (such as "poison") may have different meanings.
        start = lore_terms.find("static inline const char *" + symbol + "(")
        expect(start >= 0, "Missing lore term function " + symbol)
        if start >= 0:
            end = lore_terms.find("\n}", start)
            section = lore_terms[start:end + 2]
            keys = {en for en, _ in table(section)}
            expected = {line.partition(":")[2] for line in data.splitlines()
                        if line.startswith(field)}
            expect(keys == expected,
                   "Monster lore term set differs from canonical " + symbol)
        expect(symbol + "(" in monster_lore,
               "Monster lore term not wired: " + symbol)
    expect('if (count && angdroid_zh_tw_enabled())' in monster_lore
           and 'angdroid_zh_tw_monster_spell(' in monster_lore,
           "Monster lore spell list did not use Chinese separators and labels")
    for snippet in ("牠可能", "，也可能", "吐出", "施放法術",
                    "，且懂得巧妙施法", "，例如", " 次／"):
        expect(snippet in monster_lore,
               "Incomplete Chinese spell lore sentence: " + snippet)
    expect('mon_spell_lore_damage(spell, race, know_hp)' in monster_lore,
           "Monster spell lore damage source changed")
    expect('ref_race = old_ref;' in monster_lore,
           "Monster spell lore reference cleanup lost")
    for phrase in ("這隻怪物不會進行物理攻擊", "尚不清楚這隻怪物的攻擊方式",
                   "這隻怪物可能", "，效果為", "；每回合平均造成", " 點傷害。"):
        expect(phrase in monster_lore,
               "Missing Chinese monster melee lore phrase: " + phrase)
    expect('angdroid_zh_tw_blow_method(' in monster_lore
           and 'angdroid_zh_tw_blow_effect(' in monster_lore,
           "Physical attack labels no longer use gamedata term maps")
    expect('total_centidamage += (percent * randcalc(dice, 0, AVERAGE))' in monster_lore,
           "Monster attack average damage calculation changed")
    expect('textblock_append(tb, "%%）");' in monster_lore
           and 'textblock_append(tb, "%%)");' in monster_lore,
           "Monster lore must render a single percent glyph in both languages")
    expect('textblock_append(tb, "%s", angdroid_zh_tw_enabled() ? "%%）"' not in monster_lore,
           "Literal percent format escapes must not go through %s")
    expect('angdroid_item_info_text(' in obj_info
           and 'object_effect_is_known(obj)' in obj_info
           and 'if (obj->kind != obj->known->kind)' in obj_info,
           "Item description localization must preserve known/unidentified gates")
    expect('angdroid_item_info_text("It can be drunk.' in obj_info,
           "Unknown item use capability is not translated")
    expect('使用成功機率：%d.%d%%' in obj_info
           and 'Your chance of success is %d.%d%%' in obj_info,
           "Item recharge chance must preserve numeric formats and English fallback")
    expect('angdroid_item_info_text("Combat info:' in obj_info,
           "Item combat section not localized")
    expect('angdroid_item_info_text("It can be drunk.' in item_info_terms
           or '"這件物品可以飲用。' in item_info_terms,
           "Missing item info display dictionary")
    expect('zh-tw-item-info.h' in native_tests or
           'test_game_ui_locale.c' in native_tests,
           "Item info standalone regression not registered")
    expect('angdroid_zh_tw_object_term(list[i], zh_tw)' in object_info,
           "Item elemental/protection list terms not translated")
    expect('angdroid_zh_tw_object_term(slays[i].name' in object_info
           and 'angdroid_zh_tw_object_term(brands[i].name' in object_info,
           "Slay and brand terms not translated")
    for term in ("acid", "strength", "fear", "undead", "searching skill"):
        expect(('"' + term + '"') in object_info_terms,
               "Missing object property translation: " + term)
    expect('test_object_info_locale.c' in native_tests,
           "Object info host C test not registered")
    mon_msg_ui = source(CPP / "angband/mon-msg.c", errors)
    expect('"zh-tw-unique-monsters.h"' in mon_msg_ui
           and 'angdroid_zh_tw_unique_monster_name(race->name)' in mon_msg_ui
           and 'if (invisible)' in mon_msg_ui,
           "Unique message subjects need hidden-identity and English fallback")
    mon_msg_originals = source(CPP / "angband/list-mon-message.h", errors)
    mon_msg_zh = source(CPP / "angband/zh-tw-mon-messages.h", errors)
    pain_source = source(ROOT / "variant_lib/angband/gamedata/pain.txt", errors)
    pain_zh = source(CPP / "angband/zh-tw-mon-pain.h", errors)
    original_codes = set(re.findall(
        r"^\s*MON_MSG\(\s*([A-Z][A-Z0-9_]*)\s*,",
        mon_msg_originals, re.M)) - {"MAX"}
    translated_codes = set(re.findall(
        r"^ZH_MON_MSG\(([A-Z][A-Z0-9_]*),",
        mon_msg_zh, re.M))
    expect(len(original_codes) == 55 and original_codes == translated_codes,
           "Missing or extra named monster event translation")
    # Monster pain messages need the race for their source-specific
    # flavor, and subject formatting carries count/visibility metadata.
    expect("get_monster_message_zh_tw(msg->msg_code, msg->race)" in mon_msg_ui
           and "get_subject_zh_tw(subject_zh, sizeof(subject_zh)," in mon_msg_ui
           and "msg->race, msg->count," in mon_msg_ui
           and "(msg->flags & MON_MSG_FLAG_INVISIBLE) != 0" in mon_msg_ui
           and "(msg->flags & MON_MSG_FLAG_OFFSCREEN) != 0" in mon_msg_ui,
           "Monster message translation and Chinese subjects are not wired")
    expect("angdroid_zh_tw_monster_list_name(race->name)" in mon_msg_ui
           and 'if (!skip_subject(msg->msg_code))' in mon_msg_ui,
           "Monster message names or omit-subject code changed")
    expect('msgt(msg_type, "%s%s（平均 %d）"' in mon_msg_ui
           and 'msgt(msg_type, "%s%s (average %d)"' in mon_msg_ui,
           "Monster damage average translation missing English fallback")
    expect("test_monster_messages_locale.c" in native_tests,
           "Monster event C99 regression not registered")
    expect('angdroid_item_info_text("Average thrown damage: "' in object_info
           and 'angdroid_item_info_text("Average damage/round: "' in object_info,
           "Average item damage captions not localized")
    expect('textblock_append(tb, "每回合攻擊次數。' in object_info
           and 'textblock_append(tb, "blow%s/round.' in object_info,
           "Blow count must keep Chinese and original English templates")
    expect('增加 %d 點力量與 %d 點敏捷後，每回合可攻擊 %d.%d 次。' in object_info
           and 'With +%d STR and +%d DEX you would get %d.%d blows' in object_info,
           "Item blows/round mechanics must preserve numeric substitutions")
    expect(object_info.count('#include "droid.h"') == 1,
           "Duplicate native locale include in object info")
    expect('angdroid_zh_tw_object_term(names[i], true)' in object_info,
           "Digging terrain labels must use the display-only dictionary")
    expect('textblock_append(tb, "回合%s"' in object_info
           and 'textblock_append(tb, "turn%s%s"' in object_info,
           "Digging times must use separate literal formats")
    for terrain in ("rubble", "magma veins", "quartz veins", "granite"):
        expect(('"' + terrain + '"') in object_info_terms,
               "Missing exact terrain name: " + terrain)
    canonical_pain = {
        line.split(":", 1)[1] for line in pain_source.splitlines()
        if line.startswith("message:")
    }
    localized_pain = table(pain_zh)
    expect(len(canonical_pain) == 65 and len(localized_pain) == 65
           and canonical_pain == {name for name, _ in localized_pain},
           "Angband pain translations must match every original pain.txt phrase")
    expect('get_monster_message_zh_tw(msg->msg_code, msg->race)' in mon_msg_ui
           and 'angdroid_zh_tw_pain_message(source)' in mon_msg_ui,
           "Monster pain messages are not translated after severity selection")
    for severity in ("95", "75", "50", "35", "20", "10", "0"):
        expect('case MON_MSG_' + severity + ':' in mon_msg_ui,
               "Monster pain severity selection changed: " + severity)
    expect("test_monster_pain_locale.c" in native_tests,
           "Monster pain C99 test not registered")
    mon_util = source(CPP / "angband/mon-util.c", errors)
    for snippet in ('你擊殺了一個看不見的敵人。',
                    '你摧毀了 %s。', '你擊敗了 %s。',
                    '%s 閃爍著改變了形態！'):
        expect(snippet in mon_util,
               "Monster death/shapechange translation missing: " + snippet)
    expect('angdroid_zh_tw_monster_list_name(mon->race->name)' in mon_util
           and 'monster_is_visible(mon)' in mon_util,
           "Monster kill name translation must preserve identity visibility")
    for original in ('You have killed %s.', 'You have destroyed %s.',
                     'You have slain %s.', 'shimmers and changes!'):
        expect(original in mon_util,
               "Monster kill/shapechange English fallback removed: " + original)
    spell_keys = {en for en, _ in re.findall(
        r'^\s*\{\s*"([^"]+)",\s*"([^"]+)"\s*\},\s*$',
        spell_names, re.MULTILINE)}
    canonical_spells = {line.split(":")[1]
                        for line in class_data.splitlines()
                        if line.startswith("spell:")}
    expect(spell_keys == canonical_spells,
           "Spell names must match the full canonical Angband class.txt set")
    expect('angdroid_zh_tw_spell_name(spell->name' in spell_ui
           and 'angdroid_utf8_clip_columns(spell_name_buf, 30)' in spell_ui
           and 'angdroid_utf8_display_columns(spell_name_buf)' in spell_ui,
           "Spell menu name display needs CJK terminal-column clipping")
    expect('angdroid_zh_tw_spell_name(spell->name, true)' in player_spell,
           "Studied spell message should use localized spell name")
    expect('test_spell_names_locale.c' in native_tests,
           "Spell-name C99 test missing from native tests")
    title_keys = {en for en, _ in re.findall(
        r'^\s*\{\s*"([^"]+)",\s*"([^"]+)"\s*\},\s*$',
        class_titles, re.MULTILINE)}
    canonical_titles = {line.partition(":")[2]
                        for line in class_titles_data.splitlines()
                        if line.startswith("title:")}
    expect(title_keys == canonical_titles,
           "Class progression title translations must cover canonical class.txt")
    expect('angdroid_zh_tw_class_title(show_title()' in player_ui,
           "Character sheet must translate progression titles at render time")
    expect('char value[96];' in player_ui,
           "Character sheet title buffer must hold multibyte Chinese text")
    expect('test_class_titles_locale.c' in native_tests,
           "Class progression title regression not registered")
    expect('angdroid_game_ui_text("Command: Sidebar Mode"' in options_ui,
           "Sidebar option screen title not localized")
    expect('strnfmt(save_label, sizeof(save_label), "將%s選項設為預設值"' in options_ui
           and '"Save as default %s options"' in options_ui,
           "Localized option saving must retain original literal format string")
    expect('format("目前生命值警告：%d（%d%%）"' in options_ui
           and 'format("Current hitpoint warning: %d (%d%%)"' in options_ui,
           "Health warning must preserve escaped percent format string")
    expect('zh_names[SIDEBAR_MODE % SIDEBAR_MAX]' in options_ui,
           "Sidebar mode names are not translated at display time")
    expect('msg("%s", angdroid_game_ui_text("Visual attr/char tables reset."' in options_ui,
           "Native options translation must use a literal printf format")
    expect(not re.search(r"\bmsg\(angdroid_game_ui_text\(", options_ui),
           "Unsafe dynamic message format in native options")
    expect('angdroid_game_ui_text(act->name, angdroid_zh_tw_enabled())' in menu_ui,
           "Native action menu needs display-only Chinese labels")
    expect('acts[oid].action(acts[oid].name, m->cursor)' in menu_ui,
           "Native action callback must retain original command label")
    expect('angdroid_game_ui_text(' in options_ui
           and '"To edit visuals, use the knowledge menu"' in options_ui,
           "Visuals menu header is untranslated")
    expect('menu_put_clipped(col, row, wid, color,' in menu_ui
           and 'angdroid_menu_copy_columns(rendered, sizeof(rendered), text,' in menu_ui
           and 'Term_putstr(col, row, -1, color, rendered)' in menu_ui,
           "Translated action menus must clip by rendered terminal columns")
    # Target selection helper localizes labels while preserving color-coded keys.
    help_source = source(CPP / "angband/zh-tw-target-help.h", errors)
    help_part = target_ui.partition("void target_display_help(")[2].partition(
        "static bool is_running_keymap(")[0]
    help_pairs = re.findall(
        r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}',
        help_source)
    expect(len(help_pairs) == 16 and
           len({en for en, _ in help_pairs}) == 16,
           "Target help must have 16 unique translation fragments")
    expect('"zh-tw-target-help.h"' in target_ui and
           'bool zh_tw = angdroid_zh_tw_enabled();' in help_part and
           help_part.count('text_out("%s", angdroid_target_help_text(') == 18 and
           'text_out(angdroid_target_help_text(' not in target_ui,
           "NDK format-security: all 18 targeting help fragments need literal %s")
    expect('text_out_c(COLOUR_L_GREEN, "%s", label);' in help_part and
           all('text_out_c(COLOUR_L_GREEN, "' + k + '");' in help_part
               for k in ("<dir>", "<click>", "g", "p", "q", "r",
                         "m", "+", "-", "o", "t", ">", "<", "x")),
           "Localized target help must preserve colored command keycaps")
    expect("test_target_help_locale.c" in native_tests,
           "Target help C99 fallback test is not registered")

    # Regression for Android Clang -Wformat -Werror=format-security:
    # text_out() is printf-like and must receive a literal format.
    expect('text_out(angdroid_store_ui_text(' not in store_ui,
           "NDK format-security: dynamic store text must be a %s argument")
    expect(store_ui.count('text_out("%s", angdroid_store_ui_text(') == 18,
           "All 18 store help fragments must use a literal %s format")
    
    # Targeted store help translation; keycaps and transactions remain canonical.
    store_text_header = source(CPP / "angband/zh-tw-store-text.h", errors)
    store_help = store_ui.partition("static void store_display_help(")[2].partition(
        "static void store_redraw(")[0]
    store_labels = re.findall(
        r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}',
        store_text_header)
    expect(len(store_labels) == 18 and
           len({en for en, _ in store_labels}) == 18,
           "Shop/home dictionary must hold 18 unique labels")
    expect('"zh-tw-store-text.h"' in store_ui and
           'bool zh_tw = angdroid_zh_tw_enabled();' in store_help and
           store_help.count('text_out("%s", angdroid_store_ui_text(') == 18 and
           'text_out(angdroid_store_ui_text(' not in store_ui,
           "Shop/home instructions must all use locale-aware labels")
    expect(all('text_out_c(COLOUR_L_GREEN, "' + key + '");' in store_help
               for key in ("x", "l", "p", "g", "d", "s", "I", "ESC")),
           "Shop/home help must preserve every color-coded shortcut")
    expect(store_ui.count('format("價格：%ld", (long)price)') == 2 and
           store_ui.count('format("Price: %ld", (long)price)') == 2 and
           'format("購買 %s？%s [Esc/N 取消，其他鍵確認]"' in store_ui and
           'format("Buy %s?%s %s"' in store_ui and
           'format("%s %s？[Esc/N 取消，其他鍵確認]"' in store_ui and
           'format("%s %s? [ESC, any other key to accept]"' in store_ui,
           "Shop prompts must preserve literal format strings and English fallback")
    expect("menu_dynamic_select_alias(m, 'g', 'p')" in store_ui and
           'cmdq_push(CMD_BUY)' in store_ui and 'cmdq_push(CMD_SELL)' in store_ui,
           "Store command dispatch changed")
    expect('test_store_text_locale.c' in native_tests,
           "Shop C99 translation regression is not registered")

    # Canonical English shopkeeper greeting templates, placeholders and locale.
    welcome_header = source(CPP / "angband/zh-tw-store-greetings.h", errors)
    welcome_block = store_ui.partition("static const char *comment_welcome[] =")[2].partition("};")[0]
    original_greetings = re.findall(
        r'^\s*"((?:\\.|[^"\\])*)",?\s*$', welcome_block, re.MULTILINE)
    localized_greetings = re.findall(
        r'^\s*\{\s*"((?:\\.|[^"\\])*)",\s*"((?:\\.|[^"\\])*)"\s*\},?\s*$',
        welcome_header, re.MULTILINE)
    expect(len(original_greetings) == 10 and
           original_greetings[0] == "" and
           len(localized_greetings) == 9 and
           len({en for en, _ in localized_greetings}) == 9 and
           set(original_greetings[1:]) == {en for en, _ in localized_greetings},
           "Chinese shopkeeper greeting dictionary must match all source formats")
    expect(all(en.count("%s") == zh.count("%s") and
               "%" not in en.replace("%s", "") and
               "%" not in zh.replace("%s", "")
               for en, zh in localized_greetings),
           "Translated shopkeeper greetings must preserve %s argument counts")
    expect('"zh-tw-store-greetings.h"' in store_ui and
           '"zh-tw-class-titles.h"' in store_ui and
           'angdroid_store_greeting_format(comment_welcome[i],' in store_ui and
           'angdroid_zh_tw_class_title(' in store_ui and
           'angdroid_zh_tw_enabled() ? "貴賓" : "valued customer"' in store_ui,
           "Shopkeeper greeting language selection and class title are not wired")
    expect('msg(comment_hint[i], random_hint());' in store_ui and
           'i = MIN(i, N_ELEMENTS(comment_welcome) - 1);' in store_ui and
           'for (j = 0; j < (int)sizeof(short_name) - 1 &&' in store_ui,
           "Shopkeeper hint selection, bounds or owner-name protection changed")
    expect('test_store_greetings_locale.c' in native_tests,
           "C99 test for shopkeeper greetings missing from native suite")

    # Curated hints must match the unmodified 99-entry canonical corpus.
    hints_data = source(ROOT / "variant_lib/angband/gamedata/hints.txt", errors)
    hints_header = source(CPP / "angband/zh-tw-store-hints.h", errors)
    canonical_hints = {
        line[2:] for line in hints_data.splitlines() if line.startswith("H:")
    }
    hint_pairs = re.findall(
        r'^\s*\{\s*("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\s*\},?\s*$',
        hints_header, re.MULTILINE)
    try:
        import json
        parsed_hints = [(json.loads(en), json.loads(zh))
                        for en, zh in hint_pairs]
    except (ValueError, TypeError):
        parsed_hints = []
    expect(len(canonical_hints) == 99 and len(parsed_hints) == 99 and
           len({en for en, _ in parsed_hints}) == 99 and
           {en for en, _ in parsed_hints} == canonical_hints and
           all(zh and zh != en for en, zh in parsed_hints),
           "All 99 shopkeeper hint translations must match canonical hints.txt")
    expect('"zh-tw-store-hints.h"' in store_ui and
           'msg("「%s」", angdroid_store_hint_text(random_hint(), true))' in store_ui and
           'msg(comment_hint[i], random_hint());' in store_ui,
           "Shopkeeper hints must translate only Chinese display strings")
    expect('test_store_hints_locale.c' in native_tests,
           "Curated shopkeeper hint C99 regression must be registered")

    # Native built-in help-browser labels are display-only; shortcut dispatch is canonical.
    help_browser = source(CPP / "angband/ui-help.c", errors)
    help_header = source(CPP / "angband/zh-tw-help-browser.h", errors)
    help_labels = re.findall(
        r'^\s*\{\s*"([^"]+)",\s*"([^"]+)"\s*\},?\s*$',
        help_header, re.MULTILINE)
    expect(len(help_labels) == 7 and len({en for en, _ in help_labels}) == 7,
           "Native help-browser UI must translate 7 distinct labels")
    expect('"zh-tw-help-browser.h"' in help_browser and
           'bool zh_tw = angdroid_zh_tw_enabled();' in help_browser and
           help_browser.count("prt(angdroid_help_browser_text(") == 7,
           "Native help browser labels must be displayed through the locale lookup")
    expect('strnfmt(caption, sizeof(caption), "說明檔案「%s」", name);' in help_browser and
           'strnfmt(caption, sizeof(caption), "Help file \'%s\'", name);' in help_browser and
           'msg("無法開啟「%s」。", name);' in help_browser and
           'msg("Cannot open \'%s\'.", name);' in help_browser and
           'format("[%s，%s，行 %d-%d/%d]"' in help_browser and
           'format("[%s, %s, Line %d-%d/%d]"' in help_browser,
           "Native help dynamic captions require literal printf formats and fallback")
    expect(all(x in help_browser for x in (
            "if (ch.code == '?') break;", "if (ch.code == '!')",
            "if (ch.code == '&')", "if (ch.code == '/')",
            "if (ch.code == '#')", "if (ch.code == '%')",
            "if (ch.code == ESCAPE) break;")),
           "Native help key command protocol changed")
    expect("test_help_browser_locale.c" in native_tests,
           "Native help-browser C99 fallback tests not registered")

    # Knowledge navigation, detail titles and status labels must stay display-only.
    knowledge_locale_tests = source(ROOT / "tests/native/test_game_ui_locale.c", errors)
    knowledge_mapping_keys = [
        "Display object knowledge", "Display rune knowledge",
        "Display artifact knowledge", "Display ego item knowledge",
        "Display monster knowledge", "Display feature knowledge",
        "Display trap knowledge", "Display shapechange effects",
        "Display hall of fame", "Display character history",
        "Display equippable comparison", "Display current knowledge",
        "monsters", "artifacts", "ego items", "known objects",
        "features", "traps", "Yes", "shape", " dead", "alive", "Inscribed"
    ]
    for label in knowledge_mapping_keys:
        expect('{' + '"' + label + '", ' in game_ui_header,
               "Missing localized knowledge menu/display key: " + label)
        expect('angdroid_game_ui_text("' + label + '", 1)' in
               knowledge_locale_tests,
               "Knowledge C99 translation coverage missing: " + label)
    expect('angdroid_game_ui_text(title,' in knowledge_ui and
           'prt(format("知識圖鑑：%s", display_title)' in knowledge_ui and
           'format("尚未發現任何%s。", display_title)' in knowledge_ui,
           "Knowledge list captions must use localized display-only titles")
    expect('main_knowledge_menu.m.title = angdroid_game_ui_text(' in knowledge_ui and
           'main_knowledge_menu.actions[i].name =' in knowledge_ui and
           'pre_store_actions[j].action' in knowledge_ui and
           'post_store_actions[j].action' in knowledge_ui,
           "Knowledge action labels must not replace callbacks or command IDs")
    expect('angdroid_game_ui_text("Yes",' in knowledge_ui and
           'angdroid_game_ui_text("shape",' in knowledge_ui and
           'angdroid_game_ui_text((race->max_num == 0) ? " dead" : "alive"' in knowledge_ui,
           "Knowledge monster/object statuses must use locale-aware text")
    expect('strnfmt(buf, sizeof(buf), "符文（%d 個尚未辨識）"' in knowledge_ui and
           'strnfmt(buf, sizeof(buf), "runes (%d unknown)"' in knowledge_ui and
           'char buf[64];' in knowledge_ui and
           'strnfmt(title, sizeof(title), "神器（種子 %08lx）"' in knowledge_ui and
           'strnfmt(title, sizeof(title), "artifacts (seed %08lx)"' in knowledge_ui,
           "Knowledge runes/artifacts must keep literal formats and enough UTF-8 space")

    # Symbol lookup monster bases: match canonical base-name + source desc.
    base_data = source(ROOT / "variant_lib/angband/gamedata/monster_base.txt", errors)
    symbol_header = source(CPP / "angband/zh-tw-symbol-query.h", errors)
    source_bases, current_base, current_desc = [], None, ""
    for line in base_data.splitlines():
        if line.startswith("name:"):
            if current_base is not None:
                source_bases.append((current_base, current_desc))
            current_base, current_desc = line[5:], ""
        elif current_base is not None and line.startswith("desc:"):
            current_desc += line[5:]
    if current_base is not None:
        source_bases.append((current_base, current_desc))
    symbol_bases = re.findall(
        r'^\s*\{ "([^"]+)", "([^"]+)", "([^"]+)" \},?$',
        symbol_header, re.MULTILINE)
    expect(len(source_bases) == 56 and len(symbol_bases) == 56 and
           [(name, desc) for name, desc, _ in symbol_bases] == source_bases and
           len({name for name, _ in source_bases}) == 56 and
           all(desc and zh for _, desc, zh in symbol_bases),
           "Symbol monster-base labels must match all 56 canonical records")
    expect('"zh-tw-symbol-query.h"' in knowledge_ui and
           "angdroid_symbol_monster_base_text(" in knowledge_ui and
           'if (streq(race->name, "Morgoth")) continue;' in knowledge_ui and
           'angdroid_symbol_query_format(buf, max, sym, race->text,' in knowledge_ui and
           "test_symbol_monster_base_locale.c" in native_tests and
           "test_symbol_query_localization.py" in source(
               ROOT / "scripts/verify-zh-tw.sh", errors),
           "Symbol query must keep display-only monster base lookup and exclusions")

    # Symbol identification: display-only translated text, original keymap.
    symbol_translation_keys = [
        "Enter character to be identified, or control+[ANU]: ",
        "Full monster list.", "Unique monster list.",
        "Non-unique monster list.", " [(r)ecall, ESC]"
    ]
    for label in symbol_translation_keys:
        expect('{"' + label + '", ' in game_ui_header,
               "Missing zh-TW symbol-inspection text: " + label)
    expect('if (!get_com(angdroid_game_ui_text(' in knowledge_ui and
           'my_strcpy(buf, angdroid_game_ui_text("Full monster list."' in knowledge_ui and
           'my_strcpy(buf, angdroid_game_ui_text("Unique monster list."' in knowledge_ui and
           'my_strcpy(buf, angdroid_game_ui_text("Non-unique monster list."' in knowledge_ui and
           'textblock_append(tb, "%s", angdroid_game_ui_text(' in knowledge_ui,
           "Symbol query prompts must use Chinese display-only mappings")
    expect('strnfmt(buf, max, "%c - 未知符號。", sym)' in knowledge_ui and
           'strnfmt(buf, max, "%c - Unknown Symbol.", sym)' in knowledge_ui and
           'strnfmt(buf, max, "? - 未知符號。")' in knowledge_ui and
           'strnfmt(buf, max, "? - Unknown Symbol.")' in knowledge_ui,
           "Symbol lookup needs safe literal formats in both languages")
    expect(all(x in knowledge_ui for x in (
        "if (sym == KTRL('A'))", "sym == KTRL('U')",
        "sym == KTRL('N')", "query.code == 'k'",
        "query.code == 'y' || query.code == 'p'",
        "query.code != 'r'", "query.code == ESCAPE")),
        "Symbol query game commands must remain unchanged")

    # All knowledge group categories must match canonical source keys.
    knowledge_group_header = source(CPP / "angband/zh-tw-knowledge-groups.h", errors)
    monster_groups_data = source(ROOT / "variant_lib/angband/gamedata/ui_knowledge.txt", errors)
    knowledge_group_tests = source(ROOT / "tests/native/test_knowledge_groups_locale.c", errors)
    category_source = set(re.findall(
        r'^monster-category:(.+)$', monster_groups_data, re.MULTILINE))
    group_order_block = knowledge_ui.partition(
        "static const grouper object_text_order[] =")[2].partition("};")[0]
    item_categories = set(re.findall(
        r'\{\s*TV_\w+,\s*"([^"]+)"\s*\}', group_order_block))
    category_source.add("***Unclassified***")
    category_source.update(item_categories)
    for name in ("rune_group_text", "feature_group_text", "trap_group_text"):
        group_block = knowledge_ui.partition(
            "static const char *" + name + "[] =")[2].partition("};")[0]
        category_source.update(re.findall(r'"([^"]+)"', group_block))
    group_pairs = re.findall(
        r'^\s*\{\s*"([^"]+)",\s*"([^"]+)"\s*\},?\s*$',
        knowledge_group_header, re.MULTILINE)
    expect(len(category_source) == 97 and len(group_pairs) == 97 and
           len({en for en, _ in group_pairs}) == 97 and
           {en for en, _ in group_pairs} == category_source and
           all(zh for _, zh in group_pairs),
           "Knowledge group display translations must cover 97 canonical labels")
    expect('"zh-tw-knowledge-groups.h"' in knowledge_ui and
           'g_names[i] = angdroid_knowledge_group_text(' in knowledge_ui and
           'g_funcs.name(g_list[i]), angdroid_zh_tw_enabled());' in knowledge_ui and
           'len = angdroid_utf8_display_columns(g_names[i]);' in knowledge_ui and
           'g_name_len = len > 20 ? 20 : (int)len;' in knowledge_ui,
           "Knowledge groups must use the display-only translation and cell widths")
    expect("menu_put_clipped(col, row, width, color, items[oid]);" in menu_ui and
           'menu_init(&group_menu, MN_SKIN_SCROLL, menu_find_iter(MN_ITER_STRINGS));' in knowledge_ui,
           "Knowledge menu text must still clip to the group column width")
    expect('test_knowledge_groups_locale.c' in native_tests and
           'angdroid_utf8_display_columns("龍類") == 4' in knowledge_group_tests and
           all(('angdroid_knowledge_group_text("' + en + '", 1)') in
               knowledge_group_tests for en in category_source),
           "Native C99 group-name regression must cover all Chinese categories")

    # Ego dictionary names are displayed only, with CJK-safe clipping in
    # the knowledge listing and English fallback in the detailed header.
    ego_knowledge_test = source(
        ROOT / "tests/test_ego_knowledge_screen.py", errors)
    expect('"zh-tw-ego-names.h"' in knowledge_ui
           and 'angdroid_zh_tw_ego_name(ego->name)' in knowledge_ui
           and 'const char *display_name = localized ? localized : ego->name;'
               in knowledge_ui
           and 'display_name, col, Term->wid, Term->wid' in knowledge_ui
           and 'c_prt(attr, clipped, row, col);' in knowledge_ui
           and 'textui_textblock_show(tb, area, format("%s %s", group, display_name));'
               in knowledge_ui
           and 'angdroid_knowledge_group_text(' in knowledge_ui
           and "test_ego_knowledge_screen.py" in source(
                ROOT / "scripts/verify-zh-tw.sh", errors)
           and "test_recall_title_uses_same_dictionary_and_group_fallback"
               in ego_knowledge_test,
           "Ego knowledge needs localized suffixes with safe column clipping")
    # Monster knowledge uses 522 regular and 101 unique labels with CJK clipping.
    monster_label_header = source(CPP / "angband/knowledge-monster-label.h", errors)
    monster_label_test = source(ROOT / "tests/native/test_knowledge_monster_label.c", errors)
    expect('"knowledge-monster-label.h"' in knowledge_ui and
           'angdroid_knowledge_monster_copy(name_label, sizeof(name_label),' in knowledge_ui and
           'race->name, rf_has(race->flags, RF_UNIQUE),' in knowledge_ui and
           'int columns = MIN(65 - col, Term->wid - col);' in knowledge_ui and
           'if (name_label[0])' in knowledge_ui and
           'c_put_str(attr, name_label, row, col);' in knowledge_ui,
           "Monster knowledge names must be localized and clipped before symbol")
    expect('"zh-tw-monsters.h"' in monster_label_header and
           '"zh-tw-unique-monsters.h"' in monster_label_header and
           '"menu-text-fit.h"' in monster_label_header and
           'if (!english || !zh_tw) return english;' in monster_label_header and
           'angdroid_zh_tw_unique_monster_name(english)' in monster_label_header and
           'return translated ? translated : english;' in monster_label_header and
           'angdroid_menu_copy_columns(dest, capacity, label,' in monster_label_header,
           "Monster knowledge must translate unique/regular names with English fallback")
    expect('test_knowledge_monster_label.c' in native_tests and
           'strcmp(label, "遠古紅")' in monster_label_test and
           'strcmp(label, "遠古")' in monster_label_test and
           'strcmp(label, "kobo")' in monster_label_test,
           "Monster knowledge renderer must test UTF-8 boundaries and English")

    # Item and rune names already have canonical translators; prevent
    # CJK glyph splitting or spilling across fixed knowledge columns.
    knowledge_column = source(CPP / "angband/knowledge-column-text.h", errors)
    knowledge_column_test = source(
        ROOT / "tests/native/test_knowledge_column_text.c", errors)
    expect('"knowledge-column-text.h"' in knowledge_ui and
           '"menu-text-fit.h"' in knowledge_column and
           'angdroid_menu_copy_columns(dest, capacity, label, available)' in
           knowledge_column and
           'angdroid_knowledge_tried_suffix(' in knowledge_ui and
           'char o_name[160] = "";' in knowledge_ui,
           "Knowledge display must preserve item translations and tried suffix")
    # Rune names now get a display-only zh-TW title before clipping;
    # verify the title path and existing metadata column boundaries.
    expect('o_name, col, 45, Term->wid' in knowledge_ui and
           'inscrip, 55, 75, Term->wid' in knowledge_ui and
           'const char *display_name = angdroid_zh_tw_rune_title(' in knowledge_ui and
           'rune_name(oid), angdroid_zh_tw_enabled(),' in knowledge_ui and
           'display_name, col, 46, Term->wid' in knowledge_ui and
           'inscrip, 47, Term->wid, Term->wid' in knowledge_ui,
           "Knowledge item/rune entries must stop before metadata columns")
    expect(all(x in knowledge_ui for x in (
           'streq(otherfields, "Ignore  Inscribed          Sym")',
           'streq(otherfields, "                   Sym  Kills")',
           'streq(otherfields, "                    Sym")',
           'streq(otherfields, "Inscribed")',
           'knowledge_header_field("符號", 76)',
           'knowledge_header_field("刻字", 46)')),
           "Knowledge column headings must localize only known literal headers")
    expect('test_knowledge_column_text.c' in native_tests and
           'strcmp(out, "治療藥")' in knowledge_column_test and
           'strcmp(out, "Powe")' in knowledge_column_test,
           "Knowledge UTF-8 column C99 regression missing")

    # Knowledge footer labels must preserve all original keyboard actions.
    knowledge_footer_text = [
        ", 'c' to copy", ", 'c', 'p' to paste",
        ", ENTER to accept", ", 'i' to insert, ENTER to accept",
        ", 'v' for visuals", ", 'r' to recall",
        ", 's' to toggle ignore, 'r'ecall, '{'",
        ", 's' to toggle ignore, 'r'ecall, '{', '}'",
        ", 'r'ecall, '{'", ", 'r'ecall, '{', '}'",
        ", 't/T' for lighting (lit)",
        ", 't/T' for lighting (torch)",
        ", 't/T' for lighting (LOS)",
        ", 't/T' for lighting (dark)",
        ", 't/T' to cycle lighting"
    ]
    knowledge_footer_tests = source(
        ROOT / "tests/native/test_game_ui_locale.c", errors)
    for label in knowledge_footer_text:
        expect('{' + '"' + label + '", ' in game_ui_header and
               'angdroid_game_ui_text("' + label + '", 1)' in
               knowledge_footer_tests,
               "Knowledge footer translation/test missing: " + label)
    expect('if (angdroid_zh_tw_enabled()) {' in knowledge_ui and
           '"方向鍵移動%s%s%s，Esc 返回"' in knowledge_ui and
           'angdroid_game_ui_text(pvs, true)' in knowledge_ui and
           'angdroid_game_ui_text(pedit, true)' in knowledge_ui and
           'angdroid_game_ui_text(xtra, true)' in knowledge_ui and
           'angdroid_menu_copy_columns(visible_prompt,' in knowledge_ui and
           'prt(format("<dir>%s%s%s, ESC", pvs, pedit, xtra)' in knowledge_ui,
           "Knowledge footer must keep English mode and safely clip Chinese")
    expect(knowledge_ui.count('soft_kbd_flash("tT");') == 2 and
           'return ", \'t/T\' to cycle lighting";' in knowledge_ui and
           'soft_kbd_flash("lL");' not in knowledge_ui and
           'return ", \'l\' to cycle lighting";' not in knowledge_ui and
           knowledge_ui.count("if (ch.code == 't')") >= 2 and
           knowledge_ui.count("} else if (ch.code == 'T')") >= 2,
           "Feature/trap lighting prompts must match actual t/T commands")
    expect('if (strstr(pedit, "paste") != NULL)' in knowledge_ui and
           'if (sel == with_insc) soft_kbd_flash("}");' in knowledge_ui,
           "Translated footer must not alter soft keyboard command detection")

    # Android NDK flagged three monster-lore dangling else warnings.
    # Guard the outer visibility/ability flags from ambiguous locale else.
    monster_lore_special = monster_lore.partition(
        "/* Describe special things. Keep the outer flag check distinct")[2].partition(
        "/* Describe light */")[0]
    expect(all(('if (rf_has(known_flags, RF_' + flag + ')) {') in
               monster_lore_special for flag in
               ("UNAWARE", "MULTIPLY", "REGENERATE")) and
           monster_lore_special.count("if (angdroid_zh_tw_enabled())") == 3 and
           monster_lore_special.count("\n\t\telse\n") == 3,
           "Monster lore zh-TW choice must use unambiguous outer braces")

    # Rune display translations must not mutate canonical rune mechanics.
    rune_lore_header = source(CPP / "angband/zh-tw-rune-lore.h", errors)
    rune_lore_tests = source(ROOT / "tests/native/test_rune_lore_locale.c", errors)
    expect('"zh-tw-rune-lore.h"' in knowledge_ui and
           'display_name = angdroid_zh_tw_rune_title(' in knowledge_ui and
           'rune_name(oid), angdroid_zh_tw_enabled()' in knowledge_ui and
           'textblock_append(tb, "%s", angdroid_zh_tw_rune_description(' in knowledge_ui and
           'rune_desc(oid), angdroid_zh_tw_enabled()' in knowledge_ui,
           "Rune knowledge must use display-only translated name and lore")
    expect('angdroid_zh_tw_object_term(term, 1)' in rune_lore_header and
           'return strcmp(mapped, term) == 0 ? NULL : mapped;' in
           rune_lore_header and
           'return english;' in rune_lore_header and
           'snprintf(out, capacity, "%s抗性", mapped)' in rune_lore_header and
           'snprintf(out, capacity, "剋制%s", mapped)' in rune_lore_header,
           "Rune names must preserve unknown English properties and safe formats")
    expect('test_rune_lore_locale.c' in native_tests and
           'title("resist fire", "火焰抗性")' in rune_lore_tests and
           'detail("Object gives the player a magical bonus to strength."' in
           rune_lore_tests and
           'title("mysterious curse", "mysterious curse")' in rune_lore_tests,
           "Rune locale test must exercise examples and unknown fallbacks")

    # Canonical terrain and trap names/descriptions remain in gamedata;
    # only the knowledge browser display layer may localize these strings.
    terrain_file = source(ROOT / "variant_lib/angband/gamedata/terrain.txt", errors)
    trap_file = source(ROOT / "variant_lib/angband/gamedata/trap.txt", errors)
    terrain_trap_header = source(CPP / "angband/zh-tw-terrain-trap.h", errors)
    terrain_trap_test = source(ROOT / "tests/native/test_terrain_trap_locale.c", errors)
    import json
    canonical_terrain = {
        line[5:] for line in terrain_file.splitlines() if line.startswith("name:")
    }
    canonical_trap = {
        line.split(":", 2)[2] for line in trap_file.splitlines()
        if line.startswith("name:") and line.count(":") >= 2
    }
    canonical_trap_lore = {
        line[5:] for line in trap_file.splitlines() if line.startswith("desc:")
    }
    def terrain_table_pairs(name):
        block = terrain_trap_header.partition(
            "static const struct angdroid_terrain_trap_pair " + name + "[] = {"
        )[2].partition("};")[0]
        raw = re.findall(
            r'^\s*\{\s*("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\s*\},?\s*$',
            block, re.MULTILINE)
        try:
            return [(json.loads(en), json.loads(zh)) for en, zh in raw]
        except ValueError:
            return []
    for table_name, expected in (
        ("terrain_names", canonical_terrain),
        ("terrain_lore", canonical_terrain - {"unknown grid"}),
        ("trap_names", canonical_trap),
        ("trap_lore", canonical_trap_lore)):
        pairs = terrain_table_pairs(table_name)
        expect(len(pairs) == len(expected) and
               {en for en, _ in pairs} == expected and
               len({en for en, _ in pairs}) == len(pairs) and
               all(zh.strip() for _, zh in pairs),
               "Terrain/trap table must match all canonical source records: " + table_name)
    expect('"zh-tw-terrain-trap.h"' in knowledge_ui and
           'angdroid_terrain_name(feat->name, angdroid_zh_tw_enabled())' in knowledge_ui and
           'angdroid_terrain_lore(' in knowledge_ui and
           'angdroid_trap_name(trap->desc, angdroid_zh_tw_enabled())' in knowledge_ui and
           'angdroid_trap_lore(' in knowledge_ui and
           'col, 64, Term->wid' in knowledge_ui,
           "Knowledge terrain/traps must be translated only when displaying")
    expect('test_terrain_trap_locale.c' in native_tests and
           'angdroid_trap_name("fire trap", 1)' in terrain_trap_test and
           'angdroid_terrain_name("General Store", 1)' in terrain_trap_test,
           "Terrain/trap bilingual C99 regression not registered")

    knowledge_source = source(CPP / "angband/ui-knowledge.c", errors)
    expect('angdroid_zh_tw_enabled() ? "神器" : "artifacts"' in knowledge_source and
           'strnfmt(title, sizeof(title), "神器（種子 %08lx）"' in knowledge_source and
           'strnfmt(title, sizeof(title), "artifacts (seed %08lx)"' in knowledge_source,
           "Known artifacts title must be localized for both randart and normal modes")

    # Fixed action names and store entries in the main knowledge menu are
    # translated for presentation without touching shop indices or digit keys.
    knowledge_menu_header = source(CPP / "angband/zh-tw-knowledge-menu.h", errors)
    knowledge_menu_test = source(ROOT / "tests/native/test_knowledge_store_menu_locale.c", errors)
    expect('"zh-tw-knowledge-menu.h"' in knowledge_ui and
           'pre_store_actions[j].label, angdroid_zh_tw_enabled()' in knowledge_ui and
           'post_store_actions[j].label, angdroid_zh_tw_enabled()' in knowledge_ui and
           'angdroid_knowledge_store_label(localized,' in knowledge_ui and
           'main_knowledge_menu.m.cmd_keys = main_knowledge_menu.storekeys;' in knowledge_ui and
           'handle_store_shortcuts' in knowledge_ui,
           "Knowledge menu labels must be translated without altering store keys")
    expect('angdroid_terrain_name(name, 1)' in knowledge_menu_header and
           "name[n - 1] == 's'" in knowledge_menu_header and
           'return NULL;' in knowledge_menu_header and
           'test_knowledge_store_menu_locale.c' in native_tests and
           'store("Weapon Smiths", 3, " (3)", 0,' in knowledge_menu_test and
           'store(NULL, 10, "", 1,' in knowledge_menu_test,
           "Knowledge store label test must cover both locales and unknown names")

    # Shape names come exclusively from canonical shape.txt; displayed
    # translations never replace shape IDs, effect data or original sort keys.
    shape_data = source(ROOT / "variant_lib/angband/gamedata/shape.txt", errors)
    shape_header = source(CPP / "angband/zh-tw-shape-lore.h", errors)
    shape_test = source(ROOT / "tests/native/test_shape_lore_locale.c", errors)
    canonical_shape_names = {
        line[5:] for line in shape_data.splitlines() if line.startswith("name:")
    }
    shape_map = shape_header.partition(
        "static inline const char *angdroid_shape_name(")[2].partition(
        "static inline const char *angdroid_shape_class(")[0]
    shape_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', shape_map)
    expect(len(shape_pairs) == len(canonical_shape_names) and
           {en for en, _ in shape_pairs} == canonical_shape_names and
           all(zh.strip() for _, zh in shape_pairs),
           "Shape label dictionary must match all canonical shape.txt names")
    expect('"zh-tw-shape-lore.h"' in knowledge_ui and
           'angdroid_shape_name(sarray[i]->name, angdroid_zh_tw_enabled())' in knowledge_ui and
           'angdroid_shape_intro(angdroid_zh_tw_enabled())' in knowledge_ui and
           'angdroid_zh_tw_spell_name(spell->name, true)' in knowledge_ui and
           'angdroid_shape_class(c->name, true)' in knowledge_ui and
           'return my_stricmp((*sleft)->name, (*sright)->name);' in knowledge_ui and
           'effect_describe(s->effect, "Changing into the shape "' in knowledge_ui and
           'effect_describe_shape_zh(s->effect)' in knowledge_ui,
           "Shape lore must retain identity, sorting and generic effect fallback")
    expect('test_shape_lore_locale.c' in native_tests and
           'shape("Pukel-man", "普克爾人")' in shape_test and
           'angdroid_shape_intro(0)' in shape_test,
           "Shape lore C99 fallback regression must be registered")

    expect(knowledge_ui.count('textblock_append(tb, "%s", angdroid_zh_tw_enabled() ? "。\\n" : ".\\n");') == 9 and
           'textblock_append(tb, angdroid_zh_tw_enabled() ?' not in knowledge_ui,
           "Shape lore localized punctuation must use a literal printf format")

    # Only exact shape.txt misc flag and ROCK player ability descriptions
    # are localized. All unrecognized descriptions retain their source text.
    shape_misc_source = source(ROOT / "variant_lib/angband/gamedata/object_property.txt", errors)
    shape_player_source = source(ROOT / "variant_lib/angband/gamedata/player_property.txt", errors)
    exact_shape_desc = (
        "Feather Falling", "Prevents paralysis", "Speeds regeneration",
        "Sustains your life force", "Grants the ability to see invisible things",
        "Gives immunity to traps")
    expect(all("desc:" + desc in shape_misc_source for desc in exact_shape_desc) and
           "desc:You are made of rock." in shape_player_source and
           all('{ "' + desc + '", "' in shape_header for desc in
               exact_shape_desc + ("You are made of rock.",)),
           "Shape misc descriptions must use exact canonical English keys")
    expect('angdroid_shape_misc_desc(prop->desc,' in knowledge_ui and
           'angdroid_shape_misc_desc(ability->desc,' in knowledge_ui and
           'strcmp(localized, prop->desc) != 0' in knowledge_ui and
           'Custom flag.' in shape_test,
           "Only known shape misc descriptions may be changed at display time")

    # Character history and hall-of-fame names are UI-only; exported history,
    # persistent score records, original paging keys and rank calculations stay.
    history_ui = source(CPP / "angband/ui-history.c", errors)
    score_ui = source(CPP / "angband/ui-score.c", errors)
    records_header = source(CPP / "angband/zh-tw-records.h", errors)
    records_test = source(ROOT / "tests/native/test_history_score_locale.c", errors)
    race_data = source(ROOT / "variant_lib/angband/gamedata/p_race.txt", errors)
    record_class_data = source(ROOT / "variant_lib/angband/gamedata/class.txt", errors)
    def record_name_map(begin, finish):
        chunk = records_header.partition(begin)[2].partition(finish)[0]
        return re.findall(r'\{ "([^"]+)", "([^"]+)" \}', chunk)
    record_races = record_name_map(
        "static inline const char *angdroid_record_race(",
        "static inline const char *angdroid_record_class(")
    record_classes = record_name_map(
        "static inline const char *angdroid_record_class(", "#endif")
    expect(len(record_races) == 11 and
           {x for x, _ in record_races} ==
           {s[5:] for s in race_data.splitlines() if s.startswith("name:")} and
           len(record_classes) == 9 and
           {x for x, _ in record_classes} ==
           {s[5:] for s in record_class_data.splitlines() if s.startswith("name:")} and
           all(zh.strip() for _, zh in record_races + record_classes),
           "Historical record display dictionary must cover all original races/classes")
    exported_history = history_ui.partition("void dump_history(")[2]
    expect('"zh-tw-records.h"' in history_ui and
           'angdroid_record_ui_text("[Player history]"' in history_ui and
           'angdroid_record_ui_text(" (LOST)"' in history_ui and
           'file_putf(file, "[Player history]\\n");' in exported_history and
           'file_putf(file, "      Turn   Depth  Note\\n");' in exported_history and
           'my_strcat(buf, " (LOST)", sizeof(buf));' in exported_history and
           "case 'n':" in history_ui and "case 'p':" in history_ui,
           "History display locale must not modify text exports or paging keys")
    expect('"zh-tw-records.h"' in score_ui and
           'angdroid_record_race(r ? r->name : "<none>", true)' in score_ui and
           'angdroid_record_class(c ? c->name : "<none>", true)' in score_ui and
           'score_print_row(attr, out_val,' in score_ui and
           'angdroid_menu_copy_columns(clipped, sizeof(clipped), line, available);' in score_ui and
           '"Killed by %s on dungeon level %d"' in score_ui and
           '"%s Hall of Fame"' in score_ui and
           'if (ch.code == ESCAPE)' in score_ui,
           "High score screen must preserve English fallback and original controls")
    expect('test_history_score_locale.c' in native_tests and
           'verify(angdroid_record_race, "Dunadan", "杜內丹人")' in records_test and
           'verify(angdroid_record_class, "Blackguard", "黑衛士")' in records_test and
           'verify(angdroid_record_ui_text, "Unused label", "Unused label")' in records_test,
           "History/highscore C99 bilingual regression must be registered")

    # Equipment comparison text is display-only: keyboard and mouse action
    # identifiers remain unchanged while both help screens and status lines
    # are localized in zh-TW. Unknown dynamic status text is kept in English.
    cmp_ui = source(CPP / "angband/ui-equip-cmp.c", errors)
    cmp_header = source(CPP / "angband/zh-tw-equip-cmp.h", errors)
    cmp_test = source(ROOT / "tests/native/test_equip_cmp_locale.c", errors)
    cmp_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', cmp_header)
    expect(len(cmp_pairs) == 49 and
           len({x for x, _ in cmp_pairs}) == len(cmp_pairs) and
           all(x.strip() and y.strip() for x, y in cmp_pairs),
           "Equipment comparison text dictionary must have unique nonempty labels")
    expect('angdroid_equip_cmp_text(english, angdroid_zh_tw_enabled())' in
               cmp_ui.replace("\n", " ") or
           'angdroid_equip_cmp_text(english, angdroid_zh_tw_enabled())' in
               cmp_ui,
           "Equipment comparison help must use locale-aware display helper")
    expect('equip_cmp_print_label(states[istate].prompt,' in cmp_ui and
           'equip_cmp_print_label(the_summary->dlg_trans_msg,' in cmp_ui and
           'angdroid_equip_cmp_text("Enter 2 or 3 ' in cmp_ui and
           "case 'q':" in cmp_ui and "case '!':" in cmp_ui and
           "case 'c':" in cmp_ui and "case 'r':" in cmp_ui and
           "case 'R':" in cmp_ui,
           "Equipment comparison input shortcuts and dynamic prompts must remain")
    expect('menu_dynamic_add_label(m, angdroid_equip_cmp_text(' in cmp_ui and
           'ACT_CTX_EQUIPCMP_DUMP_FILE' in cmp_ui and
           'ACT_CTX_EQUIPCMP_RESET' in cmp_ui and
           'angdroid_menu_copy_columns(clipped, sizeof(clipped),' in cmp_ui and
           'test_equip_cmp_locale.c' in native_tests and
           'check("Skip selection", "略過選取");' in cmp_test and
           'Unsupported future label' in cmp_test,
           "Equipment comparison mouse labels and C99 fallback test must remain")

    # Curse selection: preserve canonical curse.txt IDs, selection indices,
    # strengths and original English unknown fallback (source-matched 27/27).
    curse_ui = source(CPP / "angband/ui-curse.c", errors)
    curse_header = source(CPP / "angband/zh-tw-curse.h", errors)
    curse_test = source(ROOT / "tests/native/test_curse_locale.c", errors)
    curse_data = source(ROOT / "variant_lib/angband/gamedata/curse.txt", errors)
    curse_def_names = [line[5:] for line in curse_data.splitlines()
                       if line.startswith("name:")]
    curse_def_desc = [line[5:] for line in curse_data.splitlines()
                      if line.startswith("desc:")]
    def curse_table_keys(start, finish):
        fragment = curse_header.partition(start)[2].partition(finish)[0]
        return re.findall(r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}', fragment)
    curse_names = curse_table_keys(
        "static inline const char *angdroid_curse_name(",
        "static inline const char *angdroid_curse_description(")
    curse_descs = curse_table_keys(
        "static inline const char *angdroid_curse_description(",
        "static inline void angdroid_curse_row(")
    expect(len(curse_names) == 27 and len(curse_descs) == 27 and
           [en for en, _ in curse_names] == curse_def_names and
           [en for en, _ in curse_descs] == curse_def_desc and
           len({en for en, _ in curse_names}) == 27 and
           all(zh.strip() for _, zh in curse_names + curse_descs),
           "Curse display dictionary must match 27 original curse.txt records")
    expect('"zh-tw-curse.h"' in curse_ui and
           'angdroid_curse_row(buf, sizeof(buf), name, power,' in curse_ui and
           'angdroid_curse_description(original,' in curse_ui and
           'selection = choice[oid].index;' in curse_ui and
           'player_knows_curse(player, i)' in curse_ui and
           'obj->curses[i].power' in curse_ui and
           'text_out(" %s。\\n", localized);' in curse_ui and
           'text_out(" %s.\\n", buf);' in curse_ui,
           "Curse menu localization must not change lore gating or selection")
    expect('angdroid_menu_copy_columns(clipped, sizeof(clipped), buf,' in curse_ui and
           'angdroid_utf8_display_columns(m->header)' in curse_ui and
           'test_curse_locale.c' in native_tests and
           'unknown curse' in curse_test and
           'angdroid_curse_row(&sentinel, 0' in curse_test,
           "Curse CJK columns and bilingual fallback regression must remain")

    # The native race/class ability menu only translates the text rendered
    # from player_property.txt. All 44 source names/descriptions remain intact.
    ability_ui = source(CPP / "angband/ui-player-properties.c", errors)
    ability_header = source(CPP / "angband/zh-tw-player-ability.h", errors)
    ability_test = source(ROOT / "tests/native/test_player_ability_locale.c", errors)
    property_source = source(ROOT / "variant_lib/angband/gamedata/player_property.txt", errors)
    property_entries = []
    for line in property_source.splitlines():
        if line.startswith("type:"):
            property_entries.append({"name": None, "desc": []})
        elif property_entries and line.startswith("name:"):
            property_entries[-1]["name"] = line[5:]
        elif property_entries and line.startswith("desc:"):
            property_entries[-1]["desc"].append(line[5:])
    def property_locale_keys(begin, finish):
        fragment = ability_header.partition(begin)[2].partition(finish)[0]
        return re.findall(r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}', fragment)
    ability_names = property_locale_keys(
        "static inline const char *angdroid_ability_name(",
        "static inline const char *angdroid_ability_description(")
    ability_descs = property_locale_keys(
        "static inline const char *angdroid_ability_description(", "#endif")
    expect(len(property_entries) == 44 and
           len(ability_names) == len(ability_descs) == 44 and
           [en for en, _ in ability_names] ==
           [entry["name"] for entry in property_entries] and
           [en for en, _ in ability_descs] ==
           ["".join(entry["desc"]) for entry in property_entries] and
           all(zh.strip() for _, zh in ability_names + ability_descs),
           "Native player ability translation must exactly match 44 source records")
    expect('"zh-tw-player-ability.h"' in ability_ui and
           'angdroid_ability_name(choices[oid].name,' in ability_ui and
           'angdroid_ability_description(choices[oid].desc,' in ability_ui and
           'angdroid_menu_copy_columns(clipped, sizeof(clipped), buf,' in ability_ui and
           'return all_letters_nohjkl[oid];' in ability_ui and
           'menu_setpriv(&menu, num_abilities, ability_list);' in ability_ui and
           'text_out_c(COLOUR_L_BLUE, "\\n%s\\n",' in ability_ui and
           'test_player_ability_locale.c' in native_tests and
           'Custom Ability' in ability_test,
           "Player ability render-only locale must preserve keys and ids")

    # Shape effects use the original effect and dice engine; only known
    # descriptions receive zh-TW labels, while unknown stacks return NULL.
    effect_engine = source(CPP / "angband/effects-info.c", errors)
    effect_engine_header = source(CPP / "angband/effects-info.h", errors)
    effect_header = source(CPP / "angband/zh-tw-shape-effects.h", errors)
    effect_locale_test = source(ROOT / "tests/native/test_shape_effect_locale.c", errors)
    source_effects = source(CPP / "angband/list-effects.h", errors)
    timed_source = source(ROOT / "variant_lib/angband/gamedata/player_timed.txt", errors)
    projection_source = source(ROOT / "variant_lib/angband/gamedata/projection.txt", errors)
    canonical_terms = ("poisoning", "stunning", "fear", "berserk rage",
                       "temporary hitpoint siphoning", "causes monsters to flee")
    effect_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', effect_header)
    expect(len(effect_pairs) == 6 and
           {en for en, _ in effect_pairs} == set(canonical_terms) and
           all(zh.strip() for _, zh in effect_pairs) and
           all("desc:" + en in timed_source or
               "desc:" + en in projection_source for en in canonical_terms),
           "Shape effect translations must match source timed/projection texts")
    shape_effects = "\n".join(line for line in shape_data.splitlines()
           if line.startswith("effect:"))
    expect(all(("EFFECT(" + name + "," in source_effects) for name in
               ("CURE", "DAMAGE", "TIMED_INC", "PROJECT_LOS")) and
           all(line.split(":")[1] in
               ("CURE", "DAMAGE", "TIMED_INC", "PROJECT_LOS") for line in
               shape_effects.splitlines()) and
           'effect_describe_shape_zh(' in effect_engine_header and
           'textblock *effect_describe_shape_zh(' in effect_engine,
           "Shape effect helper must cover canonical types and expose interface")
    shape_renderer = effect_engine.partition(
        "textblock *effect_describe_shape_zh(")[2].partition(
        "size_t effect_get_menu_name(")[0]
    expect('return NULL;' in shape_renderer and
           'effect_desc(current)' in shape_renderer and
           'dice_roll(current->dice, &value)' in shape_renderer and
           'format_dice_string(&value, 1,' in shape_renderer and
           'copy_to_textblock_with_coloring(tb, dice_string)' in shape_renderer and
           'angdroid_shape_effect_known(original)' in shape_renderer and
           'textblock_append(tb, "變身時，");' in shape_renderer and
           'textblock_append(tb, "%s", localized)' in shape_renderer and
           'textblock_append(tb, localized)' not in shape_renderer and
           'textblock_append(tb, "%s", translated ? "。\\n" : ".\\n");' in
                knowledge_ui and
           'test_shape_effect_locale.c' in native_tests and
           'tests/test_shape_effect_render_source.py' in source(ROOT / "scripts/verify-zh-tw.sh", errors) and
           'new future effect' in effect_locale_test,
           "Shape effect formatting and unknown fallback must preserve values")

    # In-game timed effects: presentation-only, exact canonical notification
    # messages from ALL original status groups. IDs, grade names, conditions,
    # numeric ranges and parsed source data must stay in English.
    timed_display = source(CPP / "angband/player-timed.c", errors)
    timed_header = source(CPP / "angband/zh-tw-timed-messages.h", errors)
    timed_test = source(ROOT / "tests/native/test_timed_messages_locale.c", errors)
    timed_text = source(ROOT / "variant_lib/angband/gamedata/player_timed.txt", errors)
    timed_names = tuple(line[5:] for line in timed_text.splitlines()
                        if line.startswith("name:"))
    parsed_timed = {}
    current = None
    for line in timed_text.splitlines():
        if line.startswith("name:"):
            current = line[5:]
            if current in timed_names:
                parsed_timed[current] = []
        if current not in timed_names:
            continue
        if line.startswith("grade:"):
            parts = line.split(":")
            parsed_timed[current].extend(
                msg for msg in parts[4:6] if msg.strip())
        elif line.startswith(("on-end:", "on-increase:", "on-decrease:")):
            msg = line.partition(":")[2]
            if msg.strip():
                parsed_timed[current].append(msg)

    timed_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', timed_header)
    timed_source_keys = [s for values in parsed_timed.values() for s in values]
    expect(len(timed_pairs) == 149 and
           len(timed_source_keys) == 151 and
           len(set(timed_source_keys)) == 148 and
           len(parsed_timed) == 53 and
           len({en for en, _ in timed_pairs}) == 149 and
           {en for en, _ in timed_pairs} ==
                set(timed_source_keys + ["You resist the effect!"]) and
           all(zh.strip() and "%" not in zh and
               re.findall(r"\{([a-z]+)\}", zh) ==
                   (["kind"] if "{kind}" in en else [])
               for en, zh in timed_pairs),
           "All 53 timed-status groups must match canonical data and preserve object-kind placeholders")
    expect('"zh-tw-timed-messages.h"' in timed_display and
           '"droid.h"' in timed_display and
           all('angdroid_timed_message(' + key + ', angdroid_zh_tw_enabled())'
               in timed_display for key in
               ("new_grade->up_msg", "new_grade->down_msg", "effect->on_end",
                "effect->on_decrease", "effect->on_increase")) and
           'print_custom_message(weapon,' in timed_display and
           'msg("%s", angdroid_timed_message(' in timed_display and
           'player_timed_grade_eq(' in timed_display and
           'timed_effects[idx].grade' in timed_display,
           "Timed events must translate at display time without changing status logic")
    expect('test_timed_messages_locale.c' in native_tests and
           'verify("You are poisoned!", "你中毒了！");' in timed_test and
           'verify("You resist the effect!", "你抵抗了這次效果！");' in timed_test and
           'A brand-new timed effect!' in timed_test and
           'Your {name} glow{s}!' in timed_test and
           'Your {kind} start{s} to drip with acid!' in timed_test and
           'Your {kind} {is} covered in frost!' in timed_test and
           'verify("Your limbs feel more energetic!", "你的四肢變得更加靈活有力！");' in timed_test,
           "Timed status C99 test must cover all groups and dynamic object-kind tags")

    # Gameplay projection feedback and monster attacks are translated only
    # at display sinks, preserving the original damage and status calculations.
    projection_player = source(CPP / "angband/project-player.c", errors)
    projection_monster = source(CPP / "angband/mon-attack.c", errors)
    projection_header = source(CPP / "angband/zh-tw-projection-feedback.h", errors)
    projection_test = source(ROOT / "tests/native/test_projection_feedback_locale.c", errors)
    projection_data = source(ROOT / "variant_lib/angband/gamedata/projection.txt", errors)
    function_sections = (
        ("angdroid_project_feedback(", 19),
        ("angdroid_project_stat(", 5),
        ("angdroid_project_blind_term(", 12),
    )
    for title, count in function_sections:
        section = projection_header.partition(
            "static inline const char *" + title)[2].partition(
            "return english;\n}")[0]
        matches = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', section)
        expect(len(matches) == count and
               len({en for en, _ in matches}) == count and
               all(zh.strip() and "%" not in zh for _, zh in matches),
               "Projection dictionary source entry mismatch: " + title)
    blind_keys = {x[11:] for x in projection_data.splitlines()
                  if x.startswith("blind-desc:")}
    blind_map = projection_header.partition(
        "static inline const char *angdroid_project_blind_term(")[2]
    expect(len(blind_keys) == 12 and
           all('{ "' + x + '", "' in blind_map for x in blind_keys),
           "Blind projection names must match every original source term")
    expect('"zh-tw-projection-feedback.h"' in projection_player and
           projection_player.count('msg("%s", angdroid_project_feedback(') >= 19 and
           'angdroid_project_stat(act, true)' in projection_player and
           'angdroid_project_blind_term(projections[typ].blind_desc, true)'
                in projection_player and
           'msg("你受到 %d 點傷害。", reduced);' in projection_player and
           'msg("你額外受到 %d 點傷害。", xtra);' in projection_player and
           'msg("You take %d damage.", reduced);' in projection_player and
           'msg("You take an extra %d damage.", xtra);' in projection_player,
           "Projection gameplay sinks must use safe locale formatting")
    expect('"droid.h"' in projection_monster and
           all(format_string in projection_monster for format_string in
               ('%s 試圖施法，但失敗了。', '%s 被擊退了。',
                '%s 沒有擊中你。', '%s 沒有擊中 %s。',
                '錯誤：找不到 %s 的效果處理程序。')) and
           'monster_spell_failrate(mon)' in projection_monster and
           'chance_of_monster_hit(mon, effect)' in projection_monster,
           "Monster attack translation must preserve the attack decision logic")
    expect('test_projection_feedback_locale.c' in native_tests and
           'verify(angdroid_project_stat, "strong", "力量");' in projection_test and
           'verify(angdroid_project_blind_term, "something", "不明力量");' in
               projection_test,
           "Projection bilingual host test must be registered")

    # Attack effect messages and all original projection descriptions are
    # translated at display-only sites. Calculation and monster handlers stay.
    attack_effect_ui = source(CPP / "angband/effect-handler-attack.c", errors)
    attack_feedback_header = source(CPP / "angband/zh-tw-attack-feedback.h", errors)
    projection_terms_header = source(CPP / "angband/zh-tw-projection-terms.h", errors)
    attack_feedback_test = source(ROOT / "tests/native/test_attack_feedback_locale.c", errors)
    source_projection_terms = source(
        ROOT / "variant_lib/angband/gamedata/projection.txt", errors)
    projection_descs = {x[5:] for x in source_projection_terms.splitlines()
                        if x.startswith("desc:")}
    attack_strings = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', attack_feedback_header)
    projection_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', projection_terms_header)
    expect(len(attack_strings) == 22 and
           len({en for en, _ in attack_strings}) == 22 and
           len(projection_pairs) == 55 and
           {en for en, _ in projection_pairs} == projection_descs and
           all(zh.strip() and "%" not in zh for _, zh in
               attack_strings + projection_pairs),
           "Attack feedback/project names must cover all original source keys")
    expect('"zh-tw-attack-feedback.h"' in attack_effect_ui and
           '"zh-tw-projection-terms.h"' in attack_effect_ui and
           'angdroid_attack_feedback(hurt_msg,' in attack_effect_ui and
           'angdroid_projection_description(projections[type].desc, true)'
                in attack_effect_ui and
           'msg("你受到 %d 點傷害。", dam);' in attack_effect_ui and
           'msg("你從 %s 身上汲取力量。", m_name);' in attack_effect_ui and
           'msg("You draw power from the %s.", m_name);' in attack_effect_ui and
           'msgt(MSG_GENERIC, "目標距離太遠（%d）。", taim);' in attack_effect_ui and
           'effect_calculate_value(context,' in attack_effect_ui and
           'take_hit(player, dam, killer);' in attack_effect_ui,
           "Effect-handler message locale must preserve original combat logic")
    expect('test_attack_feedback_locale.c' in native_tests and
           'verify(angdroid_projection_description, "chaos", "混沌");' in
               attack_feedback_test and
           'verify(angdroid_attack_feedback, "You feel better.", "你感覺好多了。");'
                in attack_feedback_test,
           "Attack feedback bilingual C99 regression must be registered")

    # All 247 source monster spell messages (209 distinct) and 49 original
    # race-specific override occurrences (17 distinct) are display-only.
    monster_spell_ui = source(CPP / "angband/mon-spell.c", errors)
    monster_spell_template_header = source(
        CPP / "angband/zh-tw-mon-spell-text.h", errors)
    monster_spell_test = source(
        ROOT / "tests/native/test_mon_spell_message_locale.c", errors)
    monster_spell_data = source(
        ROOT / "variant_lib/angband/gamedata/monster_spell.txt", errors)
    monster_alt_data = source(
        ROOT / "variant_lib/angband/gamedata/monster.txt", errors)
    monster_projection_data = source(
        ROOT / "variant_lib/angband/gamedata/projection.txt", errors)
    spell_originals = [line.partition(":")[2]
        for line in monster_spell_data.splitlines()
        if line.startswith(("message-vis:", "message-invis:",
                            "message-miss:", "message-save:"))]
    alt_originals = [line.split(":", 2)[2]
        for line in monster_alt_data.splitlines()
        if line.startswith(("message-vis:", "message-invis:",
                            "message-miss:")) and line.count(":") >= 2]
    source_keys = set(spell_originals + alt_originals)
    spell_fragment_files = (
        "zh-tw-mon-spell-0.inc", "zh-tw-mon-spell-1.inc",
        "zh-tw-mon-spell-2.inc", "zh-tw-mon-spell-3.inc",
        "zh-tw-mon-spell-alt.inc")
    source_parts = [source(CPP / "angband" / fname, errors)
                    for fname in spell_fragment_files]
    spell_fragment_pairs = [
        re.findall(r'\{ "([^"]+)", "([^"]+)" \}', part)
        for part in source_parts]
    spell_pairs = [pair for frag in spell_fragment_pairs for pair in frag]
    expect(len(spell_originals) == 247 and
           len(set(spell_originals)) == 209 and
           len(alt_originals) == 49 and
           len(set(alt_originals)) == 17 and
           [len(x) for x in spell_fragment_pairs] == [55, 55, 55, 44, 17] and
           len(spell_pairs) == len(source_keys) == 226 and
           {en for en, _ in spell_pairs} == source_keys and
           all(zh.strip() and "%" not in zh for _, zh in spell_pairs),
           "Monster spell bilingual source dictionary must cover every key")
    preserved_tags = ("name", "target", "type", "oftype")
    expect(all(
        all(("{"+ tag +"}" in en) == ("{"+ tag +"}" in zh)
            for tag in preserved_tags) and
        "{pronoun}" not in zh
        for en, zh in spell_pairs),
        "Localized spell messages must preserve dynamic name/target/type tags")
    lash_text = source(CPP / "angband/zh-tw-mon-spell-lash.h", errors)
    lash_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', lash_text)
    canonical_lashes = {line[10:] for line in
        monster_projection_data.splitlines() if line.startswith("lash-desc:")}
    expect(len(lash_pairs) == 23 and
           {en for en, _ in lash_pairs} == canonical_lashes and
           all(zh.strip() for _, zh in lash_pairs),
           "Monster whip and spit lash types must match canonical projections")
    expect('"zh-tw-mon-spell-text.h"' in monster_spell_ui and
           '"zh-tw-mon-spell-lash.h"' in monster_spell_ui and
           'in_cursor = angdroid_mon_spell_text(in_cursor,' in
                monster_spell_ui and
           'angdroid_mon_spell_text(level->save_message,' in monster_spell_ui and
           'angdroid_mon_spell_lash(type_name, true)' in monster_spell_ui and
           'angdroid_zh_tw_enabled() ? "你" : "you"' in monster_spell_ui and
           'msgt(spell->msgt, "%s", buf);' in monster_spell_ui and
           'spell_check_for_fail_rune(spell);' in monster_spell_ui,
           "Monster spell display hook must preserve IDs and original combat engine")
    expect(all('#include "' + f + '"' in monster_spell_template_header
               for f in spell_fragment_files) and
           'test_mon_spell_message_locale.c' in native_tests and
           'sizeof(cases) / sizeof(cases[0]) == 226' in monster_spell_test and
           'A new monster spell.' in monster_spell_test,
           "Monster spell locale integration must register independent C99 test")

    # Core player warnings, death messages and command prompts remain
    # presentation-only; savefile causes and user notes are unchanged.
    player_util_i18n = source(CPP / "angband/player-util.c", errors)
    misc_i18n = source(CPP / "angband/cmd-misc.c", errors)
    player_feedback = source(CPP / "angband/zh-tw-player-feedback.h", errors)
    player_feedback_test = source(ROOT / "tests/native/test_player_feedback_locale.c", errors)
    terrain_for_player = source(ROOT / "variant_lib/angband/gamedata/terrain.txt", errors)
    player_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', player_feedback)
    expect(len(player_pairs) == 39 and
           len({en for en, _ in player_pairs}) == 39 and
           'hurt-msg:The lava burns you!' in terrain_for_player and
           '{ "The lava burns you!", "熔岩灼傷了你！" }' in player_feedback and
           all(zh.strip() and "%" not in zh for _, zh in player_pairs),
           "Native player feedback must retain all exact original keys")
    expect('"zh-tw-player-feedback.h"' in player_util_i18n and
           '"zh-tw-shape-lore.h"' in player_util_i18n and
           'angdroid_shape_name(p->shape->name, true)' in player_util_i18n and
           'angdroid_player_feedback(square_feat(' not in player_util_i18n and
           'square_feat(cave, grid)->hurt_msg, angdroid_zh_tw_enabled()'
                in player_util_i18n and
           'get_check(angdroid_player_feedback("Die? ",' in player_util_i18n and
           'msg("你突然痛得大叫！%s", dam_text);' in player_util_i18n and
           'msgt(MSG_DEATH, "%s", angdroid_player_feedback(' in player_util_i18n and
           'msgt(MSG_HITPOINT_WARN, "%s", angdroid_player_feedback(' in player_util_i18n and
           'my_strcpy(p->died_from, kb_str, sizeof(p->died_from));' in player_util_i18n and
           'p->chp -= dam;' in player_util_i18n and
           'take_hit(p, dam, "over-exertion");' in player_util_i18n,
           "Player event display must retain death, damage and savefile state")
    expect('"zh-tw-player-feedback.h"' in misc_i18n and
           'history_add(player, note, HIST_USER_INPUT);' in misc_i18n and
           'player->noscore |= NOSCORE_WIZARD;' in misc_i18n and
           'player->noscore |= NOSCORE_BORG;' in misc_i18n and
           'msg("筆記：%s", tmp);' in misc_i18n and
           'msg("%s", &note[3]);' in misc_i18n and
           'get_string(angdroid_player_feedback("Note: ",' in misc_i18n,
           "Notes and no-score commands retain original history and flags")
    expect('test_player_feedback_locale.c' in native_tests and
           'check("You die.", "你死了。");' in player_feedback_test and
           'unknown future prompt' in player_feedback_test,
           "Player feedback native bilingual C99 test must be registered")

    # Generic spell handlers: localize exact static gameplay messages only;
    # formatted dynamic object and monster outcomes remain untouched here.
    general_effects = source(CPP / "angband/effect-handler-general.c", errors)
    general_effect_header = source(
        CPP / "angband/zh-tw-general-effect-messages.h", errors)
    general_effect_test = source(
        ROOT / "tests/native/test_general_effect_messages_locale.c", errors)
    general_effect_pairs = re.findall(
        r'\{ "([^"]+)", "([^"]+)" \}', general_effect_header)
    general_effect_test_pairs = re.findall(
        r'\{ "([^"]+)", "([^"]+)" \}', general_effect_test)
    expect(len(general_effect_pairs) == 71 and
           len({en for en, _ in general_effect_pairs}) == 71 and
           all(zh.strip() and "%" not in zh for _, zh in general_effect_pairs) and
           general_effect_test_pairs == general_effect_pairs,
           "Generic spell feedback must have 71 canonical English keys and tests")
    expect('"zh-tw-general-effect-messages.h"' in general_effects and
           general_effects.count('msg("%s", angdroid_general_effect_message(')
                == 78 and
           general_effects.count('"%s", angdroid_general_effect_message(')
                == 82 and
           'msgt(MSG_TPLEVEL, "%s", angdroid_general_effect_message(' in
                general_effects and
           'angdroid_zh_tw_enabled()' in general_effects and
           'effect_calculate_value(context,' in general_effects,
           "All 82 original fixed-format spell feedback sinks must be localized")
    expect('test_general_effect_messages_locale.c' in native_tests and
           'Nothing happens!' in general_effect_test and
           'Teleportation forbidden!' in general_effect_test,
           "Generic effect host C99 regression must be registered")

    # Formatted generic effect feedback: preserve original numeric values,
    # monster/item names, original stat/brand identities and game logic.
    formatted_effects = source(CPP / "angband/effect-handler-general.c", errors)
    formatted_header = source(
        CPP / "angband/zh-tw-general-effect-terms.h", errors)
    formatted_test = source(
        ROOT / "tests/native/test_general_effect_terms_locale.c", errors)
    canonical_obj_property = source(
        ROOT / "variant_lib/angband/gamedata/object_property.txt", errors)
    formatted_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', formatted_header)
    stat_properties = [en for en, _ in formatted_pairs[:10]]
    expect(len(formatted_pairs) == 15 and
           len({en for en, _ in formatted_pairs}) == 15 and
           all(any(key + en in canonical_obj_property
                   for key in ("adjective:", "neg-adjective:"))
               for en in stat_properties),
           "Formatted effect stat adjectives must match object_property.txt")
    expect('"zh-tw-general-effect-terms.h"' in formatted_effects and
           '"zh-tw-shape-lore.h"' in formatted_effects and
           'angdroid_general_brand_name(name, true)' in formatted_effects and
           'angdroid_general_stat_adjective(desc_stat(' in formatted_effects and
           'angdroid_general_item_type(item, true)' in formatted_effects and
           'angdroid_shape_name(shape->name, true)' in formatted_effects and
           'msg("%s 有 %d 點生命值。", m_name, mon->hp);' in formatted_effects and
           'msg("你受到 %d 點傷害。\\n", dam);' in formatted_effects and
           'msgt(MSG_DRAIN_STAT, "你感覺自己非常%s。%s",' in formatted_effects and
           'msg("Your %s (%c) %s disenchanted!", o_name,' in formatted_effects and
           'effect_calculate_value(context,' in formatted_effects and
           'player_stat_dec(player,' in formatted_effects,
           "Formatted effect localization must not change game calculations")
    expect('test_general_effect_terms_locale.c' in native_tests and
           'verify(angdroid_general_brand_name, "Flame", "火焰");' in
                formatted_test and
           'verify(angdroid_general_stat_adjective, "weak", "虛弱");' in
                formatted_test,
           "Formatted native spell locale tests must be registered")


    # Monster terrain messages and quest victory: display-only translation.
    movement_ui = source(CPP / "angband/mon-move.c", errors)
    quest_ui = source(CPP / "angband/player-quest.c", errors)
    movement_locale = source(CPP / "angband/zh-tw-move-quest.h", errors)
    movement_test = source(ROOT / "tests/native/test_move_quest_locale.c", errors)
    terrain_data = source(ROOT / "variant_lib/angband/gamedata/terrain.txt", errors)
    move_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', movement_locale)
    terrain_phrases = {line.partition(":")[2] for line in
                       terrain_data.splitlines()
                       if line.startswith("confused-msg:")}
    expect(len(move_pairs) == 12 and
           len({en for en, _ in move_pairs}) == 12 and
           terrain_phrases.issubset({en for en, _ in move_pairs}) and
           all(zh.strip() and "%" not in zh for _, zh in move_pairs),
           "Movement and quest dictionaries must match original terrain keys")
    expect('"zh-tw-move-quest.h"' in movement_ui and
           '"zh-tw-move-quest.h"' in quest_ui and
           'angdroid_move_quest_text(m ? m : "stumbles", true)' in movement_ui and
           'msg("%s%s。", m_name,' in movement_ui and
           'msg("%s %s.", m_name, m ? m : "stumbles")' in movement_ui and
           'angdroid_move_quest_text(' in quest_ui and
           'square_set_feat(cave, grid, FEAT_MORE);' in quest_ui and
           'p->total_winner = true;' in quest_ui,
           "Movement and quest original game behavior must remain")
    expect('test_move_quest_locale.c' in native_tests and
           'test("bashes into a wall", "撞上了牆壁");' in movement_test and
           'test("*** CONGRATULATIONS ***", "＊＊＊ 恭喜你 ＊＊＊");' in movement_test,
           "Movement and quest regression must remain registered")

    # Monster theft, mimic revelation and allocation diagnostics are
    # translated at the output only; inventory/loot logic remains canonical.
    monster_util_ui = source(CPP / "angband/mon-util.c", errors)
    monster_make_ui = source(CPP / "angband/mon-make.c", errors)
    monster_event_dict = source(CPP / "angband/zh-tw-monster-events.h", errors)
    monster_event_test = source(ROOT / "tests/native/test_monster_events_locale.c", errors)
    monster_event_pairs = re.findall(
        r'\{ "([^"]+)", "([^"]+)" \}', monster_event_dict)
    expect(len(monster_event_pairs) == 5 and
           len({en for en, _ in monster_event_pairs}) == 5 and
           all(zh.strip() and "%" not in zh for _, zh in monster_event_pairs),
           "Monster allocation/theft messages must have safe unique translations")
    expect('"zh-tw-monster-events.h"' in monster_util_ui and
           '"zh-tw-monster-events.h"' in monster_make_ui and
           'angdroid_monster_event("treasure", angdroid_zh_tw_enabled())' in
                monster_util_ui and
           'angdroid_monster_event("You vanish into the shadows!",' in
                monster_util_ui and
           'msg("你沒能從%s身上偷到%s。", m_name, o_name)' in monster_util_ui and
           'msg("You fail to steal %s from %s.", o_name, m_name)' in
                monster_util_ui and
           'effect_simple(EF_TELEPORT,' in monster_util_ui and
           'player->au += obj->pval;' in monster_util_ui and
           'get_mon_num_prep(' in monster_make_ui and
           'angdroid_monster_event("Too many monsters!",' in monster_make_ui,
           "Monster theft and spawning behavior must remain untouched")
    expect('test_monster_events_locale.c' in native_tests and
           'angdroid_monster_event("treasure", 1)' in monster_event_test and
           'future event' in monster_event_test,
           "Monster event C99 regression must be registered")

    # Character sheet display-only translation. Export remains canonical.
    sheet_ui = source(CPP / "angband/ui-player.c", errors)
    sheet_header = source(CPP / "angband/zh-tw-character-sheet.h", errors)
    sheet_test = source(ROOT / "tests/native/test_character_sheet_locale.c", errors)
    sheet_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', sheet_header)
    expect(len(sheet_pairs) == 37 and
           len({en for en, _ in sheet_pairs}) == 37 and
           all(zh.strip() for _, zh in sheet_pairs),
           "Character sheet display dictionary must have 37 unique labels")
    expect('"zh-tw-character-sheet.h"' in sheet_ui and
           'angdroid_character_sheet_text("Armor"' in sheet_ui and
           'angdroid_character_sheet_text("Saving Throw"' in sheet_ui and
           'angdroid_character_sheet_text("Very Bad"' in sheet_ui and
           'angdroid_character_sheet_text("Normal"' in sheet_ui and
           'angdroid_character_sheet_text("Town"' in sheet_ui and
           'angdroid_character_sheet_text("Character dump successful."' in sheet_ui and
           'panel_line(p, COLOUR_L_BLUE, "Blows", "%d.%d/turn"' in sheet_ui and
           'panel_line(p, COLOUR_L_BLUE, "射擊次數", "%d.%d/回合"' in sheet_ui,
           "Character sheet must keep fixed numeric formats and locale labels")
    sheet_export = sheet_ui.partition("void write_character_dump(")[2].partition(
        "bool dump_save(")[0]
    expect('file_putf(fff, "  [%s Character Dump]\\n\\n", buildid);' in sheet_export and
           'file_putf(fff, "  [Last Messages]\\n\\n");' in sheet_export and
           'file_putf(fff, "  [Character Equipment]\\n\\n");' in sheet_export and
           'file_putf(fff, "  [Options]\\n\\n");' in sheet_export,
           "Canonical exported dump headings must remain intact")
    expect('test_character_sheet_locale.c' in native_tests and
           'pair("Legendary", "傳奇");' in sheet_test and
           'pair("Character dump failed!", "角色資料匯出失敗！");' in sheet_test,
           "C99 character sheet regression must remain registered")

    # Inventory feedback must keep original item accounting, labels and
    # grammar in English while rendering zh-TW messages separately.
    gear_ui = source(CPP / "angband/obj-gear.c", errors)
    gear_h = source(CPP / "angband/zh-tw-gear-messages.h", errors)
    gear_t = source(ROOT / "tests/native/test_gear_messages_locale.c", errors)
    gear_pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', gear_h)
    expect(len(gear_pairs) == 4 and
           len({x for x, _ in gear_pairs}) == 4,
           "Four canonical inventory status labels must remain unique")
    expect('"zh-tw-gear-messages.h"' in gear_ui and
           '"droid.h"' in gear_ui and
           'msg("你還有 %d 次充能可用。", obj->pval);' in gear_ui and
           'msg("You have %d charge%s remaining."' in gear_ui and
           'msgt(MSG_WIELD, "你穿戴了 %s（%c）。"' in gear_ui and
           'msgt(MSG_WIELD, fmt, o_name, gear_to_label(player, wielded));'
                in gear_ui and
           'msgt(MSG_WIELD, "你卸下了 %s（%c）。"' in gear_ui and
           'gear_to_label(player, obj)' in gear_ui and
           'combine_pack(player)' in gear_ui,
           "Inventory localization must preserve gear identity and calculations")
    expect('test_gear_messages_locale.c' in native_tests and
           'Mushrooms for breakfast!' in gear_t and
           'Other gear message' in gear_t and
           'angdroid_gear_message(NULL, 1) == NULL' in gear_t,
           "Inventory bilingual regression must be registered")

    # Object command feedback is localized without editing item IDs,
    # artifact objects or original English message text.
    obj_command = source(CPP / "angband/cmd-obj.c", errors)
    expect('format("真的要卸下 %s 嗎？ ", o_name)' in obj_command and
           'format("Really take off %s? ", o_name)' in obj_command and
           'msgt(MSG_WIELD, "你卸下了 %s（%c）。"' in obj_command and
           'msgt(MSG_WIELD, "%s %s (%c).", act, o_name,' in obj_command and
           'gear_to_label(player, equip_obj)' in obj_command and
           'msg("你看見 %s。", name);' in obj_command and
           'msg("You see %s.", name);' in obj_command and
           'msg("你持有 %s（第一組 %c）。", name, label);' in obj_command and
           'msg("You have %s (1st %c).", name, label);' in obj_command and
           'msg("你持有 %s（%c）。", name, label);' in obj_command and
           'msg("You have %s (%c).", name, label);' in obj_command and
           '"Error in do_cmd_wield(), please report."' in obj_command and
           '裝備替換時發生錯誤，請回報此問題。' in obj_command,
           "Object use/replace feedback must preserve English fallback")

    if errors:
        for item in errors:
            print("FAIL:", item)
        return 1
    print(f"PASS: {checks} native game-localization source safeguards")
    print("  Monster list entries: 522 | item kind labels: 326")
    print("  Death menu labels: 9 | death prompts: 11")
    print("NOTE: Requires Android NDK and on-device zh-TW gameplay validation")
    return 0


if __name__ == "__main__":
    sys.exit(main())
