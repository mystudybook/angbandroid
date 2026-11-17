#!/usr/bin/env python3
"""Enforce display-only UTF-8 item-name prefix handling in the visible list."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
MAIN = (CPP / "obj-list.c").read_text(encoding="utf-8")
UI = (CPP / "ui-obj-list.c").read_text(encoding="utf-8")
HEADER = (CPP / "object-list-name-display.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
fn = MAIN.split("void object_list_format_name(", 1)[1]

assert '#include "object-list-name-display.h"' in MAIN
assert "char name[256];" in fn
assert "if (!line_buffer || size == 0)" in fn
assert "object_desc(name, sizeof(name), base_obj, ODESC_PREFIX | ODESC_FULL |" in fn
assert "ODESC_ALTNUM | (entry->count[field] << 16), player)" in fn
assert "has_singular_prefix || entry->count[field] != 1" in fn
assert "angdroid_object_list_name_copy(line_buffer, size, name," in fn
assert "strtok(" not in fn
assert '"%3.3s "' not in fn
assert "my_strcat(line_buffer, chunk, size)" not in fn
assert "TV_SOFT_ARMOR" in fn and "TV_HARD_ARMOR" in fn
assert "object_list_format_name(&list->entries[entry_index]" in UI
assert "angdroid_utf8_clip_columns(line_buffer, full_width)" in UI
assert "angdroid_utf8_append_complete(out, capacity, used, rest)" in HEADER
assert "angdroid_utf8_copy_complete(out, capacity, name)" in HEADER
assert "strchr(name, ' ')" in HEADER
assert "unsigned char ch" in HEADER
assert "tests/native/test_object_list_name_utf8.c" in NATIVE
assert "tests/test_object_list_name_utf8_source.py" in VERIFY
print("PASS: visible object names preserve complete UTF-8 and original grammar")
