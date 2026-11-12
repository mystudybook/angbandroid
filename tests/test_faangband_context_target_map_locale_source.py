#!/usr/bin/env python3
"""Source-level contract for FAangband's everyday context/target/map prompts.

These are display-only translations.  Key bindings, command codes, target
paths, item awareness and behavior of the rest/retire controls stay unchanged.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
CONTEXT = (FA / "ui-context.c").read_text(encoding="utf-8")
TARGET = (FA / "ui-target.c").read_text(encoding="utf-8")
MAP = (FA / "ui-map.c").read_text(encoding="utf-8")
COMMAND = (FA / "ui-command.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def section(src, start, stop):
    assert start in src and stop in src, (start, stop)
    return src.split(start, 1)[1].split(stop, 1)[0]


def need(src, *needles):
    for needle in needles:
        assert needle in src, needle


# One shared display-only translator handles the extra FA commands; the
# existing common dictionary remains the fallback.
labels = section(CONTEXT, "static const char *fa_context_ui_text(",
                 "static void fa_context_print_prompt(")
need(labels,
    'if (!angdroid_zh_tw_enabled() || !english) return english;',
    'return angdroid_game_ui_text(english, true);',
    '{"Use", "使用"}', '{"Cast", "施法"}',
    '{"Inscribe", "刻字"}', '{"Uninscribe", "清除刻字"}',
    '{"Open Chest", "開啟寶箱"}', '{"Steal", "偷竊"}',
    '{"Drop", "丟棄"}', '{"Unignore", "取消忽略"}')
need(CONTEXT,
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    'menu_dynamic_add_label_valid(m, fa_context_ui_text((text)),',
    'angdroid_menu_copy_columns(clipped, sizeof(clipped), label,',
    'prt(clipped, 0, 0);')
assert CONTEXT.count('fa_context_print_prompt(angdroid_game_ui_text('
                     '"(Enter to select, ESC) Command:"') == 4
assert CONTEXT.count('msg("%s", angdroid_game_ui_text("Target Selected."') == 3

# Pseudo IDs and cancelled menu values are not legal cmd_code values.
# Only an actionable real command passes through cmd_lookup_key().
player = section(CONTEXT, "int context_menu_player(int mx, int my)",
                 "int context_menu_cave(")
cave = section(CONTEXT, "int context_menu_cave(",
               "int context_menu_object(")
obj = section(CONTEXT, "int context_menu_object(",
              "static int show_command_list(")
for piece in (player, cave, obj):
    need(piece,
        '/* Pseudo menu values and Cancel are not cmd_code values. */',
        'cmdkey = 0;',
        'cmdkey = cmd_lookup_key(selected, mode);',
        'case -1:', 'menu_dynamic_select(m)')
    assert piece.index('cmdkey = 0;') < piece.index('switch')
    assert piece.index('cmdkey = cmd_lookup_key(selected, mode);') > (
        piece.index('switch'))

need(player, 'MENU_VALUE_INVENTORY', 'MENU_VALUE_REST',
     'key_confirm_command(cmdkey)',
     'cmdq_push(selected);', "Term_keypress('R', 0);")
need(cave, 'square_apparent_name(player->cave, grid)',
     'fa_context_print_prompt(angdroid_zh_tw_enabled() ?',
     'cmd_set_arg_point(cmdq_peek(), "point", loc(x, y));',
     'ODESC_PREFIX | ODESC_FULL |',
     '(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)')
need(obj, 'fa_context_ui_text("Drop All")',
     'object_is_ignored(obj) ? "Unignore" : "Ignore"',
     'get_item_allow(obj, cmdkey, selected, false)',
     'r.width = MIN(r.width, MAX(1, Term->wid - 2));',
     '"(Enter to select, ESC) Command for %s:"',
     '(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)')
# The command ID must remain tied to the original key; Chinese text is not
# written into cmd_info, keymap, or cached nested-menu names.
show_command = section(CONTEXT, "static int show_command_list(",
                       "int context_menu_command(")
need(show_command,
     'fa_context_ui_text(cmd_list[i].desc)',
     'Term_keypress(cmd_list[selected-1].key[mode], 0)',
     'char cmd_name[192];')
need(CONTEXT,
     'fa_context_ui_text(commands[oid].desc)',
     'fa_context_ui_text(cmds_all[oid].name)',
     'keypress_to_readable(key, sizeof(key), kp)',
     'angdroid_menu_copy_columns(visible, sizeof(visible), full,',
     'menu_dynamic_add(m, fa_context_ui_text("Item"), 1)',
     'show_command_list(cmds_all[selected].list, cmds_all[selected].len,')

# The target help is translated without changing the target selection
# controls or consuming state in the rest of the UI.
need(TARGET, 'msg("%s", angdroid_game_ui_text("Target Selected."',
     'msg("%s", angdroid_game_ui_text("Target Aborted."',
     '"Press \'?\' for help."',
     'text_out_wrap = saved_wrap;',
     'text_out_wrap = MIN(MAX(1, saved_wrap), MAX(1, wid - 2));',
     'text_out(angdroid_zh_tw_enabled() ? " 與 " : " and ");',
     "target_set_monster(m_local);",
     "target_set_location(y, x);",
     "cmd_set_arg_point(cmdq_peek(), \"point\", loc(x, y));")
assert TARGET.count('"Press \'?\' for help."') == 2

# Map messages are clipped by Unicode columns, centered without negative x,
# while the native wilderness movement mapping stays identical.
need(MAP,
     'static void fa_map_prompt_centered(',
     'angdroid_utf8_display_columns(message)',
     'angdroid_menu_copy_columns(clipped, sizeof(clipped), message,',
     'x = columns >= Term->wid ? 0 : (Term->wid - columns) / 2;',
     '"按任意鍵繼續"',
     '"請稍候……"',
     '"方向鍵捲動，其他按鍵繼續"',
     'fa_map_prompt_centered(prompt, Term->hgt - 1);',
     'case ARROW_UP:', 'case ARROW_DOWN:',
     'case ARROW_LEFT:', 'case ARROW_RIGHT:')

# The visible command strings changed. The actual confirmation characters,
# scheduled rest modes, and output file formats did not.
need(COMMAND,
     '"請輸入 @ 確認角色退休："',
     "soft_kbd_flash(\"@\");",
     "if (ch.code != '@') return;",
     '"休息（0-9999 回合、! 回 HP/SP、* 回滿、& 完全恢復、$ 日出／日落）："',
     "REST_COMPLETE", "REST_ALL_POINTS", "REST_SOME_POINTS",
     "REST_SUNLIGHT",
     '"擷取格式：H=HTML、F=論壇文字？ "',
     '"Dump as (H)TML or (F)orum text? "',
     '"hf", 2, \' \'',
     'html_screenshot(tmp_val, mode, other_term);',
     '"要包含怪物清單嗎？ "')

assert "test_faangband_context_target_map_locale_source.py" in RUNNER
print("PASS: FA context menus, target help, maps and common command zh-TW contracts")
