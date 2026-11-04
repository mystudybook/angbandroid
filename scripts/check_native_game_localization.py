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

    expect("!rf_has(race->flags, RF_UNIQUE)" in mon,
           "Unique monster names must remain in their original display form")
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
    expect("MIN(menu_dynamic_longest_entry(m) + 3 + 2" in menu_ui,
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
    expect('format("知識圖鑑：%s", title)' in knowledge_ui,
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
    expect("#define ANGDROID_ZH_TW_MONSTER_LORE 59" in monster_lore_dict,
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
    expect("get_monster_message_zh_tw(msg->msg_code)" in mon_msg_ui
           and "get_subject_zh_tw(subject_zh, sizeof(subject_zh)" in mon_msg_ui,
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
           help_part.count("text_out(angdroid_target_help_text(") == 18,
           "Target help text outputs must all use the selected locale")
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

    # Monster knowledge rows reuse the 522-name curated translation and cell clip.
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
           '"menu-text-fit.h"' in monster_label_header and
           'if (!english || !zh_tw || is_unique) return english;' in monster_label_header and
           'return translated ? translated : english;' in monster_label_header and
           'angdroid_menu_copy_columns(dest, capacity, label,' in monster_label_header,
           "Monster knowledge must preserve unique/English names and UTF-8 glyphs")
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
    expect('o_name, col, 45, Term->wid' in knowledge_ui and
           'inscrip, 55, 75, Term->wid' in knowledge_ui and
           'rune_name(oid), col, 46, Term->wid' in knowledge_ui and
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
