#!/usr/bin/env python3
"""Check object list row widths and popup bounds without running the game."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/angband/ui-obj-list.c").read_text(encoding="utf-8")
LAYOUT = (ROOT / "app/src/main/cpp/angband/object-list-compact-layout.h").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
section = SRC.split("static void object_list_format_section(", 1)[1].split(
    "static bool object_list_format_special(", 1)[0]
interactive = SRC.split("void object_list_show_interactive(", 1)[1]
subwindow = SRC.split("void object_list_show_subwindow(", 1)[1].split(
    "void object_list_show_interactive(", 1)[0]

assert '#include "object-list-compact-layout.h"' in SRC
assert "angdroid_object_row_fit(max_width," in section
assert "if (!budget.show_location) location[0] = '\\0';" in section
assert "angdroid_utf8_clip_columns(line_buffer, full_width);" in section
assert "angdroid_utf8_printf_field_bytes(" in section
assert "object_list_format_name(&list->entries[entry_index]" in section
assert "if (height < 1 || width < 7)" in subwindow
assert "if (height < 7 || width < 12)" in interactive
assert "angdroid_object_list_popup_fit(width, height," in interactive
assert "r.col = popup.col;" in interactive
assert "r.width = safe_width;" in interactive
assert "r.page_rows = safe_height;" in interactive
assert "safe_width = MIN(width - 13" not in interactive
assert "screen_width < 12 || screen_height < 7" in LAYOUT
assert "popup_width < 7" in LAYOUT
assert "available - location_columns >= 8" in LAYOUT
assert "tests/native/test_object_list_compact_layout.c" in RUN
assert "tests/test_object_list_compact_layout_source.py" in VERIFY
print("PASS: object-list narrow-screen layout source contract")
