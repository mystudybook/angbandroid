#!/usr/bin/env python3
"""Source coverage and final-output safety for FAangband's object names."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
ANG = ROOT / "app/src/main/cpp/angband"
DESC = (FA / "obj-desc.c").read_text(encoding="utf-8")
BASIC = (FA / "obj-desc-basic-utf8.h").read_text(encoding="utf-8")
SRC = (ROOT / "variant_lib/faangband/gamedata/object.txt").read_text(encoding="utf-8")
CHEST_DATA = (ROOT / "variant_lib/faangband/gamedata/chest_trap.txt").read_text(encoding="utf-8")
CHEST = (FA / "obj-chest.c").read_text(encoding="utf-8")
CHEST_HELPER = (ANG / "obj-desc-chest-money-utf8.h").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_obj_basic_utf8.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
EXPORT = (FA / "save-charoutput.c").read_text(encoding="utf-8")
SPOILER = (FA / "ui-spoil.c").read_text(encoding="utf-8")

assert '#include "obj-desc-basic-utf8.h"' in DESC
assert '#include "../angband/obj-desc-chest-money-utf8.h"' in BASIC
assert "angdroid_obj_desc_append_span(" in BASIC
assert "if (!out || !cap) return 0;" in BASIC
assert "if (!source) return 0;" in BASIC
assert 'label = translated_ui ? "未知物品"' in BASIC
assert 'label = translated_ui ? "（空）" : "(nothing)"' in BASIC
assert "angdroid_obj_desc_money_text(" in BASIC

base = DESC.split("void object_base_name(", 1)[1].split(
    "void object_kind_name(", 1)[0]
kind = DESC.split("void object_kind_name(", 1)[1].split(
    "static const char *obj_desc_get_modstr(", 1)[0]
assert "if (!buf || !max) return;" in base
assert "buf[0] = '\\0';" in base
assert "if (kb->name && kb->name[0])" in base
assert "if (!buf || !max) return;" in kind
assert "if (!kind) return;" in kind
assert "!easy_know && !kind->aware && kind->flavor" in kind
assert "angdroid_faangband_copy_utf8(buf, max," in kind
assert "kind->flavor->text" in kind
assert "my_strcpy(" not in kind
assert "obj_desc_name_format(buf, max, 0, kind->name, NULL, false);" in kind

final = DESC[DESC.index("size_t object_desc(char *buf"):]
assert "if (!buf || !max) return 0;" in final
assert final.index("if (!buf || !max) return 0;") < final.index("if (!obj || !obj->known)")
assert "buf[0] = '\\0';" in final
assert "angdroid_faangband_basic_item_text(" in final
assert "obj->kind != obj->known->kind" in final
assert "angdroid_faangband_money_text(" in final
assert "ignore_item_ok(p, obj), translated_ui" in final
assert "return strnfmt(buf, max" not in final
assert "obj->kind->everseen = true" in final
assert "obj->ego->everseen = true" in final
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in SPOILER

# Source contains exactly the 11 canonical Angband-compatible gold kinds.
items = []
for block in re.split(r"(?=^name:)", SRC, flags=re.M):
    if not block.startswith("name:"):
        continue
    name = block.splitlines()[0][5:]
    if re.search(r"^type:gold$", block, re.M):
        items.append(name)
source_gold = set(items)
known_gold = {
    "copper", "silver", "garnets", "gold", "opals",
    "sapphires", "rubies", "diamonds", "emeralds", "mithril",
    "adamantite"
}
assert source_gold == known_gold, (source_gold, known_gold)
for name in known_gold:
    assert f'{{ "{name}",' in CHEST_HELPER

# Exact source trap names are used after the existing knowledge gate.
labels = set(re.findall(r"^name:(.*)$", CHEST_DATA, re.M))
for label in labels:
    assert f'{{ "{label}",' in CHEST_HELPER
for label in ("unlocked", "disarmed", "multiple traps", "empty"):
    assert f'"{label}"' in CHEST
assert "if (obj->pval && !obj->known->pval) return end;" in DESC
assert "angdroid_obj_desc_append_chest_status(" in DESC
assert "end = obj_desc_chest(obj, buf, max, end, translated_ui);" in DESC
assert "angdroid_obj_desc_append_flavor_effect(" in DESC
assert "angdroid_obj_desc_append_suffix_atomic(" in DESC
assert 'strnfcat(buf, max, &end, " of %s"' not in DESC
assert 'strnfcat(buf, max, &end, " %s", ego_name)' not in DESC

assert "for (cap = 0; cap <= 64; ++cap)" in TEST
assert "CHECK(storage[cap + 1] == '#');" in TEST
assert "angdroid_utf8_next_scalar(" in TEST
assert "money_catalog()" in TEST
assert 'strcmp(out, "未知物品") == 0' in TEST
assert 'strcmp(out, "（空）") == 0' in TEST
assert "tests/native/test_faangband_obj_basic_utf8.c" in RUNNER
assert "tests/test_faangband_obj_basic_utf8_source.py" in VERIFY
print("PASS: FA basic and money safe output; original identity and knowledge preserved")
