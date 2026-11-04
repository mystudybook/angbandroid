#!/usr/bin/env python3
"""Guard display-width clipping in all native Angband menu text paths.

Host C99 helper tests exercise Unicode boundaries. Source assertions here
are not a replacement for Android APK or screenshot acceptance.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
if '#include "menu-text-fit.h"' not in source:
    raise SystemExit("FAIL: shared native menu text fitter not included")

helper = source.split("static void menu_put_clipped(", 1)[1].split("\n}", 1)[0]
for part in (
    "if (width <= 0 || !text)",
    "angdroid_menu_copy_columns(",
    "Term_putstr(col, row, -1, color, rendered)",
):
    if part not in helper:
        raise SystemExit(f"FAIL: display-cell safe text painter missing {part}")

action = source.split("static void display_action_aux(", 1)[1].split(
    "static char menu_action_tag(", 1
)[0]
if "menu_put_clipped(col, row, wid, color," not in action:
    raise SystemExit("FAIL: translated action title may overflow terminal cells")

string = source.split("static void display_string(", 1)[1].split(
    "static bool handle_string(", 1
)[0]
if "menu_put_clipped(col, row, width, color, items[oid]);" not in string:
    raise SystemExit("FAIL: string row still uses codepoint count as cell count")
if "Term_putstr(col, row, width, color" in string:
    raise SystemExit("FAIL: unsafe direct string menu painter remains")

row = source.split("static void display_menu_row(struct menu *menu, int pos, int top,\n")
if len(row) < 3:
    raise SystemExit("FAIL: menu row definition not found")
row_def = row[-1].split("\n}", 1)[0]
for part in (
    "angdroid_menu_tag_columns(width)",
    "if (sel && width > 0)",
    "if (width > 0)",
):
    if part not in row_def:
        raise SystemExit(f"FAIL: narrow menu tag width unsafe: {part}")
if "width -= 3;" in row_def:
    raise SystemExit("FAIL: tag width can become negative")

refresh = source.split("void menu_refresh(", 1)[1].split(
    "\n/*** MENU RUNNING", 1
)[0]
if refresh.count("menu_put_clipped(") < 3:
    raise SystemExit("FAIL: title/header/prompt bypass display-width clipping")

print("PASS: native action, string, shortcut and decoration text use safe column limits")
