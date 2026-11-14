#!/usr/bin/env python3
"""Artifact knowledge list titles must not truncate CJK bytes or overwrite columns."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/angband/ui-knowledge.c")
COLUMNS = read("app/src/main/cpp/angband/knowledge-column-text.h")
DISPLAY = read("app/src/main/cpp/angband/menu-text-fit.h")
NATIVE = read("tests/native/test_knowledge_column_text.c")
RUNNER = read("scripts/test-native-utf8.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

def body(first, last):
    return SOURCE.split(first, 1)[1].split(last, 1)[0]

builder = body("static void get_artifact_display_name(", "static void display_artifact(")
row = body("static void display_artifact(", "static struct object *find_artifact(")
lore = body("static void desc_art_fake(", "static int a_cmp_tval(")

# The artifact is still constructed as a temporary fake-known item, using
# the same original identification and canonical ODESC flag choices.
assert "make_fake_artifact(obj, &a_info[a_idx]);" in builder
assert "object_copy(known_obj, obj);" in builder
assert "ODESC_PREFIX | ODESC_BASE | ODESC_SPOIL | ODESC_ZH_TW" in builder
assert "object_wipe(known_obj);" in builder
assert "object_wipe(obj);" in builder

assert "char o_name[512];" in row
assert "char clipped[256];" in row
assert "get_artifact_display_name(o_name, sizeof o_name, oid);" in row
assert "if (Term && col >= 0 && col < Term->wid)" in row
assert "angdroid_knowledge_clip_label(clipped, sizeof clipped," in row
assert "o_name, col, Term->wid, Term->wid" in row
assert "if (clipped[0]) c_prt(attr, clipped, row, col);" in row
assert "c_prt(attr, o_name, row, col);" not in row
assert "char o_name[80];" not in row

# Full artifact recall is not limited to the 80-byte list buffer either.
assert "char header[512];" in lore
assert "object_desc(header, sizeof(header), obj," in lore
assert "ODESC_PREFIX | ODESC_FULL | ODESC_CAPITAL | ODESC_ZH_TW" in lore
assert "textui_textblock_show(tb, area, header);" in lore
assert "history_is_artifact_known(player, obj->artifact)" in lore
assert "object_copy(known_obj, obj);" in lore

# Clip against visible columns, complete UTF-8 scalars, and destination
# capacity; never use UTF-8 byte length to position the artifact row.
assert "angdroid_menu_copy_columns(dest, capacity, label, available)" in COLUMNS
assert "col >= 0 && col < terminal_columns && last > col" in COLUMNS
assert "angdroid_utf8_next_scalar(source, &codepoint)" in DISPLAY
assert "angdroid_terminal_columns(codepoint)" in DISPLAY
assert "length >= capacity - bytes" in DISPLAY
assert "columns = angdroid_knowledge_clip_label(" in NATIVE
assert 'strcmp(clipped, "失落王國") == 0' in NATIVE
assert 'strcmp(short_buffer, "失落") == 0' in NATIVE
assert "tests/native/test_knowledge_column_text.c" in RUNNER
assert "tests/test_artifact_knowledge_display_source.py" in VERIFY
print("PASS: artifact knowledge list and recall use full CJK-safe source rendering")
