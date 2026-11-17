#!/usr/bin/env python3
"""FAangband-only object kind coverage, identification, and UI-only wiring."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA_DATA = (ROOT / "variant_lib/faangband/gamedata/object.txt").read_text(encoding="utf-8")
STANDARD = (CPP / "zh-tw-items.h").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-faangband-items.h").read_text(encoding="utf-8")
OBJ = (CPP / "obj-desc.c").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
RNG = (CPP / "obj-util.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

canonical = dict(re.findall(r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', STANDARD))
supplement = re.findall(
    r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},$',
    HEADER, re.M)
assert len(canonical) == 326
assert len(supplement) == 39
assert len(set((name, kind) for name, kind, _ in supplement)) == 39
assert all(name not in canonical and text for name, _, text in supplement)
source = []
lines = FA_DATA.splitlines()
for i, line in enumerate(lines):
    if not line.startswith("name:"):
        continue
    name = line[5:]
    found = next((s[5:] for s in lines[i+1:i+7] if s.startswith("type:")), None)
    if name not in canonical and name not in ("<pile>", "<unknown item>",
                                               "<unknown treasure>", "<curse object>"):
        source.append((name, found))
assert len(set(source)) == 39
assert set(source) == {(name, kind) for name, kind, _ in supplement}
assert '#include "zh-tw-faangband-items.h"' in OBJ
assert OBJ.count("angdroid_zh_tw_faangband_item_kind_name(") == 4
assert "if (zh_tw && !translated)" in OBJ
assert "if (aware && obj->kind->name)" in OBJ
assert "if (!localized_kind)" in OBJ
assert "if (!visible_effect)" in OBJ
assert "if (!known)" in OBJ
assert "angdroid_zh_tw_identified_flavor_name(display_name," in OBJ
assert "translated ? translated : kind->name" in OBJ
assert "if (!easy_know && !kind->aware && kind->flavor)" in OBJ
assert "angdroid_zh_tw_flavor_name(flavor)" in OBJ
assert "ODESC_ZH_TW" not in EXPORT and "ODESC_ZH_TW" not in RNG
assert "zh-tw-faangband-items.h" not in EXPORT
assert "tests/native/test_faangband_item_names_locale.c" in NATIVE
assert "tests/test_faangband_item_names_source.py" in VERIFY
print("PASS: 39 exact FAangband supplemental names and awareness guards")
