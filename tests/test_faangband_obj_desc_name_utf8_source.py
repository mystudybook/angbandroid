#!/usr/bin/env python3
"""FAangband Native item grammar must use bounded Unicode-safe formatter."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
ANG = ROOT / "app/src/main/cpp/angband"
DESC = (FA / "obj-desc.c").read_text(encoding="utf-8")
HEADER = (ANG / "obj-desc-name-utf8.h").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_obj_desc_name_utf8.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
EXPORT = (FA / "save-charoutput.c").read_text(encoding="utf-8")
SPOILER = (FA / "ui-spoil.c").read_text(encoding="utf-8")
ANG_DESC = (ANG / "obj-desc.c").read_text(encoding="utf-8")

assert '#include "../angband/obj-desc-name-utf8.h"' in DESC
assert '#include "obj-desc-name-utf8.h"' in ANG_DESC
assert "angdroid_obj_desc_name_format_safe(" in HEADER
assert "angdroid_obj_desc_append_span(" in HEADER
assert "angdroid_utf8_next_scalar(" in HEADER
assert "if (!closing) return end;" in HEADER
assert "if (modstr)" in HEADER
for grammar in ("'&'", "'~'", "'|'","'#'"):
    assert f"if (*fmt == {grammar})" in HEADER

start = DESC.index("size_t obj_desc_name_format(")
end = DESC.index("static size_t obj_desc_name(", start)
formatter = DESC[start:end]
assert "return angdroid_obj_desc_name_format_safe(" in formatter
assert "buf, max, end, fmt, modstr, pluralise);" in formatter
assert "buf[end++] = *fmt" not in formatter
assert "while (*fmt)" not in formatter
assert "strnfcat(" not in formatter
assert "end = obj_desc_name_format(buf, max, end," in DESC
assert "basename, modstr, plural);" in DESC

# The C99 loop asserts sentinel bytes before and after all capacities,
# complete UTF-8 code points, default English grammar, and offset handling.
assert "for (cap = 0; cap <= 64; ++cap)" in TEST
assert "guard[cap+1] == 'Z'" in TEST
assert "guard[0] == 'Z'" in TEST
assert "out[end] == '\\0'" in TEST
assert "angdroid_utf8_next_scalar(" in TEST
assert "CHECK(scalar != 0xFFFDu)" in TEST
assert "const char invalid[] = { (char)0xE4, 'X', 0 };" in TEST
assert 'expect(invalid, NULL, false, "?X");' in TEST
assert "中文" in TEST
assert "大馬士革鋼" in TEST
assert "Sto" not in formatter
assert 'expect("& Sta|ff|ves|", NULL, true, "Staves");' in TEST
assert 'expect("a # of #", "紅寶石", false, "a 紅寶石 of 紅寶石");' in TEST
assert "tests/native/test_faangband_obj_desc_name_utf8.c" in RUN
assert "tests/test_faangband_obj_desc_name_utf8_source.py" in VERIFY

# No new Chinese flag reaches the canonical save/export or spoiler paths.
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in SPOILER

print("PASS: FAangband item grammar uses bounded UTF-8 formatter, no direct writes")
