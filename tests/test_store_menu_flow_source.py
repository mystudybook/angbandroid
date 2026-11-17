#!/usr/bin/env python3
"""Check scrolling stock popups and empty-store commands remain usable.

Host-C tests cover the helpers; this guards integration in the native menus.
No emulator/physical touchscreen test is performed here.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
menu = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
menu_h = (ROOT / "app/src/main/cpp/angband/ui-menu.h").read_text(encoding="utf-8")
store = (ROOT / "app/src/main/cpp/angband/ui-store.c").read_text(encoding="utf-8")

if "MN_MOUSE_ROW_ACTION = 0x100" not in menu_h:
    raise SystemExit("FAIL: store row mouse callback flag missing")
if "MN_ALLOW_EMPTY_INPUT = 0x80" not in menu_h:
    raise SystemExit("FAIL: missing explicit empty-menu command opt-in")
if "menu->flags |= MN_ALLOW_EMPTY_INPUT | MN_MOUSE_ROW_ACTION;" not in store:
    raise SystemExit("FAIL: empty shop cannot opt into Sell/Help command dispatch")

dispatch = menu.split("static bool menu_handle_action(", 1)[1].split(
    "\n/**\n * Handle navigation keypresses.", 1
)[0]
for fragment in (
    "angdroid_menu_action_item_index(",
    "if (oid < 0 || !is_valid_row(m, m->cursor))",
    "angdroid_menu_allow_empty_input(",
    "in->type == EVT_KBRD || in->type == EVT_MOUSE",
    "row_handler(m, in, -1)",
):
    if fragment not in dispatch:
        raise SystemExit(f"FAIL: empty-stock menu dispatch missing: {fragment}")

if 'angdroid_store_menu_screen_row(' not in store:
    raise SystemExit("FAIL: store uses absolute stock positions for popup")
handler = store.split("static bool store_menu_handle(", 1)[1].split(
    "\nstatic region store_menu_region", 1
)[0]
for fragment in (
    "oid, m->top, m->active.row, m->active.page_rows",
    "popup_row < 0",
    "context_menu_store_item(ctx, oid, 1, popup_row)",
    "event->mouse.y == angdroid_store_menu_screen_row(",
    "angdroid_store_stock_index_valid(",
):
    if fragment not in handler:
        raise SystemExit(f"FAIL: offscreen store popup or unsafe stock cursor: {fragment}")
if "m->active.row + oid" in handler:
    raise SystemExit("FAIL: old unscrolled stock popup row still used")
if "region_inside(&m->active, event) &&" not in handler:
    raise SystemExit("FAIL: an outside/return touch can buy the selected stock row")
if handler.index("region_inside(&m->active, event) &&") > handler.index(
    "event->mouse.y == angdroid_store_menu_screen_row("
):
    raise SystemExit("FAIL: stock mouse row must validate its active bounds")
if "if ((event->mouse.y == 0) || (event->mouse.y == 1))" not in handler:
    raise SystemExit("FAIL: header touch no longer opens the store command popup")

# The popup restores a saved screen even on Cancel/Examine; the shop frame
# must be redrawn after every valid popup, not only after a purchase.
popup_select = handler.split("if (event->type == EVT_SELECT) {", 1)[1].split(
    "} else if (event->type == EVT_MOUSE)", 1
)[0]
if "(void)context_menu_store_item(ctx, oid, 1, popup_row);" not in popup_select:
    raise SystemExit("FAIL: item popup is not dispatched through the guarded selection")
if "if (purchased)" in popup_select:
    raise SystemExit("FAIL: canceled/examined popup leaves stale shop frame")
for item in ("store_display_recalc(ctx);", "store_menu_recalc(m);", "store_redraw(ctx);"):
    if item not in popup_select:
        raise SystemExit(f"FAIL: missing shop repaint after popup: {item}")
if popup_select.index("cmdq_pop(CTX_STORE);") > popup_select.index("store_display_recalc(ctx);"):
    raise SystemExit("FAIL: redraw occurred before stock mutation callbacks")

if "ctx->stock_changed = false;" not in handler:
    raise SystemExit("FAIL: a fresh action never acknowledges the new stock list")

print("PASS: scrolled shop popup, invalidated stock and empty commands safe")
