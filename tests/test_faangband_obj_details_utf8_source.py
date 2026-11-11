#!/usr/bin/env python3
"""FAangband item detail suffixes stay complete UTF-8 units and gated."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"

def read(path):
    return path.read_text(encoding="utf-8")

HEADER = read(FA / "obj-desc-details-utf8.h")
DESC = read(FA / "obj-desc.c")
C_TEST = read(ROOT / "tests/native/test_faangband_obj_details_utf8.c")
RUN = read(ROOT / "scripts/test-native-utf8.sh")
VERIFY = read(ROOT / "scripts/verify-zh-tw.sh")
EXPORT = read(FA / "save-charoutput.c")
SPOILER = read(FA / "ui-spoil.c")

assert '#include "obj-desc-details-utf8.h"' in DESC
assert '#include "../angband/obj-desc-suffix-utf8.h"' in HEADER
assert "angdroid_faangband_append_detailf(" in HEADER
assert "vsnprintf(group, sizeof(group), format, args);" in HEADER
assert "if (length < 0 || (size_t)length >= sizeof(group)) return false;" in HEADER
assert "angdroid_obj_desc_append_suffix_atomic(" in HEADER
assert "angdroid_faangband_append_inscription(" in HEADER
assert "if (needed > cap - used - 1 ||" in HEADER
assert "bytes > cap - used - 1 - needed" in HEADER
assert "angdroid_utf8_next_scalar(s + pos, &scalar)" in HEADER
assert "if (!labels[i]) return false;" in HEADER
assert "out[used++] = '{';" in HEADER
assert "out[used++] = '}';" in HEADER
assert "angdroid_faangband_append_modifiers(" in HEADER
assert "if (sizeof(group) - used <= 2) return false;" in HEADER
assert "if (written < 0 || (size_t)written >= sizeof(group) - used - 2)" in HEADER

for marker in ("empty", "tried", "cursed", "ignore", "unseen"):
    assert f'strcmp(source, "{marker}")' in HEADER
assert "return source; /* \"??\" and unknown markers" in HEADER

start = DESC.index("static size_t obj_desc_chest(")
stop = DESC.index("static size_t obj_desc_inscrip(", start)
details = DESC[start:stop]
assert "angdroid_faangband_append_detailf(" in details
assert details.count("angdroid_faangband_append_detailf(") == 18
assert "angdroid_faangband_append_modifiers(" in details
assert "strnfcat(buf, max, &end," not in details
assert 'if (!((obj->notice & OBJ_NOTICE_ASSESSED) || spoil)) return end;' in details
assert "if (!num_mods) return end;" in details
assert "bool aware = object_flavor_is_aware(obj) || (mode & ODESC_STORE);" in details
assert "if (aware && tval_can_have_charges(obj))" in details
assert "if (tval_is_rod(obj) && obj->number > 1)" in details
assert '" (%d charge%s)"' in details
assert '" (%d charging)"' in details
assert '" (charging)"' in details
assert '" (%d turns)"' in details
assert '"（剩餘 %d 次）"' in details
assert '"（%d 根充能中）"' in details
assert '"（充能中）"' in details
assert '"（剩餘 %d 回合）"' in details

start = DESC.index("static size_t obj_desc_inscrip(")
stop = DESC.index("/**\n * Describes item", start)
annotation = DESC[start:stop]
assert "quark_str(obj->note)" in annotation
assert "if (!object_flavor_is_aware(obj))" in annotation
assert "if (obj->known->curses)" in annotation
assert "if (p && ignore_item_ok(p, obj))" in annotation
assert "if (!object_runes_known(obj)" in annotation
assert "angdroid_faangband_status_ui_name(" in annotation
assert "angdroid_faangband_append_inscription(" in annotation
assert "strnfcat(buf, max, &end," not in annotation
assert "obj_desc_inscrip(const struct object *obj, char *buf," in annotation
assert "bool translated_ui)" in annotation

assert "bool translated_ui = (mode & ODESC_ZH_TW) && !spoil && !terse &&" in DESC
assert "angdroid_zh_tw_enabled();" in DESC
assert "end = obj_desc_light(obj, buf, max, end, translated_ui);" in DESC
assert "end = obj_desc_charges(obj, buf, max, end, mode, translated_ui);" in DESC
assert "end = obj_desc_aware(obj, buf, max, end, translated_ui);" in DESC
assert "end = obj_desc_inscrip(obj, buf, max, end, p, translated_ui);" in DESC
assert "end = obj_desc_mods(obj->known, buf, max, end);" in DESC
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in SPOILER

assert "for (cap = 0; cap <= 96; ++cap)" in C_TEST
assert "for (cap = 0; cap <= 80; ++cap)" in C_TEST
assert "CHECK(guard[cap + 1] == 'Z');" in C_TEST
assert "CHECK(guard[cap + 1] == 'X');" in C_TEST
assert "2147483647" in C_TEST
assert "long_mods[128]" in C_TEST
assert 'CHECK(!angdroid_faangband_append_inscription(' in C_TEST
assert 'CHECK(!angdroid_faangband_append_detailf(' in C_TEST
assert "test_faangband_obj_details_utf8.c" in RUN
assert "test_faangband_obj_details_utf8_source.py" in VERIFY
print("PASS: FAangband modifier/charge/inscription groups atomic; UI-only statuses")
