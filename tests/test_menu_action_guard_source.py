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

# All generic menu paths must translate visible indices through the same bounds
# check before invoking row callbacks, including rendering and soft keys.
sections = {
    "row validity": ui.split("static bool is_valid_row(struct menu *menu, int cursor)", 1)[1].split(
        "static bool no_valid_row", 1
    )[0],
    "private tags": ui.split("static int get_cursor_key(", 1)[1].split(
        "static menu_row_style_t", 1
    )[0],
    "row rendering": ui.split("static void display_menu_row(", 2)[-1].split(
        "static void menu_choose_skin", 1
    )[0],
    "screen refresh": ui.split("void menu_refresh(", 1)[1].split(
        "/*** MENU RUNNING AND INPUT HANDLING CODE", 1
    )[0],
    "keyboard hints": ui.split("static void keys_to_ui(", 1)[1].split(
        "/**\n * Run a menu.", 1
    )[0],
}
for label, section in sections.items():
    if "angdroid_menu_action_item_index(" not in section:
        raise SystemExit(f"FAIL: {label} passes an unchecked filtered row to callbacks")
for label in ("row validity", "private tags", "row rendering", "keyboard hints"):
    if "if (oid < 0)" not in sections[label]:
        raise SystemExit(f"FAIL: {label} does not reject invalid mapped indices")
if "menu->browse_hook && oid >= 0" not in sections["screen refresh"]:
    raise SystemExit("FAIL: browse hook must not receive a stale source row")

print("PASS: selected menu actions and all generic filtered row callbacks are bounded")
