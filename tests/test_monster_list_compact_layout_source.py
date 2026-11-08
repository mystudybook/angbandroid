#!/usr/bin/env python3
"""Source-level invariants for monster-list narrow terminal geometry."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/angband/ui-mon-list.c").read_text(encoding="utf-8")
HELPER = (ROOT / "app/src/main/cpp/angband/monster-list-compact-layout.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

section = SRC.split("static void monster_list_format_section(", 1)[1].split(
    "static bool monster_list_format_special(", 1)[0]
interactive = SRC.split("void monster_list_show_interactive(", 1)[1].split(
    "void monster_list_force_subwindow_update(", 1)[0]
subwindow = SRC.split("void monster_list_show_subwindow(", 1)[1].split(
    "void monster_list_show_interactive(", 1)[0]

assert '#include "monster-list-compact-layout.h"' in SRC
assert "angdroid_monster_row_fit(max_width," in section
assert "if (!budget.show_location) location[0] = '\\0';" in section
assert "angdroid_monster_sleep_fits(full_width, asleep_columns)" in section
assert "angdroid_utf8_clip_columns(line_buffer, name_width)" in section
assert "angdroid_utf8_printf_field_bytes(" in section
assert "textblock_append_c(tb, line_attr, \"%-*s%s\\n\"," in section
assert "if (height < 1 || width < 7)" in subwindow
assert "if (height < 7 || width < 12)" in interactive
assert "angdroid_monster_list_popup_fit(width, height," in interactive
assert "r.col = -safe_width;" in interactive
assert "r.width = safe_width;" in interactive
assert "r.page_rows = safe_height;" in interactive
assert "safe_width = MIN(width - 40" not in interactive
assert "screen_width < 12 || screen_height < 7" in HELPER
assert "body_rows = preferred_height > 0 ? preferred_height : 1;" in HELPER
assert "content - location_columns >= 4" in HELPER
assert 'test_monster_list_compact_layout.c' in RUNNER
assert 'test_monster_list_compact_layout_source.py' in VERIFY
print("PASS: monster-list narrow screen source contract")
