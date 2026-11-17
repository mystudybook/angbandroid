#!/usr/bin/env python3
"""Guard player-written UTF-8 inscriptions and localized display statuses."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SRC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
HELPER = (CPP / "obj-desc-inscription-utf8.h").read_text(encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
PYTHON_RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
DETAIL_TEST = (ROOT / "tests/test_item_details_localization.py").read_text(encoding="utf-8")
FN = SRC.split("static size_t obj_desc_inscrip(", 1)[1].split(
    "static size_t obj_desc_aware(", 1)[0]

assert '#include "obj-desc-inscription-utf8.h"' in SRC
assert 'u[n++] = quark_str(obj->note);' in FN
assert "if (zh_tw && !(i == 0 && obj->note))" in FN
assert 'if (streq(label, "cursed")) label = "受詛咒";' in FN
assert 'if (streq(label, "ignore")) label = "忽略";' in FN
assert "angdroid_obj_desc_inscription_append(buf, max, &end," in FN
assert "display, (size_t)n, zh_tw, obj->note != 0);" in FN
assert 'strnfcat(buf, max, &end, "%s", u[i]);' not in FN
assert 'strnfcat(buf, max, &end, "}");' not in FN
assert 'zh_tw ? "、" : ", "' in HELPER
assert "capacity - used < 4" in HELPER
assert "angdroid_utf8_append_complete(out, capacity - 1, used, label)" in HELPER
assert "if (used - before_text != label_bytes) break;" in HELPER
assert "out[used++] = '}';" in HELPER
assert "tests/native/test_obj_desc_inscription_utf8.c" in NATIVE_RUNNER
assert "tests/test_obj_desc_inscription_utf8_source.py" in PYTHON_RUNNER
assert 'obj-desc-inscription-utf8.h' in DETAIL_TEST
print("PASS: inscription UTF-8 source/locale/brace contract")
