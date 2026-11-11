#!/usr/bin/env python3
"""Guard native Angband menu touch mapping against blank-cell selection.

The hit-test math is exercised independently by test_menu_hit_test.c.
This check is not an Android emulator or touchscreen test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")

if '#include "menu-hit-test.h"' not in source:
    raise SystemExit("FAIL: native hit-test helpers are not included")

for fn in ("scrolling_get_cursor", "object_skin_get_cursor"):
    body = source.split(f"static int {fn}(", 1)[1].split("\n}", 1)[0]
    if "angdroid_menu_scrolling_hit(" not in body:
        raise SystemExit(f"FAIL: {fn} still maps blank rows to a selection")
    if "if (cursor >= n) cursor = n - 1;" in body:
        raise SystemExit(f"FAIL: {fn} still clamps blank row to last item")

column = source.split("static int columns_get_cursor(", 1)[1].split(
    "static void display_columns(", 1
)[0]
if "return angdroid_menu_columns_hit(" not in column:
    raise SystemExit("FAIL: column touch mapping does not reject empty cells")
if "if (cursor >= n) cursor = n - 1;" in column:
    raise SystemExit("FAIL: column touch still selects last item on blank cells")

mouse = source.split("bool menu_handle_mouse(", 1)[1].split(
    "static bool menu_handle_action(", 1
)[0]
if "if (is_valid_row(menu, new_cursor))" not in mouse:
    raise SystemExit("FAIL: invalid touch hit can select an action")

# The current touch must resolve its new stock row BEFORE dispatching.
# Header/outside actions must still run independently of the active list.
select = source.split("ui_event menu_select(", 1)[1].split(
    "\n/* ================== MENU ACCESSORS", 1
)[0]
inside = select.split("if (in.type == EVT_MOUSE) {", 1)[1].split(
    "} else if (in.type == EVT_KBRD)", 1
)[0]
for phrase in (
    "if (region_inside(&menu->active, &in)) {",
    "menu_handle_mouse(menu, &in, &out);",
    "if (out.type == EVT_SELECT && !no_act &&",
    "menu_handle_action(menu, &in))",
    "} else {",
    "if (!no_act && menu_handle_action(menu, &in))",
):
    if phrase not in inside:
        raise SystemExit(f"FAIL: touch row may dispatch stale selection: {phrase}")
if inside.index("menu_handle_mouse(menu, &in, &out);") > inside.index(
    "if (out.type == EVT_SELECT && !no_act &&"
):
    raise SystemExit("FAIL: touch action runs before current row hit-test")
if "(menu->flags & MN_MOUSE_ROW_ACTION)" not in inside:
    raise SystemExit("FAIL: raw mouse event must be opt-in for stock menus")
if "touch_select_dispatched = true;" not in inside:
    raise SystemExit("FAIL: one touch can dispatch an action twice")
if "!touch_select_dispatched &&" not in select:
    raise SystemExit("FAIL: event fallback can dispatch the same action twice")
if inside.count("menu_handle_mouse(menu, &in, &out);") != 2:
    raise SystemExit("FAIL: header and item touch paths are inconsistent")
if "if (new_cursor == menu->cursor || !(menu->flags & MN_DBL_TAP))" not in mouse:
    raise SystemExit("FAIL: first tap of a new row bypasses double-tap setting")
if "if (in->mouse.button == 2)" not in mouse:
    raise SystemExit("FAIL: touchscreen right-click/return path missing")

print("PASS: native touch dispatch resolves current row before action")
