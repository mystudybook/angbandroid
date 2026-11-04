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

if "MN_ALLOW_EMPTY_INPUT = 0x80" not in menu_h:
    raise SystemExit("FAIL: missing explicit empty-menu command opt-in")
if "menu->flags |= MN_ALLOW_EMPTY_INPUT;" not in store:
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

print("PASS: scrolled shop item popups and stock-free commands remain safe")
