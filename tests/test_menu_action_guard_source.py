#!/usr/bin/env python3
"""Assert native action menus use the selected item's flags and safe cursor.

Standalone C99 coverage is in tests/native/test_menu_action_guard.c.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ui = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")

if '#include "menu-action-guard.h"' not in ui:
    raise SystemExit("FAIL: missing native menu action guard header")

action = ui.split("static bool menu_action_handle(", 1)[1].split(
    "\n/**\n * Virtual function table", 1
)[0]
if "angdroid_menu_action_can_run(acts[oid].flags," not in action:
    raise SystemExit("FAIL: the selected action's flags are not checked")
if "acts->flags & MN_ACT_GRAYED" in action:
    raise SystemExit("FAIL: first action's disabled flag still governs all rows")

handler = ui.split("static bool menu_handle_action(", 1)[1].split(
    "\n/**\n * Handle navigation keypresses.", 1
)[0]
for part in (
    "angdroid_menu_action_item_index(",
    "m->cursor, count, m->count, m->filter_list",
    "if (oid < 0 || !is_valid_row(m, m->cursor))",
    "return m->row_funcs->row_handler(m, in, oid);",
):
    if part not in handler:
        raise SystemExit(f"FAIL: unsafe menu callback selection: {part}")

if "oid = m->filter_list[m->cursor];" in handler:
    raise SystemExit("FAIL: unchecked raw filter list cursor access")

print("PASS: selected menu actions honor per-item gray state and cursor bounds")
