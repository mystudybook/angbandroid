#!/usr/bin/env python3
"""Check native navigation no longer loops forever on invalid horizontal rows."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")

if '#include "menu-valid-cursor.h"' not in source:
    raise SystemExit("FAIL: menu cursor helper not included")

section = source.split("bool menu_handle_keypress(", 1)[1].split(
    "static void keys_to_ui(", 1
)[0]
required = (
    "int direction = ddy[dir] ? ddy[dir] : ddx[dir];",
    "angdroid_menu_find_valid_cursor(menu->cursor,",
    "menu_navigation_is_valid",
    "if (next >= 0)",
    "out->type = EVT_NONE;",
)
for fragment in required:
    if fragment not in section:
        raise SystemExit(f"FAIL: missing finite menu navigation guard: {fragment}")
if "menu->cursor += ddy[dir];" in section:
    raise SystemExit("FAIL: zero vertical step could hang a horizontal menu move")

# Horizontal/vertical arrows must skip hidden rows just as FAangband does.
adapter = source.split(
    "static int menu_navigation_is_valid(void *context, int row)\\n{", 1
)[1].split("\\n}\\n", 1)[0]
if "menu->row_funcs->valid_row(menu, oid) == MN_ROW_VALID" not in adapter:
    raise SystemExit("FAIL: hidden rows can be reached by arrow keys")
nonav = source.split("static bool no_valid_row(", 1)[1].split("\\n}\\n", 1)[0]
if "menu_navigation_is_valid(menu, i)" not in nonav:
    raise SystemExit("FAIL: hidden-only menus count as navigable")

# The old >= total-1 test skipped the actual final row of a page.
if "angdroid_menu_page_next_cursor(menu->cursor," not in section:
    raise SystemExit("FAIL: Spacebar paging can skip the final menu row")
if "target, count, 1, menu, menu_navigation_is_valid" not in section:
    raise SystemExit("FAIL: Spacebar can leave the cursor on a disabled row")
if "menu->top = next;" not in section:
    raise SystemExit("FAIL: page top not aligned with selected cursor")
if "menu->cursor >= total - 1" in section:
    raise SystemExit("FAIL: Spacebar still wraps before the last menu item")

repair = source.split("void menu_ensure_cursor_valid(struct menu *m)", 1)[1].split(
    "/* ======================== MENU INITIALIZATION", 1
)[0]
for item in (
    "if (count <= 0)",
    "start = m->cursor < 0 ? 0 :",
    "angdroid_menu_find_valid_cursor(start, count, 1,",
    "m->cursor = next;",
    "m->top = 0;",
    "m->top >= count",
):
    if item not in repair:
        raise SystemExit(f"FAIL: stock update may leave an invalid selection: {item}")

# Enter must not emit SELECT after the stock list was invalidated.
if "if (is_valid_row(menu, menu->cursor))" not in section:
    raise SystemExit("FAIL: Enter with a cleared cursor exits or selects an item")
if "else\n\t\t\teat = true;" not in section:
    raise SystemExit("FAIL: invalid Enter still escapes the menu")

print("PASS: native menu direction, paging, Enter and cursor recovery")
