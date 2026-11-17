#!/usr/bin/env python3
"""Validate atomic artifact/ego suffix writes and existing reveal guards."""
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/angband/obj-desc.c").read_text(encoding="utf-8")
HEADER = (ROOT / "app/src/main/cpp/angband/obj-desc-suffix-utf8.h").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
block = SRC.split("/* Append extra names of various kinds */", 1)[1].split(
    "return end;\n}", 1)[0]

assert '#include "obj-desc-suffix-utf8.h"' in SRC
assert "if (object_is_known_artifact(obj))" in block
assert "obj->known->ego && !(mode & ODESC_NOEGO)" in block
assert "obj->ego && store" in block
assert block.count("if (localized_kind && translated_suffix)") == 2
assert block.count("angdroid_zh_tw_item_suffix(") == 2
assert block.count("angdroid_obj_desc_append_suffix_atomic(") == 4
assert "artifact_name, true);" in block
assert "ego_name, true);" in block
assert "suffix_display, false);" in block
assert 'strnfcat(buf, max, &end, " %s", artifact_name);' not in block
assert 'strnfcat(buf, max, &end, " %s", ego_name);' not in block
assert "length >= capacity - used - padding" in HEADER
assert "angdroid_utf8_next_scalar(suffix + pos, &scalar)" in HEADER
assert "if (leading_space) out[used++] = ' ';" in HEADER
assert "*end = used;" in HEADER
assert "tests/native/test_obj_desc_suffix_utf8.c" in RUN
assert "tests/test_obj_desc_suffix_utf8_source.py" in VERIFY
print("PASS: visible artifact/ego suffixes append atomically")
