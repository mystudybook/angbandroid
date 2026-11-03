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
    menu_ui = source(CPP / "angband/ui-menu.c", errors)
    context_ui = source(CPP / "angband/ui-context.c", errors)
    game_ui = source(CPP / "angband/ui-game.c", errors)
    obj_cmd = source(CPP / "angband/cmd-obj.c", errors)
    obj_info = source(CPP / "angband/obj-info.c", errors)
    item_info_terms = source(CPP / "angband/zh-tw-item-info.h", errors)
    spell_ui = source(CPP / "angband/ui-spell.c", errors)
    magic_terms = source(CPP / "angband/zh-tw-magic.h", errors)
    player_spell = source(CPP / "angband/player-spell.c", errors)
    cave_cmd = source(CPP / "angband/cmd-cave.c", errors)
    player_attack = source(CPP / "angband/player-attack.c", errors)
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
