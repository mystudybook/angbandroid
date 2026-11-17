#!/usr/bin/env python3
"""Display-only monster source labels remain complete for item provenance."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

INFO = read("app/src/main/cpp/angband/obj-info.c")
DISPLAY = read("app/src/main/cpp/angband/item-origin-display.h")
ANG = read("variant_lib/angband/gamedata/monster.txt")
FA = read("variant_lib/faangband/gamedata/monster.txt")
NATIVE = read("tests/native/test_item_origin_monster_name.c")
RUNNER = read("scripts/test-native-utf8.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

origin = INFO.split("static bool describe_origin(", 1)[1].split(
    "static void describe_flavor_text(", 1)[0]
assert '#include "item-origin-display.h"' in INFO
assert "char name_buffer[256];" in origin
assert "const char *name = name_buffer;" in origin
assert "angdroid_item_origin_monster_name(name_buffer," in origin
assert "sizeof(name_buffer), dropper, article, unique, comma)" in origin
assert "name = dropper;" in origin
assert 'article = is_a_vowel(dropper[0]) ? "an " : "a ";' in origin
assert "rf_has(obj->origin_race->flags, RF_UNIQUE)" in origin
assert "rf_has(obj->origin_race->flags, RF_NAME_COMMA)" in origin
assert 'dropper = "monster lost to history";' in origin
assert "if (terse && !obj_can_wear(obj))" in origin
assert "obj->origin == ORIGIN_DROP_MIMIC" in origin
assert "origin = ORIGIN_FLOOR;" in origin
assert 'textblock_append(tb, origins[origin].desc, name, loot_spot)' in origin
assert "textblock_append(tb, \"由%s在%s掉落\", zh_dropper, zh_spot)" in origin
assert "angdroid_zh_tw_unique_monster_name(dropper)" in origin
assert "angdroid_zh_tw_monster_list_name(dropper)" in origin
assert "if (found) zh_dropper = found;" in origin
assert 'zh_dropper = "已不可考的怪物";' in origin
assert "obj->origin_depth * 50" in origin
assert "my_strcat(name, dropper," not in origin
assert "char name[80];" not in origin

# Entire English source is copied or the helper returns failure, and the
# production display uses the complete original source as its fallback.
assert "memcpy(out + prefix_len, source, source_len);" in DISPLAY
assert "if (prefix_len >= capacity ||" in DISPLAY
assert "source_len >= capacity - prefix_len - suffix_len" in DISPLAY
assert "out[0] = '\\0';" in DISPLAY
assert "if (needs_comma) out[prefix_len + source_len] = ',';" in DISPLAY

for label, text in (("Angband", ANG), ("FAangband", FA)):
    names = [line[5:] for line in text.splitlines()
             if line.startswith("name:")]
    assert len(names) == 624, (label, len(names))
    lengths = [len(name.encode("utf-8")) for name in names]
    assert lengths and all(size > 0 for size in lengths)
    # Do not cap future canonical names at today's 36-byte maximum:
    # the display builder explicitly falls back to the complete source
    # when its destination is too small.

assert 'known("Eöl, the Dark Elven Smith"' in NATIVE
assert 'known("Gothmog, the High Captain of Balrogs"' in NATIVE
assert "long_name[390] = '\\0';" in NATIVE
assert "tests/native/test_item_origin_monster_name.c" in RUNNER
assert "tests/test_item_origin_monster_name_source.py" in VERIFY
print("PASS: item origin display keeps 624/624 canonical monster source names intact")
