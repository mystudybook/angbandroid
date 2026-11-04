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
if "show_obj(oid, row - oid, col, cursor, olist_mode, width);" not in source:
    raise SystemExit("FAIL: inventory selection ignores its drawable row width")
if "show_obj(i, row, col, false, mode, Term->wid - col);" not in source:
    raise SystemExit("FAIL: inventory panel does not bound drawable row width")
if "area.width = (int)MAX(max_len, angdroid_utf8_display_columns(header));" not in source:
    raise SystemExit("FAIL: object menu header still measured in bytes")

print("PASS: inventory and equipment names are clipped by cells without mutation")
