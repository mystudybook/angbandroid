#!/usr/bin/env python3
"""Filtered FA menus never misindex Android keyboard tags or overflow buffers."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
HELPER = (FA / "ui-menu-key-guard.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_menu_key_guard.c").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "ui-menu-key-guard.h"' in MENU
for function in (
    "angdroid_fa_menu_source_index(",
    "angdroid_fa_menu_selection_key(",
    "angdroid_fa_menu_add_key(",
    "angdroid_fa_menu_page_rows(",
):
    assert function in HELPER, function
assert "filter ? filter[position] : position" in HELPER
assert "source >= 0 && source < source_count ? source : -1" in HELPER
assert "capacity - *used <= 1" in HELPER
assert "output[*used] = '\\0';" in HELPER
assert "page_rows < count - top ? page_rows : count - top" in HELPER

valid = MENU.split("static bool is_valid_row(struct menu *menu, int cursor)\n{", 1)[1].split(
    "static bool no_valid_row(", 1)[0]
assert "angdroid_fa_menu_source_index(" in valid
assert "if (oid < 0)" in valid
assert "menu->row_funcs->valid_row(menu, oid)" in valid

selection = MENU.split("static int get_cursor_key(", 1)[1].split(
    "static menu_row_style_t menu_row_style_for_validity", 1)[0]
assert "angdroid_menu_visible_tag_first(" in selection
assert "angdroid_menu_visible_tag_end(" in selection
assert "(menu->flags & MN_INSCRIP_TAGS) && menu->inscriptions &&" in selection
assert "for (i = first; i < end; i++)" in selection
assert "for (i = 0; i < n && menu->selections[i]; i++)" in selection
assert "angdroid_fa_menu_source_index(" in selection
assert "if (oid < 0) continue;" in selection

display = MENU.split("static void display_menu_row(struct menu *menu, int pos, int top,\n"
    "                             bool cursor, int row, int col, int width)\n{", 1)[1].split(
    "void menu_refresh(", 1)[0]
assert "angdroid_fa_menu_source_index(" in display
assert "if (width <= 0 || oid < 0) return;" in display
assert "angdroid_fa_menu_selection_key(menu->selections, pos)" in display
assert "menu->row_funcs->display_row(menu, oid, cursor, row, col, width);" in display
assert "menu->filter_list[oid]" not in display

refresh = MENU.split("void menu_refresh(", 1)[1].split(
    "bool menu_handle_mouse(", 1)[0]
assert "angdroid_fa_menu_source_index(" in refresh
assert "if (menu->browse_hook && oid >= 0)" in refresh
action = MENU.split("static bool menu_handle_action(", 1)[1].split(
    "bool menu_handle_keypress(", 1)[0]
assert "angdroid_fa_menu_source_index(" in action
assert "if (oid < 0) return false;" in action

keyboard = MENU.split("static void keys_to_ui(", 1)[1].split(
    "ui_event menu_select(", 1)[0]
assert "char buf[256] = \"\";" in keyboard
assert "size_t used = 0;" in keyboard
assert "angdroid_fa_menu_source_index(" in keyboard
assert "menu->filter_list, cursor, n, menu->count" in keyboard
assert "angdroid_menu_visible_tag_first(" in keyboard
assert "angdroid_menu_visible_tag_end(" in keyboard
assert "angdroid_fa_menu_selection_key(" in keyboard
assert "angdroid_fa_menu_add_key(" in keyboard
assert "size_t tag_capacity;" in keyboard
assert "reserved = nkeys < remaining ? nkeys : remaining;" in keyboard
assert "tag_capacity = sizeof(buf) - reserved;" in keyboard
assert "buf, tag_capacity, &used, sel" in keyboard
assert "if (oid < 0) continue;" in keyboard
assert "temp[current++] = sel;" not in keyboard
assert "oid = menu->filter_list[oid]" not in keyboard
assert "my_strcat(buf, menu->cmd_keys, sizeof(buf));" in keyboard
assert "my_strcat(buf, menu->switch_keys, sizeof(buf));" in keyboard
assert "soft_kbd_flash(buf);" in keyboard
assert "strdeldup(buf);" in keyboard

for test in (
    "filtered_indices();", "visible_selection_keys();",
    "bounded_keyboard_text();", "page_relative_tags();",
):
    assert test in C_TEST
assert "for (i = 0; i < 3000; ++i)" in C_TEST
assert "INT_MAX" in C_TEST
assert "test_faangband_menu_key_guard.c" in NATIVE
assert "test_faangband_menu_key_guard_source.py" in VERIFY
print("PASS: FA filtered keyboard hints and selection keys are index/capacity safe")
