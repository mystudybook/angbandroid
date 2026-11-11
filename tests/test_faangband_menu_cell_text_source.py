#!/usr/bin/env python3
"""FA menus render title, prompt, selector, and row text by Unicode cells."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
UI = (FA / "ui-menu.c").read_text(encoding="utf-8")
CELL = (FA / "ui-menu-cell-text.h").read_text(encoding="utf-8")
CLIP = (ROOT / "app/src/main/cpp/angband/menu-text-fit.h").read_text(
    encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_menu_cell_text.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "ui-menu-cell-text.h"' in UI
assert '#include "../angband/menu-text-fit.h"' in CELL
assert "angdroid_fa_menu_visible_columns(" in CELL
assert "angdroid_fa_menu_copy_visible(" in CELL
assert "row < 0 || row >= terminal_height" in CELL
assert "col < 0 || col >= terminal_width" in CELL
assert "remaining = terminal_width - col;" in CELL
assert "return requested < remaining ? requested : remaining;" in CELL
assert "if (!out || capacity == 0) return 0;" in CELL
assert "out[0] = '\\0';" in CELL
assert "angdroid_menu_copy_columns(out, capacity, source, (size_t)cells)" in CELL
assert "angdroid_utf8_next_scalar(source, &codepoint)" in CLIP
assert "angdroid_terminal_columns(codepoint)" in CLIP

painter = UI.split("static void fa_menu_put_clipped(", 1)[1].split(
    "static void fa_menu_erase_clipped(", 1)[0]
assert "char clipped[512];" in painter
assert "angdroid_fa_menu_copy_visible(" in painter
assert "Term->wid, Term->hgt, row, col, limit" in painter
assert "c_put_str(color, clipped, row, col);" in painter
erase = UI.split("static void fa_menu_erase_clipped(", 1)[1].split(
    "/**", 1)[0]
assert "angdroid_fa_menu_visible_columns(" in erase
assert "Term_erase(col, row, cells);" in erase

actions = UI.split("static void display_action_aux(", 1)[1].split(
    "static char menu_action_tag(", 1)[0]
assert "fa_menu_erase_clipped(row, col, wid);" in actions
assert "fa_menu_put_clipped(color, act->name, row, col, wid);" in actions
assert "Term_putstr(" not in actions
strings = UI.split("static void display_string(", 1)[1].split(
    "static bool handle_string(", 1)[0]
assert "fa_menu_put_clipped(color, items[oid], row, col, width);" in strings
assert "Term_putstr(" not in strings
dynamic = UI.split("static void dynamic_display(", 1)[1].split(
    "static const menu_iter dynamic_iter", 1)[0]
assert "fa_menu_put_clipped(color, entry->text, row, col, width);" in dynamic
assert "Term_putstr(" not in dynamic

row = UI.split("static void display_menu_row(struct menu *menu, int pos, int top,\n"
    "                             bool cursor, int row, int col, int width)\n{", 1)[1].split(
    "void menu_refresh(", 1)[0]
assert "width = angdroid_fa_menu_visible_columns(" in row
assert "if (width <= 0 || oid < 0) return;" in row
assert "if (width <= 3) {" in row
assert 'fa_menu_put_clipped(color, format("%c) ", sel),' in row
assert "width -= 3;" in row
assert "menu->row_funcs->display_row(menu, oid, cursor, row, col, width);" in row
assert "Term_putstr(" not in row

refresh = UI.split("void menu_refresh(", 1)[1].split(
    "bool menu_handle_mouse(", 1)[0]
assert "angdroid_fa_menu_region_visible(" in refresh
for field in ("title", "header", "prompt"):
    assert f"if (menu->{field})" in refresh
    assert f"menu->{field}," in refresh
assert refresh.count("fa_menu_put_clipped(") == 3
assert "Term_putstr(" not in refresh
assert "menu->skin->display_list(menu, menu->cursor, &menu->top, loc);" in refresh
size = UI.split("static bool menu_calc_size(struct menu *menu)\n{", 1)[1].split(
    "bool menu_layout(", 1)[0]
assert "menu->active.col += 4;" in size
assert "menu->active.width = menu->active.width > 4 ?" in size
assert "menu->active.width - 4 : 0;" in size
assert "angdroid_utf8_display_columns(menu->prompt)" in size

for part in ("test_row_rectangles();", "test_chinese_labels();",
             "test_buffer_capacities();", "test_extreme_geometry();"):
    assert part in TEST
assert "check_utf8(out);" in TEST
assert "cap = 0; cap <= 48" in TEST
assert "INT_MIN" in TEST and "INT_MAX" in TEST
assert "test_faangband_menu_cell_text.c" in RUN
assert "test_faangband_menu_cell_text_source.py" in VERIFY
print("PASS: FA generic menus clip title, prompt, shortcuts, and rows to cells")
