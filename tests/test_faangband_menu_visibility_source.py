#!/usr/bin/env python3
"""FAangband relative tags: no hidden/disabled ghost keys on Android."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
HELPER = (FA / "ui-menu-key-guard.h").read_text(encoding="utf-8")
NAV = (ROOT / "app/src/main/cpp/angband/menu-valid-cursor.h").read_text(
    encoding="utf-8")
TAGS = (ROOT / "app/src/main/cpp/angband/menu-visible-tags.h").read_text(
    encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_menu_visibility_contract.c").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "ui-menu-key-guard.h"' in MENU
assert '#include "../angband/menu-valid-cursor.h"' in MENU
assert '#include "../angband/menu-visible-tags.h"' in MENU
assert "angdroid_fa_menu_source_index(" in HELPER
assert "angdroid_menu_find_valid_cursor(" in NAV
assert "angdroid_menu_visible_tag_first(" in TAGS
assert "angdroid_menu_visible_tag_end(" in TAGS

visibility = MENU.split("static int fa_menu_navigation_is_valid(", 1)[1].split(
    "static int get_cursor_key(", 1)[0]
assert "angdroid_fa_menu_source_index(" in visibility
assert "if (oid < 0) return false;" in visibility
assert "menu->row_funcs->valid_row(menu, oid) == MN_ROW_VALID" in visibility

no_valid = visibility.split("static bool no_valid_row(", 1)[1].split(
    "\n}\n", 1)[0]
assert "fa_menu_navigation_is_valid(menu, i)" in no_valid
assert "is_valid_row(menu, i)" not in no_valid

relative = MENU.split("static int get_cursor_key(", 1)[1].split(
    "static menu_row_style_t menu_row_style_for_validity(", 1)[0]
assert "bool paged = fa_menu_relative_tags_paged(menu);" in relative
assert "if (!fa_menu_navigation_is_valid(menu, i)) continue;" in relative
assert "c = menu->skin->get_tag(menu, paged ? i - first : i);" in relative
assert relative.index("if (!fa_menu_navigation_is_valid(menu, i))") < (
    relative.index("c = menu->skin->get_tag("))
# The explicitly assigned tag paths are retained; we only suppress
# automatically generated *relative* ghost keys for undisplayed entries.
assert "menu->selections[i]" in relative
assert "menu->row_funcs->get_tag(menu, oid);" in relative
assert "menu->inscriptions[D2I(key.code)]" in relative

row = MENU.split(
    "static void display_menu_row(struct menu *menu, int pos, int top,\n"
    "                             bool cursor, int row, int col, int width)\n{", 1)[1].split(
    "void menu_refresh(", 1)[0]
assert "if (row_valid == MN_ROW_HIDDEN)" in row
assert "return;" in row
assert "fa_menu_relative_tags_paged(menu) ? pos - top : pos" in row

mouse = MENU.split("bool menu_handle_mouse(", 1)[1].split(
    "static bool menu_handle_action(", 1)[0]
assert "fa_menu_navigation_is_valid(menu, new_cursor)" in mouse
assert "if (is_valid_row(menu, new_cursor)" not in mouse

keyboard = MENU.split("bool menu_handle_keypress(", 1)[1].split(
    "static void keys_to_ui(", 1)[0]
assert "if (fa_menu_navigation_is_valid(menu, menu->cursor))" in keyboard
assert "no_valid_row(menu, count)" in keyboard
assert "angdroid_menu_find_valid_cursor(" in keyboard

hints = MENU.split("static void keys_to_ui(", 1)[1].split(
    "ui_event menu_select(", 1)[0]
assert "if (oid < 0 || !fa_menu_navigation_is_valid(menu, cursor))" in hints
assert "angdroid_menu_visible_tag_first(" in hints
assert "angdroid_menu_visible_tag_end(" in hints
assert "paged ? cursor - first : cursor" in hints
assert "buf, tag_capacity, &used, sel" in hints
assert "soft_kbd_replace(buf);" in hints
assert "row_valid != MN_ROW_VALID" not in hints

for test in (
    "test_filtered_hidden_labels();",
    "test_invalid_filters_and_bounds();",
    "mask < 729",
    "const int mapping[] = {5, 1, 3, 2, 0, 4}",
    "INT_MAX",
    "INT_MIN",
):
    assert test in TEST, test
assert "angdroid_menu_find_valid_cursor(" in TEST
assert "angdroid_menu_visible_tag_first(" in TEST
assert "test_faangband_menu_visibility_contract.c" in NATIVE
assert "test_faangband_menu_visibility_source.py" in VERIFY
print("PASS: FA displayed rows, relative shortcuts, and soft-key hints agree")
