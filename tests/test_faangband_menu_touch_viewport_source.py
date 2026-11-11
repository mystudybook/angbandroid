#!/usr/bin/env python3
"""FA menu touch cells, filtered rows, compact layout, and post-rotation cursor."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
ANG = ROOT / "app/src/main/cpp/angband"
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
VIEW = (FA / "ui-menu-viewport.h").read_text(encoding="utf-8")
HIT = (ANG / "menu-hit-test.h").read_text(encoding="utf-8")
COLUMN = (ANG / "menu-column-width.h").read_text(encoding="utf-8")
COMPACT = (ANG / "menu-compact-layout.h").read_text(encoding="utf-8")
KEYS = (FA / "ui-menu-key-guard.h").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_menu_touch_viewport.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for name in (
    "ui-menu-viewport.h", "menu-hit-test.h",
    "menu-column-width.h", "menu-compact-layout.h",
):
    directory = "../angband/" if name.startswith("menu-") else ""
    assert f'#include "{directory}{name}"' in MENU
assert "angdroid_fa_menu_clamp_cursor(" in VIEW
assert "angdroid_fa_menu_clamp_top(" in VIEW
assert "cursor - top >= page_rows" in VIEW
assert "max_top = count > page_rows ? count - page_rows : 0;" in VIEW
assert "angdroid_menu_scrolling_hit(" in HIT
assert "angdroid_menu_columns_hit(" in HIT
assert "angdroid_menu_fit_column_width(" in COLUMN
assert "angdroid_menu_use_compact_columns(" in COMPACT
assert "angdroid_fa_menu_selection_key(" in KEYS

scroll = MENU.split("static int scrolling_get_cursor(", 1)[1].split(
    "static void display_scrolling(", 1)[0]
object_cursor = MENU.split("static int object_skin_get_cursor(", 1)[1].split(
    "static void object_skin_display(", 1)[0]
for func in (scroll, object_cursor):
    assert "angdroid_menu_scrolling_hit(" in func
    assert "if (cursor >= n) cursor = n - 1;" not in func

scroll_body = MENU.split("static void display_scrolling(", 1)[1].split(
    "static char scroll_get_tag(", 1)[0]
object_body = MENU.split("static void object_skin_display(", 1)[1].split(
    "static char object_skin_get_tag(", 1)[0]
for func in (scroll_body, object_body):
    assert "angdroid_fa_menu_clamp_top(*top, cursor, n, rows_per_page);" in func
    assert "if (*top < n && i < n - *top)" in func
    assert "cursor - *top < rows_per_page" in func
    assert "i + *top < n" not in func or "i < n - *top" in func

cols_hit = MENU.split("static int columns_get_cursor(", 1)[1].split(
    "static void display_columns(", 1)[0]
cols_draw = MENU.split("static void display_columns(", 1)[1].split(
    "static char column_get_tag(", 1)[0]
assert "rows <= 0 || n <= 0 || loc->width <= 0" in cols_hit
assert "if (rows <= 0 || loc->width <= 0) return;" in cols_draw
assert "for (r = 0; r < rows; ++r)" in cols_draw
assert "fa_menu_erase_clipped(row + r, col, loc->width);" in cols_draw
assert "if (n <= 0) return;" in cols_draw
for func in (cols_hit, cols_draw):
    assert "angdroid_menu_use_compact_columns(n, rows, loc->width, 8)" in func
    assert "angdroid_menu_fit_column_width(23, loc->width, 0, cols)" in func
assert "angdroid_menu_scrolling_hit(" in cols_hit
assert "angdroid_menu_columns_hit(" in cols_hit
assert "display_scrolling(menu, cursor, top, loc);" in cols_draw
assert "colw = (w - col) / cols" not in cols_hit + cols_draw
assert "if (cursor < 0) cursor = 0;" not in cols_hit
assert "col + c * colw" in cols_draw
assert "Term_gotoxy(col + dx, row + dy);" in cols_draw

column_direction = MENU.split("static ui_event column_process_direction(", 1)[1].split(
    "static const menu_skin menu_skin_column", 1)[0]
assert "if (rows_per_page <= 0 || n <= 0) return out;" in column_direction
assert "return scroll_process_direction(m, dir);" in column_direction
assert "angdroid_fa_menu_clamp_cursor(m->cursor, n)" in column_direction

for tag in ("scroll_get_tag", "object_skin_get_tag", "column_get_tag"):
    func = MENU.split("static char " + tag + "(", 1)[1].split("\n}\n", 1)[0]
    assert "angdroid_fa_menu_selection_key(" in func, tag
    assert "menu->selections[pos - menu->top]" not in func

row = MENU.split("static void display_menu_row(struct menu *menu, int pos, int top,\n"
    "                             bool cursor, int row, int col, int width)\n{", 1)[1].split(
    "void menu_refresh(", 1)[0]
assert "fa_menu_relative_tags_paged(menu) ? pos - top : pos" in row

ensure = MENU.split("void menu_ensure_cursor_valid(", 1)[1].split(
    "static bool menu_calc_size(", 1)[0]
assert "angdroid_fa_menu_clamp_cursor(" in ensure
assert "if (count <= 0)" in ensure
assert "m->cursor = -1;" in ensure
assert "for (row = start; row < count; row++)" in ensure
assert "for (row = 0; row < start; row++)" in ensure
assert "angdroid_fa_menu_clamp_top(" in ensure

mouse = MENU.split("bool menu_handle_mouse(", 1)[1].split(
    "static bool menu_handle_action(", 1)[0]
assert "menu->skin->get_cursor(" in mouse
assert "fa_menu_navigation_is_valid(menu, new_cursor)" in mouse
assert "menu->skin->get_cursor(" in mouse
# Visible-row validity and filtered source mapping are both performed by
# the shared navigation adapter, not redundantly in the tap handler.
adapter = MENU.split("static int fa_menu_navigation_is_valid(", 1)[1].split(
    "static int get_cursor_key(", 1)[0]
assert "angdroid_fa_menu_source_index(" in adapter
assert "== MN_ROW_VALID" in adapter

select = MENU.split("ui_event menu_select(", 1)[1].split(
    "const menu_iter *menu_find_iter(", 1)[0]
assert "menu_ensure_cursor_valid(menu);" in select
assert select.count("menu_ensure_cursor_valid(menu);") >= 2
assert "in.type == EVT_RESIZE" in select
assert "menu_refresh(menu, popup);" in select

for func in (
    "scrolling_blank_rows();", "columns_match_drawn_cells();",
    "scroll_cursor_after_rotation();",
    "relative_shortcuts_after_scrolling();",
):
    assert func in TEST
assert "INT_MAX" in TEST and "INT_MIN" in TEST
assert "test_faangband_menu_touch_viewport.c" in RUN
assert "test_faangband_menu_touch_viewport_source.py" in VERIFY
print("PASS: FA menu blank row taps, filtered touch, column fit, and rotation scroll")
