#!/usr/bin/env python3
"""FAangband-exclusive artifact lore is complete, display-only, and guarded."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/artifact.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/artifact.txt").read_text(encoding="utf-8")
STANDARD = (CPP / "zh-tw-artifact-lore.h").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-faangband-artifact-lore.h").read_text(encoding="utf-8")
INFO = (CPP / "obj-info.c").read_text(encoding="utf-8")
FA_INFO = (ROOT / "app/src/main/cpp/faangband/obj-info.c").read_text(encoding="utf-8")
SAVE = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
OBJ = (CPP / "obj-desc.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def extract(data):
    sections = re.split(r"^name:", data, flags=re.M)[1:]
    result = {}
    for section in sections:
        lines = section.splitlines()
        name = lines[0].strip()
        description = "".join(
            row[5:] for row in lines if row.startswith("desc:")
        ).strip()
        assert name not in result
        result[name] = description
    return result

ang = extract(ANG)
fa = extract(FA)
exclusive = fa.keys() - ang.keys()
translations = re.findall(
    r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},$',
    HEADER, re.M,
)
standard = re.findall(
    r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},$',
    STANDARD, re.M,
)

assert len(ang) == 138 and len(fa) == 126
assert len(exclusive) == 41
assert len(translations) == 41
assert len(standard) == 138
assert {english for english, _ in translations} == exclusive
assert {english for english, _ in standard} == ang.keys()
assert len({english for english, _ in translations}) == 41
assert all(fa[name] and translated and len(translated) > 4
           for name, translated in translations)
assert all(any(ord(c) > 127 for c in translated)
           for _, translated in translations)
assert "ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE 41" in HEADER
assert "if (!name) return NULL;" in HEADER
assert "strcmp(name, entries[i].english) == 0" in HEADER
assert '#include "zh-tw-faangband-artifact-lore.h"' in INFO
assert INFO.count("angdroid_zh_tw_faangband_artifact_flavor(") == 1

# The original artifact identification check, random artifact exclusion,
# and mode control must still enclose both translation lookups.
start = INFO.index("if (!OPT(player, birth_randarts) && obj->artifact &&")
stop = INFO.index('textblock_append(tb, "%s\\n\\n", flavor);', start)
section = INFO[start:stop]
assert "obj->known->artifact && obj->artifact->text" in section
assert "if (zh_tw)" in section
assert section.index("if (zh_tw)") < section.index(
    "angdroid_zh_tw_faangband_artifact_flavor("
)
assert section.index("angdroid_zh_tw_artifact_flavor(") < section.index(
    "angdroid_zh_tw_faangband_artifact_flavor("
)
assert "if (!translated)" in section and "if (translated) flavor = translated;" in section
assert "const char *flavor = obj->artifact->text;" in section
assert "angdroid_zh_tw_enabled() && !(mode & OINFO_SPOIL)" in INFO
# The real FAangband game builds a separate obj-info.c engine.
assert '#include "../angband/zh-tw-faangband-artifact-lore.h"' in FA_INFO
assert "angdroid_zh_tw_faangband_artifact_flavor(" in FA_INFO
assert "obj->artifact && obj->known->artifact && obj->artifact->text" in FA_INFO
assert "if (!terse) describe_flavor_text(tb, obj, ego," in FA_INFO
assert "angdroid_zh_tw_enabled() && !(mode & OINFO_SPOIL)" in FA_INFO

assert "zh-tw-faangband-artifact-lore.h" not in SAVE
assert "zh-tw-faangband-artifact-lore.h" not in OBJ
assert "tests/native/test_faangband_artifact_lore_locale.c" in NATIVE
assert "tests/test_faangband_artifact_lore_source.py" in VERIFY

print("PASS: all 41 FA-only artifact stories and known-object zh-TW UI wiring")
