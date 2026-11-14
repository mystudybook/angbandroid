#!/usr/bin/env python3
"""FAangband knowledge main menu, shape lore, history and symbol query UI.

Verify only display-facing translations and UTF-8 positioning change.
The source engine's callbacks, sorting, search and save data remain intact.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
FA = (CPP / "faangband/ui-knowledge.c").read_text(encoding="utf-8")
UI = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
SHAPES = (CPP / "angband/zh-tw-shape-lore.h").read_text(encoding="utf-8")
SYMBOL = (CPP / "angband/zh-tw-symbol-query.h").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def section(begin, end):
    assert begin in FA and end in FA
    return FA.split(begin, 1)[1].split(end, 1)[0]

shape_menu = section("static void do_cmd_knowledge_shapechange(", "ui_knowledge.txt parsing")
shape_lore = section("static void shape_lore(", "static void do_cmd_knowledge_shapechange(")
shape_lists = section("static void shape_lore_helper_append_to_list(", "static void shape_lore(")
main_menu = section("static menu_action knowledge_actions[] =", "Other knowledge functions")
messages = section("void do_cmd_messages(void)", "#define GET_ITEM_PARAMS")
symbols = section("static void lookup_symbol(", "void do_cmd_center_map(")

for header in (
    '#include "../angband/zh-tw-shape-lore.h"',
    '#include "../angband/zh-tw-spell-names.h"',
    '#include "../angband/zh-tw-class-book-names.h"',
    '#include "../angband/zh-tw-symbol-query.h"',
    '#include "../angband/zh-tw-terrain-trap.h"',
):
    assert header in FA, header

# Shape order is canonical, but rows, headings and lore are translated.
for key in (
    "sort(sarray, count, sizeof(sarray[0]), compare_shape_names);",
    "angdroid_shape_name(sarray[i]->name, angdroid_zh_tw_enabled())",
    '"知識圖鑑：變身形態"',
    '"名稱" : "Name"',
    '"r 詳情／Esc 返回"',
    "angdroid_knowledge_clip_label(clipped, sizeof(clipped),",
    "if (first_w < 12 || first_h < 9) return;",
    "if (wnew < 12 || hnew < 9) break;",
    "if (h != hnew || w != wnew)",
    "menu_layout(m, &list_region);",
    'footer = angdroid_zh_tw_enabled() ? "Esc 返回" : "Esc";',
    'soft_kbd_flash("r");',
    'soft_kbd_clear(true);',
    "case EVT_ESCAPE:",
    "shape_lore(sarray[mark]);",
):
    assert key in shape_menu, key
assert "angdroid_shape_name(s->name, angdroid_zh_tw_enabled())" in shape_lore
assert "angdroid_shape_intro(angdroid_zh_tw_enabled())" in shape_lore
assert "shape_lore_append_basic_combat(tb, s);" in shape_lore
assert "shape_lore_append_triggering_spells(tb, s);" in shape_lore

for key in (
    '"護甲 %+d"', '"命中 %+d"', '"傷害 %+d"',
    'angdroid_shape_term("Adds", angdroid_zh_tw_enabled())',
    'angdroid_shape_term(skill_index_to_name(i),',
    'lookup_obj_property(OBJ_PROPERTY_MOD, i)->name',
    'angdroid_shape_term("Makes you vulnerable to", angdroid_zh_tw_enabled())',
    'angdroid_shape_term("Makes you resistant to", angdroid_zh_tw_enabled())',
    'angdroid_shape_term("Makes you immune to", angdroid_zh_tw_enabled())',
    'angdroid_shape_term("Provides protection from", angdroid_zh_tw_enabled())',
    'angdroid_shape_term("Sustains", angdroid_zh_tw_enabled())',
    "angdroid_shape_misc_desc(prop->desc,",
    "angdroid_shape_misc_desc(ability->desc,",
    "angdroid_zh_tw_spell_name(spell->name, true)",
    "angdroid_zh_tw_class_book_name(",
    '"The %s spell, %s, from %s triggers the shapechange."',
    "shape_lore_append_list(tb, msgs, n);",
):
    assert key in shape_lists, key
assert "angdroid_shape_intro(int zh_tw)" in SHAPES

# All twelve action callbacks and their fixed positions must stay the same.
assert "static const char *const knowledge_action_keys[] = {" in main_menu
assert "assert(N_ELEMENTS(knowledge_action_keys) == N_ELEMENTS(knowledge_actions))" in main_menu
assert "knowledge_actions[i].name = angdroid_game_ui_text(" in main_menu
assert "knowledge_menu.title = angdroid_game_ui_text(" in main_menu
assert "menu_setpriv(menu, N_ELEMENTS(knowledge_actions), knowledge_actions);" in main_menu
assert "knowledge_actions[7].flags =" in main_menu
action_block = main_menu.split("};", 1)[0]
action_count = len(re.findall(r"\{\s*0,\s*0,\s*\"Display ", action_block))
assert action_count == 12, action_count
assert '"Display contents of home", "查看家中物品"' in UI

# UTF-8 horizontal scrolling and search matches must use terminal columns.
for key in (
    "Term_get_size(&wid, &hgt);",
    "if (wid < 3 || hgt < 5) break;",
    "angdroid_utf8_skip_columns(msg, (size_t)q)",
    "angdroid_utf8_prefix_columns(msg, str)",
    "angdroid_utf8_clip_columns(highlighted,",
    "angdroid_knowledge_clip_label(visible, sizeof(visible),",
    "message_color(i + j)",
    "message_count(i + j)",
    "my_stristr(message_str(z), shower)",
    '"歷史訊息（%d–%d／%d），捲動 %d 欄"',
    'prompt = "Esc";',
    "angdroid_game_ui_text(shower[0] ?",
    'soft_kbd_flash("=-pn");',
    "soft_kbd_clear(true);",
    '"Find: "',
    "askfor_aux(shower, sizeof shower, NULL)",
    "case ARROW_LEFT:",
    "case ARROW_RIGHT:",
    "case KC_PGUP:",
    "case KC_PGDOWN:",
):
    assert key in messages, key
assert "(msg + q)" not in messages
assert "Term_putstr(str-msg," not in messages
assert "my_strcpy" not in messages

# Source symbol identity remains unchanged; only text labels are translated.
for key in (
    "angdroid_symbol_tval_name(canonical,",
    "angdroid_symbol_query_format(buf, max, sym, canonical,",
    "angdroid_terrain_name(f_info[i].name,",
    "angdroid_symbol_monster_base_text(race->name, race->text,",
    '"%c - 未知符號。"',
    '"? - 未知符號。"',
    '"Enter character to be identified, or control+[ANU]: "',
    '"Full monster list."',
    '"Unique monster list."',
    '"Non-unique monster list."',
    '"查看詳情？(y/k/n)："',
    '" [(r)ecall, ESC]"',
    "if (sym == KTRL('A'))",
    "sym == KTRL('U')",
    "sym == KTRL('N')",
    "sort(who, num, sizeof(*who), cmp_pkill)",
    "sort(who, num, sizeof(*who), cmp_level)",
):
    assert key in symbols, key
assert "angdroid_symbol_monster_base_text(" in SYMBOL
assert 'test_faangband_knowledge_shape_message_symbol_source.py' in VERIFY
print("PASS: FA shape knowledge, history search and symbol query display-only translation")
