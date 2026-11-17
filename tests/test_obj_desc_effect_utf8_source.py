#!/usr/bin/env python3
"""Enforce display-only, awareness-gated, atomic effect/scroll suffixes."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SRC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
HELPER = (CPP / "obj-desc-effect-utf8.h").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
SCOPE = SRC.split("else if (localized_scroll && aware && !obj->artifact)", 1)[1]
SCOPE = SCOPE.split("return end;\n}", 1)[0]

assert '#include "obj-desc-effect-utf8.h"' in SRC
assert "else if (localized_scroll && aware && !obj->artifact)" in SRC
assert "angdroid_zh_tw_item_kind_name(obj->kind->name)" in SCOPE
assert "known ? known : obj->kind->name" in SCOPE
assert "angdroid_obj_desc_append_scroll_effect(buf, max, &end," in SCOPE
assert "else if (!localized_kind && aware && !obj->artifact" in SCOPE
assert "!localized_flavored &&" in SCOPE
assert "(obj->kind->flavor || obj->kind->tval == TV_SCROLL)" in SCOPE
assert "angdroid_obj_desc_append_flavor_effect(buf, max, &end," in SCOPE
assert 'strnfcat(buf, max, &end, " of %s"' not in SCOPE
assert 'strnfcat(buf, max, &end, " \'%s\'"' not in SCOPE
assert "if (c >= remaining) return false;" in HELPER
assert "angdroid_utf8_next_scalar(p, &cp)" in HELPER
assert "memcpy(out + start + a + b, suffix, c)" in HELPER
assert 'terse ? " \'" : " of "' in HELPER
assert 'terse ? "\'" : ""' in HELPER
assert '        "（", effect, "）");' in HELPER
assert "tests/native/test_obj_desc_effect_utf8.c" in NATIVE
assert "tests/test_obj_desc_effect_utf8_source.py" in VERIFY
print("PASS: identified scroll/flavor effect suffixes are atomic UTF-8")
