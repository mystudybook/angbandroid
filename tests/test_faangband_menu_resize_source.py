#!/usr/bin/env python3
"""FAangband menu resize must redraw without cursor changes or bad geometry."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
UI = (FA / "ui-menu.c").read_text(encoding="utf-8")
STORE = (FA / "ui-store.c").read_text(encoding="utf-8")
OBJECT = (FA / "ui-object.c").read_text(encoding="utf-8")
GUARD = (FA / "ui-menu-resize-guard.h").read_text(encoding="utf-8")
UNICODE = (ROOT / "app/src/main/cpp/curses/unicode_display.h").read_text(
    encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_menu_resize_guard.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "ui-menu-resize-guard.h"' in UI
assert '#include "../curses/unicode_display.h"' in UI
assert "angdroid_fa_menu_region_visible(" in GUARD
assert "screen_width <= 0 || screen_height <= 0" in GUARD
assert "col < 0 || row < 0 || width <= 0 || rows <= 0" in GUARD
assert "col >= screen_width || row >= screen_height" in GUARD
assert "width <= screen_width - col && rows <= screen_height - row" in GUARD

refresh = UI.split("void menu_refresh(", 1)[1].split(
    "bool menu_handle_mouse(", 1)[0]
assert "if (!Term || !angdroid_fa_menu_region_visible(" in refresh
assert "Term->wid, Term->hgt, loc->col, loc->row" in refresh
assert "loc->width, loc->page_rows" in refresh
assert refresh.index("angdroid_fa_menu_region_visible(") < (
    refresh.index("if (reset_screen)"))
assert "menu->skin->display_list(menu, menu->cursor, &menu->top, loc);" in refresh

select = UI.split("ui_event menu_select(", 1)[1].split(
    "const menu_iter *menu_find_iter(", 1)[0]
assert "if (!Term || !angdroid_fa_menu_region_visible(" in select
assert "in.type = EVT_ESCAPE;" in select
assert "assert(menu->active.width != 0" not in select
assert "notify |= (EVT_SELECT | EVT_ESCAPE | EVT_SWITCH);" in select
assert "in.type == EVT_RESIZE" in select
assert "menu_calc_size(menu);" in select
assert "menu->row_funcs->resize(menu);" in select
resize = select.split("in.type == EVT_RESIZE", 1)[1].split(
    "/* Redraw menu here if cursor has moved */", 1)[0]
assert "angdroid_fa_menu_region_visible(" in resize
assert "if (popup) screen_load();" in resize
assert "in.type = EVT_ESCAPE;" in resize
assert "return in;" in resize
assert "menu_refresh(menu, popup);" in resize
assert resize.index("menu->row_funcs->resize(menu);") < (
    resize.index("menu_refresh(menu, popup);"))
assert "if (cursor != menu->cursor)" in select
assert "menu_refresh(menu, popup);" in select

size = UI.split("static bool menu_calc_size(struct menu *menu)\n{", 1)[1].split(
    "bool menu_layout(", 1)[0]
assert "angdroid_utf8_display_columns(menu->prompt);" in size
assert "if (menu->active.width <= 2 ||" in size
assert "cells >= (size_t)(menu->active.width - 2)" in size
assert "menu->active.width = 0;" in size
assert "int offset = (int)cells + 2;" in size
assert "strlen(menu->prompt) + 2" not in size
assert "angdroid_utf8_display_columns(" in UNICODE

# The shop only recalculates and draws its frame. The shared event loop
# handles the stock list repaint; no second rendering from store callback.
store_resize = STORE.split("static void store_menu_resize(", 1)[1].split(
    "\n}\n", 1)[0]
assert "store_display_recalc(ctx);" in store_resize
assert "STORE_FRAME_CHANGE | STORE_GOLD_CHANGE" in store_resize
assert "store_redraw(ctx);" in store_resize
assert "menu_refresh(m, false);" not in store_resize
assert "store_menu_handle,\n\tstore_menu_resize" in STORE

# Equipment and inventory menus still re-layout through their own callback,
# which the shared menu driver will repaint after the resize event.
item_resize = OBJECT.split("static void fa_item_menu_resize(", 1)[1].split(
    "\n}\n", 1)[0]
assert "angdroid_item_selection_region(" in item_resize
assert "menu_layout(m, &area);" in item_resize
assert "get_item_action, fa_item_menu_resize };" in OBJECT

assert "exhaustive_tiny_terminals();" in C_TEST
assert "rotation_extremes();" in C_TEST
assert "INT_MIN" in C_TEST and "INT_MAX" in C_TEST
assert "reference_visible(" in C_TEST
assert "w = -1; w <= 12" in C_TEST
assert "h = -1; h <= 10" in C_TEST
assert "test_faangband_menu_resize_guard.c" in RUN
assert "test_faangband_menu_resize_source.py" in VERIFY
print("PASS: FA generic menus resize/repaint with valid cells and Unicode prompts")
