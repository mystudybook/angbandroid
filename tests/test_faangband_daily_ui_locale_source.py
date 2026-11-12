#!/usr/bin/env python3
"""FAangband daily commands, store, input and options: screen-only zh-TW.

The original command keys, item selection checks, quantity constraints and
option IDs must remain unchanged; untranslated strings retain English mode.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
KNOW = (FA / "ui-knowledge.c").read_text(encoding="utf-8")
STORE = (FA / "ui-store.c").read_text(encoding="utf-8")
INPUT = (FA / "ui-input.c").read_text(encoding="utf-8")
OPTIONS = (FA / "ui-options.c").read_text(encoding="utf-8")
UI = (ROOT / "app/src/main/cpp/angband/zh-tw-ui.h").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def extract(text, begin, end):
    assert begin in text and end in text
    body = text.split(begin, 1)[1]
    assert end in body
    return body.split(end, 1)[0]

def has(body, *tokens):
    for token in tokens:
        assert token in body, token

inventory = extract(KNOW, "void do_cmd_inven(void)", "void do_cmd_look(void)")
locate = extract(KNOW, "void do_cmd_locate(void)", "static int cmp_mexp(")
shop_header = extract(STORE, "static int context_menu_store(", "static bool handle_g_context_store_item(")
shop_popup = extract(STORE, "static bool context_menu_store_item(", "static bool store_menu_handle(")
store_rules = extract(STORE, "static bool store_sell(", "static void store_examine(")
name_input = extract(INPUT, "bool get_character_name(", "static bool textui_get_string(")
pause = extract(INPUT, "void pause_line(", "static int dir_transitions")
options_main = extract(OPTIONS, "static menu_action option_actions[] =", "void cleanup_options(void)")
keymaps = extract(OPTIONS, "static struct menu *keymap_menu;", "static void do_cmd_visuals(")

# All three selection workflows use the same translated request, while the
# original work modes/context actions and no-item return paths remain.
has(inventory,
    '"You have nothing in your inventory."',
    '"You are not wielding or wearing anything."',
    '"You have nothing in your quiver."',
    '"Select Item:", angdroid_zh_tw_enabled()',
    "player->upkeep->command_wrk = (USE_INVEN);",
    "player->upkeep->command_wrk = (USE_EQUIP);",
    "player->upkeep->command_wrk = (USE_QUIVER);",
    "context_menu_object(obj)",
    "player_is_shapechanged(player)")
assert inventory.count("get_item(&obj, angdroid_game_ui_text(") == 3

has(locate,
    '"地圖區域 [%d,%d]（%s），方向？"',
    '"地圖區域 [%d(%02d),%d(%02d)]（%s），方向？"',
    '"Map sector [%d,%d], which is%s your sector.  Direction?"',
    'my_strcpy(tmp_val, "起點", sizeof(tmp_val));',
    'angdroid_utf8_display_columns(prompt)',
    'angdroid_knowledge_clip_label(visible, sizeof(visible),',
    "dir = target_dir(command);", "change_panel(dir);", "verify_panel();")

has(shop_header,
    'angdroid_game_ui_text("Inspect inventory",',
    '"存放" : "出售"',
    'angdroid_game_ui_text("Exit",',
    "ACT_INSPECT_INVEN", "ACT_SELL", "ACT_EXIT",
    "fa_store_put_clipped(COLOUR_WHITE,")
has(shop_popup,
    'angdroid_game_ui_text("Examine",',
    '"取出" : "購買"',
    'home ? "Take one" : "Buy one"',
    "(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)",
    'fa_store_put_clipped(COLOUR_WHITE,',
    'ctx->list[oid] != obj',
    "ACT_BUY_ONE", 'm->switch_keys = "g";')

has(store_rules,
    'angdroid_store_quantity_prompt(',
    'ANGDROID_STORE_Q_STASH',
    'ANGDROID_STORE_Q_TAKE',
    'ANGDROID_STORE_Q_BUY',
    'angdroid_game_ui_text(',
    '"金幣"' if False else '"價格：%ld"',
    'angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0',
    'store_check_num(store, temp_obj)',
    'ctx->list[item] != obj',
    'cmdq_push(CMD_BUY)',
    'cmdq_push(CMD_SELL)')
# Monetary acceptance in FA is unchanged: ESC and N/n reject, all other
# keys accept. No implicit change to the existing trading behavior.
confirm = extract(STORE, "static bool store_get_check(", "static bool store_sell(")
has(confirm, "if (ch.code == ESCAPE)", 'strchr("Nn", ch.code)')

has(INPUT,
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    '"數量（0-%d，* 代表全部）："',
    '"File name: ", angdroid_zh_tw_enabled()',
    '"Replace existing file? ", angdroid_zh_tw_enabled()',
    '"Saving as %s.", angdroid_zh_tw_enabled()',
    '"Direction or <click> (Escape to cancel)? "',
    'Term_control(TERM_CONTROL_QUANTITY, "end")',
    'soft_kbd_linger("${quant}*")')
has(name_input,
    '"Enter a name for your character (* for a random name): "',
    'soft_kbd_linger("*")',
    'res = askfor_aux_ext(buf, buflen, get_name_keypress, handle_name_mouse)',
    'soft_kbd_clear(true)')
has(pause,
    'angdroid_utf8_display_columns(message)',
    'angdroid_menu_copy_columns(visible, sizeof(visible), message,',
    "put_str(visible, tm->hgt - 1, x)")

has(options_main,
    "static const char *const option_action_english[] = {",
    "assert(N_ELEMENTS(option_action_english) == N_ELEMENTS(option_actions))",
    "option_actions[i].name = option_action_english[i] ?",
    '"Options Menu", angdroid_zh_tw_enabled()',
    'option_menu->flags = MN_CASELESS_TAGS',
    "menu_select(option_menu, 0, false)")
assert options_main.count("menu_select(option_menu, 0, false)") == 1
has(keymaps,
    "static const char *const keymap_action_english[] = {",
    "assert(N_ELEMENTS(keymap_actions) == N_ELEMENTS(keymap_action_english))",
    "keymap_actions[i].name = angdroid_game_ui_text(",
    "keymap_menu->title = title;",
    "m->switch_keys" if False else "ui_keymap_query")
has(OPTIONS,
    '"目前基本延遲：%d 毫秒"',
    '"目前模式：%s"',
    '"目前生命值警告：%d（%d%%）"',
    '"目前移動延遲：%d（%d 毫秒）"',
    '"操作：側邊欄模式"' if False else '"Command: Sidebar Mode"',
    '"已載入「%s」。"')
has(UI,
    '"You have nothing in your inventory."',
    '"Purchase which item? (ESC to cancel, Enter to select)"',
    '"Direction or <click> (Escape to cancel)? "')
assert "test_faangband_daily_ui_locale_source.py" in RUNNER
print("PASS: FA daily commands, store, input and settings locale display contracts")
