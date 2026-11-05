#!/usr/bin/env python3
"""Ensure Chinese inventory item names are displayed without corrupting data.

Pure terminal-column arithmetic is exercised by test_object_list_text.c.
This static integration test does not run the Android or native game engine.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-object.c").read_text(encoding="utf-8")

if '#include "object-list-text.h"' not in source:
    raise SystemExit("FAIL: inventory column calculation helper is missing")

start = source.index("static void show_obj(")
end = source.index("/**\n * ------------------------------------------------------------------------\n * Display of lists", start)
render = source[start:end]

checks = (
    "olist_detail_t mode, int row_width)",
    "angdroid_object_list_cell_budget(ex_offset, row_width)",
    "char name[sizeof(items[obj_num].o_name)]",
    "angdroid_menu_copy_columns(name, sizeof(name), items[obj_num].o_name,",
    "c_put_str(attr, name, row + obj_num,",
)
for pattern in checks:
    if pattern not in render:
        raise SystemExit(f"FAIL: inventory name rendering is not column-safe: {pattern}")
if "items[obj_num].o_name[truncate]" in render:
    raise SystemExit("FAIL: displaying a narrow menu mutates cached object text")

width_calculation = source.split("static void set_obj_names(", 1)[1].split(
    "static void show_obj_list(", 1
)[0]
if "angdroid_utf8_display_columns(items[i].o_name)" not in width_calculation:
    raise SystemExit("FAIL: longest object name still measured in UTF-8 bytes")

if source.count("angdroid_object_list_name_offset(") != 2:
    raise SystemExit("FAIL: list and selection view calculate widths differently")
if "show_obj(oid, row - oid, col, cursor, olist_render_mode, width);" not in source:
    raise SystemExit("FAIL: inventory selection ignores its drawable row width")
if "show_obj(i, row, col, false, mode, Term->wid - col);" not in source:
    raise SystemExit("FAIL: inventory panel does not bound drawable row width")
if "angdroid_utf8_display_columns(header)" not in source or (
    "angdroid_item_selection_region(width, height," not in source or
    "area.width = layout.width;" not in source
):
    raise SystemExit("FAIL: item selection fails to resize with terminal geometry")
if "get_item_action, item_menu_resize };" not in source:
    raise SystemExit("FAIL: rotated item selection lacks a layout callback")
if "angdroid_item_selection_footer_row(local_area->row," not in source:
    raise SystemExit("FAIL: quiver footer may cover selectable item rows")
if "local_area->row + i" in source:
    raise SystemExit("FAIL: inventory footer still uses unbounded item count")
if "if (z_info->quiver_slot_size <= 0) return;" not in source:
    raise SystemExit("FAIL: quiver slot count may divide by zero")

if "static olist_detail_t object_list_visible_extras(" not in source:
    raise SystemExit("FAIL: inventory numeric-column fitting policy is missing")
if 'olist_render_mode = object_list_visible_extras(olist_mode, Term->wid);' not in source:
    raise SystemExit("FAIL: item-selection list bypasses the responsive detail policy")
if "mode = object_list_visible_extras(mode, Term->wid);" not in source:
    raise SystemExit("FAIL: inventory panel bypasses the responsive detail policy")
if "if (mode & OLIST_RECHARGE) ex_width += 10;" not in source:
    raise SystemExit("FAIL: recharge chance column is not reserved in inventory")
if "if (olist_render_mode & OLIST_RECHARGE) ex_width += 10;" not in source:
    raise SystemExit("FAIL: recharge chance column is not reserved in selection")
if "angdroid_object_list_detail_fits(offset, row_width," not in source:
    raise SystemExit("FAIL: a numeric detail may be clipped to a misleading number")
if source.count("object_list_put_detail(buf, row + obj_num, col,") != 4:
    raise SystemExit("FAIL: price, failure, recharge and weight need row bounds")
if "char o_name[256];" not in source:
    raise SystemExit("FAIL: item cached UTF-8 names are still constrained to 80 bytes")


print("PASS: inventory and equipment names are clipped by cells without mutation")
