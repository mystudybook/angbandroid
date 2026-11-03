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
    spell_ui = source(CPP / "angband/ui-spell.c", errors)
    player_spell = source(CPP / "angband/player-spell.c", errors)
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
         "angdroid_zh_tw_monster_list_name(race->name)", 128),
        (obj, obj_header, "ANGDROID_ZH_TW_ITEM_KINDS",
         "angdroid_zh_tw_item_kind_name(kind->name)", 76),
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
    for symbol in ("angdroid_utf8_display_columns",
                   "angdroid_utf8_clip_columns",
                   "angdroid_utf8_printf_field_bytes"):
        expect(symbol in display_width,
               f"Missing Unicode column helper: {symbol}")
        expect(symbol in monster_list and symbol in object_list,
               f"Native monster/item lists are not both using {symbol}")
    expect("test_unicode_display.c" in native_tests,
           "CJK UTF-8 clipping regression is not registered")

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
    if errors:
        for item in errors:
            print("FAIL:", item)
        return 1
    print(f"PASS: {checks} native game-localization source safeguards")
    print("  Monster list entries: 128 | item kind labels: 76")
    print("  Death menu labels: 9 | death prompts: 11")
    print("NOTE: Requires Android NDK and on-device zh-TW gameplay validation")
    return 0


if __name__ == "__main__":
    sys.exit(main())
