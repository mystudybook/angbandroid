#!/usr/bin/env python3
"""FAangband store tap order, scrolled popup rows and stale selection safety."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MENU = (ROOT / "app/src/main/cpp/faangband/ui-menu.c").read_text(encoding="utf-8")
STORE = (ROOT / "app/src/main/cpp/faangband/ui-store.c").read_text(encoding="utf-8")
OBJECT = (ROOT / "app/src/main/cpp/faangband/ui-object.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_faangband_store_touch_flow.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

select = MENU.split("ui_event menu_select(", 1)[1].split(
    "const menu_iter *menu_find_iter(", 1)[0]
mouse = select.split("if (in.type == EVT_MOUSE) {", 1)[1].split(
    "} else if (in.type == EVT_KBRD)", 1)[0]
assert "if (region_inside(&menu->active, &in)) {" in mouse
inside = mouse.split("if (region_inside(&menu->active, &in)) {", 1)[1].split(
    "} else {", 1)[0]
assert "menu_handle_mouse(menu, &in, &out);" in inside
assert "menu_handle_action(menu, &in)" in inside
assert inside.index("menu_handle_mouse(menu, &in, &out);") < inside.index(
    "menu_handle_action(menu, &in)")
assert "if (out.type == EVT_SELECT && !no_act &&" in inside
assert "if (!no_act && menu_handle_action(menu, &in))" in mouse
assert "menu_handle_mouse(menu, &in, &out);" in mouse
assert "out.type == EVT_SELECT && !no_act &&" in select
assert "menu_handle_action(menu, &out)" in select
assert "(menu->flags & MN_MOUSE_ROW_ACTION)" in inside
assert "touch_select_dispatched = true;" in inside
assert "!touch_select_dispatched &&" in select
assert "get_item_action" in OBJECT
item_handler = OBJECT.split("static bool get_item_action(", 1)[1].split(
    "static void item_menu_browser(", 1)[0]
assert "if (event->type == EVT_SELECT)" in item_handler

assert '#include "../angband/store-menu-row.h"' in STORE
assert '#include "../angband/store-stock-guard.h"' in STORE
get_stock = STORE.split("static int store_get_stock(", 1)[1].split(
    "/** Enum for context menu entries */", 1)[0]
assert "angdroid_store_selected_index(" in get_stock
assert "e.type == EVT_SELECT" in get_stock
assert "ctx->list[selected]" in get_stock
assert "return oid;" not in get_stock
assert "store_get_stock(m, oid)" not in STORE

handler = STORE.split("static bool store_menu_handle(", 1)[1].split(
    "static void store_menu_resize(", 1)[0]
assert "angdroid_store_menu_screen_row(" in handler
assert "popup_row < 0" in handler
assert "context_menu_store_item(ctx, oid, 1, popup_row);" in handler
assert "angdroid_store_stock_index_valid(" in handler
assert "ctx->list[oid]" in handler
assert "region_inside(&m->active, event) &&" in handler
assert "event->mouse.y == angdroid_store_menu_screen_row(" in handler
assert "m->active.row + oid" not in handler
assert "if ((event->mouse.y == 0) || (event->mouse.y == 1))" in handler
assert "event->mouse.button == 2" in handler
popup_select = handler.split("if (event->type == EVT_SELECT) {", 1)[1].split(
    "} else if (event->type == EVT_MOUSE)", 1
)[0]
assert "(void)context_menu_store_item(ctx, oid, 1, popup_row);" in popup_select
assert "if (purchased)" not in popup_select
for repaint in ("store_display_recalc(ctx);", "store_menu_recalc(m);", "store_redraw(ctx);"):
    assert repaint in popup_select, repaint
assert popup_select.index("cmdq_pop(CTX_STORE);") < popup_select.index(
    "store_display_recalc(ctx);")

for test in (
    "stock_touch_after_scrolling();", "filtered_inventory_view();",
    "obsolete_selection_cannot_purchase();", "INT_MAX",
):
    assert test in NATIVE, test
assert "test_faangband_store_touch_flow.c" in RUN
assert "test_faangband_store_touch_source.py" in VERIFY
print("PASS: FA tapped stock row precedes callback; visible coordinates and selections safe")
