#!/usr/bin/env python3
"""Ensure dynamic popups use the bounded terminal-cell placement helper.

C99 tests cover coordinate arithmetic. This checks the native menu call path,
not actual Android touch or renderer behavior.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
menu = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
header = (ROOT / "app/src/main/cpp/angband/menu-popup-layout.h").read_text(encoding="utf-8")

if '#include "menu-popup-layout.h"' not in menu:
    raise SystemExit("FAIL: dynamic popups do not include layout guard")
if "static inline int angdroid_menu_popup_bounds(" not in header:
    raise SystemExit("FAIL: portable popup layout missing")

location = menu.split("void menu_dynamic_calc_location(", 1)[1].split(
    "\nint menu_dynamic_select_alias(", 1
)[0]
for fragment in (
    "region r = {0, 0, 0, 0};",
    "angdroid_menu_popup_box popup;",
    "angdroid_menu_popup_bounds(",
    "Term->wid, Term->hgt, mx, my, menu_dynamic_longest_entry(m)",
    "m->count, &popup",
    "r.col = popup.col;",
    "r.row = popup.row;",
    "r.width = popup.width;",
    "r.page_rows = popup.page_rows;",
    "menu_layout(m, &r);",
):
    if fragment not in location:
        raise SystemExit(f"FAIL: popup geometry integration is missing: {fragment}")

for old in (
    "r.col = Term->wid - r.width - 1;",
    "r.row = Term->hgt - r.page_rows - 1;",
    "r.page_rows = m->count;",
    "r.col = mx + 1;",
):
    if old in location:
        raise SystemExit(f"FAIL: unbounded popup placement survives: {old}")

if "menu_dynamic_calc_location(m, mx, my);" not in (
    ROOT / "app/src/main/cpp/angband/ui-store.c"
).read_text(encoding="utf-8"):
    raise SystemExit("FAIL: store popup path no longer invokes the layout")

if "menu_dynamic_select(m, 0, true);" not in menu:
    # Not a regression indicator; modal operation runs through menu_select
    if "ui_event e = menu_select(m, 0, true);" not in menu:
        raise SystemExit("FAIL: modal popup selection path missing")

print("PASS: native dynamic menu placement is bounded and store popups use it")
