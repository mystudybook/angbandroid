#!/usr/bin/env python3
"""Source wiring for bounded original Angband object grammar and UTF-8."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
DESC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
HEADER = (CPP / "obj-desc-name-utf8.h").read_text(encoding="utf-8")
UI = (CPP / "obj-list.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
fmt = DESC.split("size_t obj_desc_name_format(", 1)[1].split(
    "static size_t obj_desc_name(", 1)[0]

assert '#include "obj-desc-name-utf8.h"' in DESC
assert "angdroid_obj_desc_name_format_safe(" in fmt
assert "buf[end++] = *fmt" not in fmt
assert "angdroid_obj_desc_append_span(" in HEADER
assert "angdroid_utf8_next_scalar(src + pos, &scalar)" in HEADER
assert "angdroid_utf8_next_scalar(fmt, &scalar)" in HEADER
assert "if (n > cap - *end - 1) return false;" in HEADER
assert "if (end >= cap)" in HEADER
assert "if (!closing) return end;" in HEADER
assert "if (modstr)" in HEADER
assert "if (*fmt == '~')" in HEADER
assert "if (*fmt == '|')" in HEADER
assert "if (*fmt == '&')" in HEADER
assert "if (*fmt == '#')" in HEADER
assert "obj_desc_name_format(buf, max, end, basename, modstr, plural)" in DESC
assert "object_desc(name, sizeof(name), base_obj, ODESC_PREFIX | ODESC_FULL |" in UI
assert "tests/native/test_obj_desc_name_utf8.c" in RUN
assert "tests/test_obj_desc_name_utf8_source.py" in VERIFY
print("PASS: original object grammar remains bounded and UTF-8 complete")
