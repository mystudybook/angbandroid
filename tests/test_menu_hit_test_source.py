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

print("PASS: native scroll/object/column menus reject blank touch targets")
