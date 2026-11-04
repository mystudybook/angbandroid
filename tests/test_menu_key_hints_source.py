#!/usr/bin/env python3
"""Static integration guard for Angband's native soft-keyboard menu hints.

Runtime behavior of the isolated helpers is tested by test_menu_key_hints.c.
This source check does not substitute for an Android engine build.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
function = source.split("static void keys_to_ui(struct menu *menu)", 1)[1].split(
    "\n/**\n * Run a menu.", 1
)[0]

checks = {
    "filter item indexes": "angdroid_menu_hint_item_index(menu->filter_list, cursor)",
    "bounded hint append": "angdroid_menu_hint_append(temp, current,",
    "hint buffer size": "sizeof(temp), sel)",
    "preserve visible row tags": "menu->skin->get_tag(menu, cursor)",
    "preserve original item keys": "menu->row_funcs->get_tag(menu, oid)",
}
for name, fragment in checks.items():
    if fragment not in function:
        raise SystemExit(f"FAIL: native menu hint integration missing {name}")

for forbidden in ("menu->filter_list[oid]", "temp[current++]"):
    if forbidden in function:
        raise SystemExit(f"FAIL: unsafe legacy shortcut hint pattern: {forbidden}")

print("PASS: filtered native menu hint source integration")
