#!/usr/bin/env python3
"""Check live native menu rendering uses bounded resize geometry.

Pure C99 layout math is tested by test_menu_compact_layout.c.
Source assertions do not replace an Android emulator rotation test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
menu_header = (ROOT / "app/src/main/cpp/angband/ui-menu.h").read_text(encoding="utf-8")

if '#include "menu-compact-layout.h"' not in source:
    raise SystemExit("FAIL: compact native layout helper not included")
if "const menu_skin *preferred_skin;" not in menu_header:
    raise SystemExit("FAIL: original column skin cannot be restored after rotation")
if "menu->preferred_skin = skin;" not in source:
    raise SystemExit("FAIL: menu initialization does not preserve original skin")

calculator = source.split("static bool menu_calc_size(struct menu *menu)\n{", 1)[1].split(
    "\nbool menu_layout(", 1
)[0]
for symbol in (
    "angdroid_menu_compute_layout(",
    "menu->active.width = bounded.width;",
    "menu->active.page_rows = bounded.page_rows;",
    "menu->title_row = bounded.title_row;",
    "menu->header_row = bounded.header_row;",
    "menu->prompt_row = bounded.prompt_row;",
    "menu_choose_skin(menu);",
):
    if symbol not in calculator:
        raise SystemExit(f"FAIL: resize layout not bounded: {symbol}")

refresh = source.split("void menu_refresh(struct menu *menu, bool reset_screen)", 1)[1].split(
    "\n/*** MENU RUNNING", 1
)[0]
for symbol in ("menu_choose_skin(menu);", "menu->title_row >= 0",
               "menu->header_row >= 0", "menu->prompt_row >= 0"):
    if symbol not in refresh:
        raise SystemExit(f"FAIL: native refresh ignores compact layout: {symbol}")

choose = source.split("static void menu_choose_skin(", 1)[1].split(
    "\nvoid menu_refresh(", 1
)[0]
if "angdroid_menu_use_compact_columns(" not in choose:
    raise SystemExit("FAIL: no narrow-screen column fallback")
if "menu->skin = &menu_skin_scroll;" not in choose:
    raise SystemExit("FAIL: narrow-screen menu does not reflow")
if "menu->top = 0;" not in choose:
    raise SystemExit("FAIL: switching back to columns leaves stale scroll offset")

run = source.split("ui_event menu_select(", 1)[1].split(
    "\n/* ================== MENU ACCESSORS", 1
)[0]
if "menu->active.width <= 0 || menu->active.page_rows <= 0" not in run:
    raise SystemExit("FAIL: zero-size menu surface can enter input loop")
if "if (!menu_calc_size(menu))" not in run:
    raise SystemExit("FAIL: resize error is ignored")
if "menu->preferred_skin == &menu_skin_object" not in run:
    raise SystemExit("FAIL: rotated object menu cannot recover its stale location")
if "resized_before_calc = true;" not in run:
    raise SystemExit("FAIL: resized object menu callback can be repeated")

print("PASS: native menu layout and terminal resize use bounded geometry")
