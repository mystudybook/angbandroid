#!/usr/bin/env python3
"""FAangband flavor source coverage and identified/unknown display guards."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
ANG = ROOT / "app/src/main/cpp/angband"
FA = ROOT / "app/src/main/cpp/faangband"

def read(path):
    return path.read_text(encoding="utf-8")

SOURCE = read(ROOT / "variant_lib/faangband/gamedata/flavor.txt")
STD = read(ANG / "zh-tw-flavor-names.h")
ADDED = read(FA / "zh-tw-faangband-flavor-names.h")
HELPER = read(FA / "obj-desc-flavor-zh-tw.h")
ITEM = read(FA / "obj-desc-item-zh-tw.h")
OBJ = read(FA / "obj-desc.c")
EXPORT = read(FA / "save-charoutput.c")
SPOILER = read(FA / "ui-spoil.c")
FLAVOR_RUNTIME = read(FA / "obj-util.c")
RUNNER = read(ROOT / "scripts/test-native-utf8.sh")
VERIFY = read(ROOT / "scripts/verify-zh-tw.sh")
C_TEST = read(ROOT / "tests/native/test_faangband_flavor_ui.c")

group = None
flavors = []
blank_scrolls = 0
for line in SOURCE.splitlines():
    if line.startswith("kind:"):
        group = line.split(":")[1]
    if line.startswith("flavor:"):
        segments = line.split(":")
        if len(segments) == 3:
            assert group == "scroll", line
            blank_scrolls += 1
        elif len(segments) >= 4:
            flavors.append((group, ":".join(segments[3:])))
        else:
            raise AssertionError(line)

names = {name for _, name in flavors}
translated = dict(re.findall(
    r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},?$',
    STD, re.M))
assert len(flavors) == 186
assert len(names) == 155
assert len(translated) == 202
assert blank_scrolls == 51
assert len(flavors) + blank_scrolls == 237
assert names - translated.keys() == {"Adamantium", "Damascene Steel"}
assert 'strcmp(english, "Adamantium") == 0' in ADDED
assert 'strcmp(english, "Damascene Steel") == 0' in ADDED
assert 'return "精金";' in ADDED
assert 'return "大馬士革鋼";' in ADDED

# Source flavor is always the native assigned source pointer.  The UI
# helper must not read, change or recompute the random flavor assignment.
assert 'angdroid_zh_tw_flavor_name(english)' in HELPER
assert 'angdroid_zh_tw_faangband_flavor_name(english)' in HELPER
assert 'return translated ? translated : english;' in HELPER
assert 'if (aware && source_kind && *source_kind) {' in HELPER
assert 'if (!effect) effect = source_kind;' in HELPER
assert 'angdroid_zh_tw_flavored_object(out, capacity, tval,' in HELPER
assert 'angdroid_zh_tw_scroll_title(' in HELPER
assert 'angdroid_zh_tw_known_item_name(' in HELPER
assert 'angdroid_zh_tw_item_suffix(' in HELPER
assert 'if (aware && source_kind && *source_kind) {' in HELPER
assert 'memcpy(out, name, length + 1);' in HELPER
assert 'angdroid_faangband_known_kind_ui_name(' in ITEM

start = OBJ.index("static size_t obj_desc_name(")
end = OBJ.index("static bool obj_desc_show_armor(", start)
block = OBJ[start:end]
assert '#include "obj-desc-flavor-zh-tw.h"' in OBJ
assert "!(mode & (ODESC_SPOIL | ODESC_TERSE))" in block
assert 'angdroid_zh_tw_enabled()' in block
assert "bool aware = object_flavor_is_aware(obj) || store || spoil;" in block
assert "show_flavor = !terse && (obj->kind->flavor || tval_is_jewelry(obj))" in block
assert "if (store || (aware && p && !OPT(p, show_flavors)))" in block
assert "if (translated_ui && show_flavor && obj->kind->flavor &&" in block
assert "!obj->artifact && !obj->ego" in block
assert "if (obj->tval == TV_SCROLL)" in block
assert "angdroid_faangband_ui_scroll(" in block
assert "angdroid_faangband_ui_flavored_object(" in block
assert "obj->kind->flavor->text, obj->kind->name, aware," in block
assert "translated_base = angdroid_obj_desc_append_suffix_atomic(" in block
assert "if (localized_kind && !translated_base) {" in block
assert "if (!translated_base) {" in block
assert "end = obj_desc_name_format(buf, max, end," in block
assert "basename, modstr, plural);" in block
assert "else if (!translated_base && aware && !obj->artifact &&" in block
assert "if (object_is_known_artifact(obj)) {" in block
assert "obj->known->ego && !(mode & ODESC_NOEGO)" in block

# No translatable effect can be emitted before knowing the kind.  Random
# scroll titles are not regenerated and are passed in from Native state.
assert "if (!source_appearance || !*source_appearance || !tval) return false;" in HELPER
assert 'source_kind, tval, true);' in HELPER
assert 'source_kind, "scroll", true);' in HELPER
assert "if (!angdroid_zh_tw_scroll_title(" in HELPER
assert "if (length >= capacity) return false;" in HELPER
# Source implementation ties the assigned scroll flavor to the existing
# per-game scroll_adj array, populated under seed_flavor.  Display code
# must not call flavor_init(), randname_make() or the RNG.
assert 'Rand_value = seed_flavor;' in FLAVOR_RUNTIME
assert 'f->text = scroll_adj[k_info[i].sval];' in FLAVOR_RUNTIME
assert 'randname_make(RANDNAME_SCROLL' in FLAVOR_RUNTIME
assert 'scroll_adj[MAX_TITLES][18]' in FLAVOR_RUNTIME
assert 'seed_flavor' not in OBJ
assert 'randname_make(' not in HELPER
assert 'flavor_init(' not in OBJ
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in SPOILER
assert "test_faangband_flavor_ui.c" in RUNNER
assert "test_faangband_flavor_ui_source.py" in VERIFY
assert C_TEST.count('        "') >= 155
assert "CHECK(strstr(text, \"治療\") == NULL);" in C_TEST
assert 'CHECK(tiny[0] == \'\\0\');' in C_TEST

print("PASS: 155 FA flavor names (153 common + 2 exclusive), 51 runtime scroll slots")
