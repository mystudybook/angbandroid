#!/usr/bin/env python3
"""Safe localized chest visibility and money description formatting."""
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SRC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
CHEST = (CPP / "obj-chest.c").read_text(encoding="utf-8")
HEADER = (CPP / "obj-desc-chest-money-utf8.h").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
OBJ_DATA = (ROOT / "variant_lib/angband/gamedata/object.txt").read_text(encoding="utf-8")
TRAP_DATA = (ROOT / "variant_lib/angband/gamedata/chest_trap.txt").read_text(encoding="utf-8")

assert '#include "obj-desc-chest-money-utf8.h"' in SRC
section = SRC.split("static size_t obj_desc_chest(", 1)[1].split(
    "static size_t obj_desc_combat(", 1)[0]
assert "if (!tval_is_chest(obj)) return end;" in section
assert "if (obj->pval && !obj->known->pval) return end;" in section
assert "chest_trap_name(obj)" in section
assert "angdroid_obj_desc_append_chest_status(buf, max, &end," in section
assert "(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()" in section
assert "end = obj_desc_chest(obj, buf, max, end, mode);" in SRC
assert "return angdroid_obj_desc_money_text(buf, max, obj->pval," in SRC
assert "obj->kind->name, ignore_item_ok(p, obj)," in SRC
assert "const char *chest_trap_name(const struct object *obj)" in CHEST
assert "angdroid_obj_desc_append_effect_wrapped(out, cap, end," in HEADER
assert 'zh_tw ? "（" : " ("' in HEADER
assert "zh_tw ? \"價值 %d 金幣的\" : \"%d gold pieces worth of \"" in HEADER
assert 'zh_tw ? " {忽略}" : " {ignore}"' in HEADER
assert 'if (!kind || !*kind) return 0;' in HEADER
for name in ("copper", "silver", "garnets", "gold", "opals", "sapphires",
             "rubies", "diamonds", "emeralds", "mithril", "adamantite"):
    assert f"name:{name}" in OBJ_DATA and f'{{ "{name}",' in HEADER
for name in ("locked", "gas trap", "poison needle",
             "summoning runes", "explosion device"):
    assert f"name:{name}" in TRAP_DATA and f'{{ "{name}",' in HEADER
assert "tests/native/test_obj_desc_chest_money_utf8.c" in RUN
assert "tests/test_obj_desc_chest_money_utf8_source.py" in VERIFY
print("PASS: knowledge-guarded chest and item-money bilingual display")
