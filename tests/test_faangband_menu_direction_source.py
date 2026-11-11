#!/usr/bin/env python3
"""FA keyboard movement is bounded; column tags match shown scroll layout."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
FA = CPP / "faangband"
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
SHARED = (CPP / "angband/menu-valid-cursor.h").read_text(encoding="utf-8")
TAGS = (CPP / "angband/menu-visible-tags.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_menu_direction.c").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "../angband/menu-valid-cursor.h"' in MENU
assert '#include "../angband/menu-visible-tags.h"' in MENU
assert "angdroid_menu_find_valid_cursor(" in SHARED
assert "angdroid_menu_page_next_cursor(" in SHARED
assert "angdroid_menu_visible_tag_first(" in TAGS
assert "angdroid_menu_visible_tag_end(" in TAGS

adapter = MENU.split(
    "static int fa_menu_navigation_is_valid(", 1)[1].split(
    "static int get_cursor_key(", 1)[0]
assert "angdroid_fa_menu_source_index(" in adapter
assert "if (oid < 0) return false;" in adapter
assert "menu->row_funcs->valid_row(menu, oid) == MN_ROW_VALID" in adapter

keyboard = MENU.split("bool menu_handle_keypress(", 1)[1].split(
    "static void keys_to_ui(", 1)[0]
assert "angdroid_menu_page_next_cursor(" in keyboard
assert "angdroid_menu_find_valid_cursor(" in keyboard
assert "fa_menu_navigation_is_valid" in keyboard
assert "int direction = ddy[dir] ? ddy[dir] : ddx[dir];" in keyboard
assert "if (next >= 0)" in keyboard
assert "out->type = EVT_NONE;" in keyboard
assert "menu->cursor = previous;" in keyboard
assert "if (fa_menu_navigation_is_valid(menu, menu->cursor))" in keyboard
assert "menu->cursor >= total - 1" not in keyboard
assert "while (!is_valid_row(menu, menu->cursor))" not in keyboard
assert "menu->cursor += ddy[dir];" not in keyboard
assert "menu->top = angdroid_fa_menu_clamp_top(" in keyboard
ensure = MENU.split("void menu_ensure_cursor_valid(", 1)[1].split(
    "static bool menu_calc_size(", 1)[0]
assert "fa_menu_navigation_is_valid(m, row)" in ensure
assert "is_valid_row(m, row)" not in ensure


# Columns: all tags are visible unless the skin has compacted down to a
# scrolling list. Display, user key input, and Android hint must agree.
mode = MENU.split("static bool fa_menu_relative_tags_paged(", 1)[1].split(
    "/* ================== GENERIC HELPER FUNCTIONS", 1)[0]
assert "menu->skin != &menu_skin_column" in mode
assert "angdroid_menu_use_compact_columns(" in mode
get_key = MENU.split("static int get_cursor_key(", 1)[1].split(
    "static menu_row_style_t menu_row_style_for_validity(", 1)[0]
assert "bool paged = fa_menu_relative_tags_paged(menu);" in get_key
assert "angdroid_menu_visible_tag_first(" in get_key
assert "angdroid_menu_visible_tag_end(" in get_key
assert "paged ? i - first : i" in get_key
assert "return i;" in get_key
assert "return i + menu->top;" not in get_key

display = MENU.split(
    "static void display_menu_row(struct menu *menu, int pos, int top,\n"
    "                             bool cursor, int row, int col, int width)\n{", 1)[1].split(
    "void menu_refresh(", 1)[0]
assert "fa_menu_relative_tags_paged(menu) ? pos - top : pos" in display

hints = MENU.split("static void keys_to_ui(", 1)[1].split(
    "ui_event menu_select(", 1)[0]
assert "bool paged = fa_menu_relative_tags_paged(menu);" in hints
assert "angdroid_menu_visible_tag_first(" in hints
assert "angdroid_menu_visible_tag_end(" in hints
assert "if (cursor < first || cursor >= end) continue;" in hints
assert "paged ? cursor - first : cursor" in hints
assert "tag_capacity" in hints
assert "soft_kbd_flash(buf);" in hints

for line in (
    "navigation_all_small_menus();", "page_navigation();",
    "relative_tag_modes();", "INT_MAX", "INT_MIN",
    "count <= 12", "mask < 2048",
):
    assert line in C_TEST, line
assert "test_faangband_menu_direction.c" in NATIVE
assert "test_faangband_menu_direction_source.py" in VERIFY
print("PASS: FA keyboard navigation finite, paging reaches last row, tags match")
