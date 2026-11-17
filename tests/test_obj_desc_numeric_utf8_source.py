#!/usr/bin/env python3
"""Numeric item details remain knowledge-gated and append atomically."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SRC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
HEADER = (CPP / "obj-desc-numeric-utf8.h").read_text(encoding="utf-8")
NATIVE_RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
OLD_TEST = (ROOT / "tests/test_item_details_localization.py").read_text(encoding="utf-8")

assert '#include "obj-desc-numeric-utf8.h"' in SRC
assert "if (!((obj->notice & OBJ_NOTICE_ASSESSED) || spoil)) return end;" in SRC
assert "(!p || (p->obj_k->dd && p->obj_k->ds))" in SRC
assert "if (!num_mods) return end;" in SRC
assert "if (obj->modifiers[i] != 0)" in SRC
assert "if (mods[j] == obj->modifiers[i]) break;" in SRC
assert "object_flavor_is_aware(obj) || (mode & ODESC_STORE)" in SRC
assert "(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()" in SRC
assert SRC.count("angdroid_obj_desc_append_formatted_detail(buf, max, &end,") == 16
assert "angdroid_obj_desc_append_modifiers_ascii(buf, max, &end," in SRC
for start, stop in (
    ("static size_t obj_desc_combat(", "static size_t obj_desc_light("),
    ("static size_t obj_desc_light(", "static size_t obj_desc_mods("),
    ("static size_t obj_desc_mods(", "static size_t obj_desc_charges("),
    ("static size_t obj_desc_charges(", "static size_t obj_desc_inscrip("),
    ("static size_t obj_desc_aware(", "size_t object_desc("),
):
    section = SRC.split(start, 1)[1].split(stop, 1)[0]
    assert "strnfcat(buf, max, &end," not in section, start
assert 'vsnprintf(text, sizeof(text), fmt, args)' in HEADER
assert 'if (n <= 0 || (size_t)n >= sizeof(text) ||' in HEADER
assert '(size_t)n >= capacity - *end) return false;' in HEADER
assert 'if (used >= capacity - *end) return false;' in HEADER
assert "text[used++] = '>';" in HEADER
assert "tests/native/test_obj_desc_numeric_utf8.c" in NATIVE_RUN
assert "tests/test_obj_desc_numeric_utf8_source.py" in VERIFY
assert "angdroid_obj_desc_append_modifiers_ascii" in OLD_TEST
print("PASS: atomic English numeric/combat/charge/aware detail contracts")
