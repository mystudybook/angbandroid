#!/usr/bin/env python3
"""Source-complete FAangband item/ego lookup, locale, and identification guard."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
ANG = ROOT / "app/src/main/cpp/angband"
FA = ROOT / "app/src/main/cpp/faangband"

def read(path):
    return path.read_text(encoding="utf-8")

SOURCE_ITEMS = read(ROOT / "variant_lib/faangband/gamedata/object.txt")
SOURCE_EGO = read(ROOT / "variant_lib/faangband/gamedata/ego_item.txt")
ANG_EGO = read(ROOT / "variant_lib/angband/gamedata/ego_item.txt")
COMMON_KINDS = read(ANG / "zh-tw-items.h")
FA_KINDS = read(ANG / "zh-tw-faangband-items.h")
BOOKS = read(ANG / "zh-tw-class-book-names.h")
COMMON_EGO = read(ANG / "zh-tw-ego-names.h")
FA_EGO = read(FA / "zh-tw-faangband-ego-names.h")
HELPER = read(FA / "obj-desc-item-zh-tw.h")
OBJ = read(FA / "obj-desc.c")
MODE = read(FA / "obj-desc.h")
EXPORT = read(FA / "save-charoutput.c")
SPOILER = read(FA / "ui-spoil.c")
RUNNER = read(ROOT / "scripts/test-native-utf8.sh")
VERIFY = read(ROOT / "scripts/verify-zh-tw.sh")

def source_names(source):
    return re.findall(r"^name:(.*)$", source, re.M)

def entries(source, cols):
    string = r'"([^"\n]+)"'
    pattern = r"^\s*\{\s*" + r",\s*".join([string] * cols) + r"\s*\},?$"
    return re.findall(pattern, source, re.M)

fa_items = source_names(SOURCE_ITEMS)
fa_ego = set(source_names(SOURCE_EGO))
ang_ego = set(source_names(ANG_EGO))
common_kinds = entries(COMMON_KINDS, 2)
fa_kinds = entries(FA_KINDS, 3)
books = entries(BOOKS, 3)
common_ego = entries(COMMON_EGO, 2)
fa_ego_entries = entries(FA_EGO, 2)

assert len(fa_items) == 363
assert len(set(fa_items)) == 332
assert len(common_kinds) == 326
assert len(fa_kinds) == 39
assert len(books) == 20
item_keys = {x[0] for x in common_kinds + fa_kinds + books}
assert set(fa_items) - item_keys == {
    "<pile>", "<unknown item>", "<unknown treasure>", "<curse object>"
}

assert len(fa_ego) == 118
assert len(ang_ego) == 79
assert len(common_ego) == 79
assert len(fa_ego_entries) == 66
assert {x[0] for x in common_ego} == ang_ego
assert {x[0] for x in fa_ego_entries} == fa_ego - ang_ego
assert {x[0] for x in common_ego + fa_ego_entries} >= fa_ego
assert all(name and translated and translated != name for name, translated
           in fa_ego_entries)
assert len({name for name, _ in fa_ego_entries}) == 66
assert "ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES 66" in FA_EGO

# Resolvers never touch game data or grant identification.
assert 'angdroid_zh_tw_item_kind_name(english)' in HELPER
assert 'angdroid_zh_tw_class_book_name(english, tval)' in HELPER
assert 'angdroid_zh_tw_faangband_item_kind_name(english, tval)' in HELPER
assert 'angdroid_zh_tw_ego_name(english)' in HELPER
assert 'angdroid_zh_tw_faangband_ego_name(english)' in HELPER
assert "if (!english || !tval || !translate) return NULL;" in HELPER
assert "if (!english || !translate) return NULL;" in HELPER

start = OBJ.index("static size_t obj_desc_name(")
stop = OBJ.index("static bool obj_desc_show_armor(", start)
section = OBJ[start:stop]
assert "ODESC_ZH_TW = 0x800" in MODE
assert "!(mode & (ODESC_SPOIL | ODESC_TERSE))" in section
assert "angdroid_zh_tw_enabled()" in section
assert "bool aware = object_flavor_is_aware(obj) || store || spoil;" in section
assert "show_flavor = !terse && (obj->kind->flavor || tval_is_jewelry(obj))" in section
assert "if (store || (aware && p && !OPT(p, show_flavors)))" in section
assert "if (translated_ui && aware && !show_flavor && obj->kind->name)" in section
assert "angdroid_faangband_known_kind_ui_name(" in section
assert "angdroid_zh_tw_identified_flavor_name(" in section
assert "angdroid_zh_tw_known_item_name(" in section
assert "translated_base = angdroid_obj_desc_append_suffix_atomic(" in section
assert "if (!translated_base) {" in section
assert "end = obj_desc_name_format(buf, max, end, basename, modstr, plural);" in section
assert "if (object_is_known_artifact(obj)) {" in section
assert "obj->known->ego && !(mode & ODESC_NOEGO)" in section
assert "obj->ego && store" in section
assert "angdroid_faangband_known_ego_ui_name(" in section
assert "angdroid_zh_tw_item_suffix(" in section
assert "angdroid_obj_desc_append_suffix_atomic(" in section
assert "else if (!translated_base && aware && !obj->artifact &&" in section
assert 'strnfcat(buf, max, &end, " %s", ego_name);' in section
# When a known effect has already become a complete Chinese kind name, the
# final English "of ..." suffix must not be appended a second time.
assert section.index("if (!translated_base) {") < section.index(
    "else if (!translated_base && aware && !obj->artifact &&"
)


# User-facing source is only opted-in; terse/English/export/spoiler stay as is.
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in SPOILER
assert "test_faangband_item_ego_ui.c" in RUNNER
assert "test_faangband_item_ego_ui_source.py" in VERIFY
print("PASS: 359/363 source items (4 system placeholders), 118 ego labels")
