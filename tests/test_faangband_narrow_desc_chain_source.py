#!/usr/bin/env python3
"""FAangband narrow output: complete base labels before visible suffixes."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
DESC = (FA / "obj-desc.c").read_text(encoding="utf-8")
BASIC = (FA / "obj-desc-basic-utf8.h").read_text(encoding="utf-8")
DETAIL = (FA / "obj-desc-details-utf8.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_narrow_desc_chain.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
EXPORT = (FA / "save-charoutput.c").read_text(encoding="utf-8")
SPOILER = (FA / "ui-spoil.c").read_text(encoding="utf-8")
SOURCE = (ROOT / "variant_lib/faangband/gamedata/object.txt").read_text(
    encoding="utf-8")

# Never split semantic (nothing), Chinese fullwidth parentheses, or the
# literal unknown-item placeholder into a meaningless fragment.
assert '#include "../angband/obj-desc-suffix-utf8.h"' in BASIC
assert 'label = translated_ui ? "未知物品"' in BASIC
assert 'label = translated_ui ? "（空）" : "(nothing)"' in BASIC
assert "angdroid_obj_desc_append_suffix_atomic(" in BASIC
assert "out[0] = '\\0';" in BASIC
assert "if (!out || !cap) return 0;" in BASIC
assert "return angdroid_faangband_copy_utf8(out, cap, label);" not in BASIC

# A copied raw unidentified appearance is not a kind name. Its formatting
# characters must remain literal; awareness checks stay in Native.
assert "angdroid_faangband_copy_utf8(" in BASIC
assert "source, strlen(source)" in BASIC
assert "!easy_know && !kind->aware && kind->flavor" in DESC
assert "angdroid_faangband_copy_utf8(buf, max," in DESC

# UI-only English fallback: stage the complete name before copying it,
# and do not append a suffix or bonus to a missing/truncated base name.
start = DESC.index("static size_t obj_desc_name(")
stop = DESC.index("static bool obj_desc_show_armor(", start)
part = DESC[start:stop]
assert "if (!translated_base) {" in part
assert "if (translated_ui) {" in part
assert 'char english_base[512] = "";' in part
assert "obj_desc_name_prefix(english_base," in part
assert "whole = obj_desc_name_format(english_base," in part
assert "whole >= sizeof(english_base) - 1" in part
assert "!angdroid_obj_desc_append_suffix_atomic(" in part
assert "buf, max, &end, english_base, false)" in part
assert "return end;" in part
assert "if (object_is_known_artifact(obj)) {" in part
assert "angdroid_faangband_known_ego_ui_name(" in part
assert "angdroid_obj_desc_append_flavor_effect(" in part
assert "end = obj_desc_name_format(buf, max, end," in part
assert "basename, modstr, plural);" in part

final = DESC[DESC.index("size_t object_desc(char *buf"):]
assert "if (!buf || !max) return 0;" in final
assert "if (translated_ui && end == 0) return 0;" in final
assert final.index("if (translated_ui && end == 0) return 0;") < (
    final.index("if (mode & ODESC_COMBAT)"))
assert "if (mode & ODESC_EXTRA)" in final

# Multiple independent atomic suffixes may fit or be skipped. When the
# base doesn't fit, no stand-alone parentheses, stats, or inscriptions.
assert "angdroid_faangband_append_modifiers(" in DETAIL
assert "angdroid_faangband_append_detailf(" in DETAIL
assert "angdroid_faangband_append_inscription(" in DETAIL
assert "chain_at_every_capacity(\"Long Sword\")" in C_TEST
assert "chain_at_every_capacity(\"長劍\")" in C_TEST
assert "short_basic_labels();" in C_TEST
assert "original_effect_fallback();" in C_TEST
assert "for (cap = 0; cap <= 128; ++cap)" in C_TEST
assert "for (cap = 0; cap <= 32; ++cap)" in C_TEST
assert "CHECK(memory[cap + 1] == '#');" in C_TEST
assert "CHECK(memory[cap + 1] == 'Z');" in C_TEST
assert "check_valid_utf8(out);" in C_TEST
assert "CHECK(strcmp(out, expected) == 0);" in C_TEST
assert "test_faangband_narrow_desc_chain.c" in RUN
assert "test_faangband_narrow_desc_chain_source.py" in VERIFY
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in SPOILER

# The source base-name templates are short: 512 bytes is deliberately
# above their real maximum (and the max ASCII quantity prefix), avoiding
# partial staging of a normal FA name.
source_names = [
    line.removeprefix("name:") for line in SOURCE.splitlines()
    if line.startswith("name:")
]
assert source_names
assert max(map(len, source_names)) < 128

print("PASS: FA narrow UI displays only whole base labels and suffix groups")
