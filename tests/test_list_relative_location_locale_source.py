#!/usr/bin/env python3
"""Bilingual direction display shared by nearby-monster and object lists."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"

def source(path):
    return (ROOT / path).read_text(encoding="utf-8")

MON = source("app/src/main/cpp/angband/ui-mon-list.c")
OBJ = source("app/src/main/cpp/angband/ui-obj-list.c")
HDR = source("app/src/main/cpp/angband/zh-tw-list-location.h")
NATIVE = source("scripts/test-native-utf8.sh")
VERIFY = source("scripts/verify-zh-tw.sh")
C = source("tests/native/test_list_relative_location_locale.c")

monster = MON.split("static void monster_list_format_section(", 1)[1].split(
    "static bool monster_list_format_special(", 1)[0]
object_list = OBJ.split("static void object_list_format_section(", 1)[1].split(
    "static bool object_list_format_special(", 1)[0]

assert '#include "zh-tw-list-location.h"' in MON
assert '#include "zh-tw-list-location.h"' in OBJ
assert 'angdroid_list_relative_location(location,' in monster
assert 'angdroid_list_relative_location(location, sizeof(location),' in object_list
assert monster.count("angdroid_list_relative_location(") == 1
assert object_list.count("angdroid_list_relative_location(") == 1
assert 'if (list->entries[index].count[section] == 1)' in monster
assert "list->entries[index].dy[section]" in monster
assert "list->entries[index].dx[section]" in monster
assert "list->entries[entry_index].dy" in object_list
assert "list->entries[entry_index].dx" in object_list

# Both lists still reserve the same right-side columns and prioritize names.
for src in (monster, object_list):
    assert "char location[20] = { '\\0' };" in src
    assert "if (!budget.show_location) location[0] = '\\0';" in src
    assert "angdroid_utf8_display_columns(location)" in src
    assert "angdroid_utf8_clip_columns(line_buffer," in src
    assert "angdroid_utf8_printf_field_bytes(" in src
    assert 'textblock_append_c(tb, line_attr, "%-*s%s\\n",' in src
    assert 'strnfmt(location, sizeof(location),' not in src
assert "angdroid_monster_sleep_fits(full_width, asleep_columns)" in monster
assert "object_list_format_name(&list->entries[entry_index]" in object_list

# The original English grammar and zero-direction choices are preserved.
assert '" %u %s %u %s"' in HDR
assert '" %u%s %u%s"' in HDR
assert 'offset_y <= 0 ? "N" : "S"' in HDR
assert 'offset_x <= 0 ? "W" : "E"' in HDR
assert 'offset_y <= 0 ? "北" : "南"' in HDR
assert 'offset_x <= 0 ? "西" : "東"' in HDR
assert "int64_t offset_y = (int64_t)dy;" in HDR
assert "int64_t offset_x = (int64_t)dx;" in HDR
assert "length >= capacity) return;" in HDR
assert "angdroid_utf8_copy_complete(out, capacity, formatted);" in HDR
assert "out[0] = '\\0';" in HDR

# C99 direct-production tests include layout budgets and INT_MIN.
assert "check_location(12, -35," in C
assert "check_location(0, 0," in C
assert "INT_MIN, INT_MAX" in C
assert "angdroid_monster_row_fit(" in C
assert "angdroid_object_row_fit(" in C
assert "angdroid_utf8_display_columns(en)" in C
assert "tests/native/test_list_relative_location_locale.c" in NATIVE
assert "tests/test_list_relative_location_locale_source.py" in VERIFY
print("PASS: monster/object distance locale preserves column and visibility budgets")
