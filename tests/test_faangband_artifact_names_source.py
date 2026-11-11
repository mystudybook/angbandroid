#!/usr/bin/env python3
"""FAangband-only artifact name coverage and display-only integration."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/artifact.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/artifact.txt").read_text(encoding="utf-8")
STD = (CPP / "zh-tw-artifact-names.h").read_text(encoding="utf-8")
ADDED = (CPP / "zh-tw-faangband-artifact-names.h").read_text(encoding="utf-8")
OBJ = (CPP / "obj-desc.c").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def source_names(data):
    return re.findall(r"^name:(.*)$", data, re.M)

standard_names = source_names(ANG)
fa_names = source_names(FA)
canonical = set(standard_names)
missing = set(fa_names) - canonical
translated = re.findall(
    r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},$',
    ADDED, re.M)
original = re.findall(
    r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},$',
    STD, re.M)

assert len(standard_names) == 138
assert len(fa_names) == 126
assert len(missing) == 41
assert len(original) == 138
assert set(name for name, _ in original) == canonical
assert len(translated) == 41
assert len(set(name for name, _ in translated)) == 41
assert all(chinese and chinese != english for english, chinese in translated)
assert set(name for name, _ in translated) == missing
assert "ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES 41" in ADDED
assert "if (!english) return NULL;" in ADDED
assert "strcmp(english, names[i].english) == 0" in ADDED
assert '#include "zh-tw-faangband-artifact-names.h"' in OBJ
assert OBJ.count("angdroid_zh_tw_faangband_artifact_name(") == 1

# English and canonical names survive unchanged; the supplemental lookup is
# reached only in the existing Chinese, known-artifact display block.
fragment = OBJ[OBJ.index("if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()) {",
                         OBJ.index("if (object_is_known_artifact(obj))",
                                   OBJ.index("static size_t obj_desc_name("))):]
fragment = fragment[:fragment.index("if (localized) artifact_name = localized;")]
assert ("const char *localized = angdroid_zh_tw_artifact_name(artifact_name);"
        in fragment)
assert ("if (!localized)\n\t\t\t\tlocalized = "
        "angdroid_zh_tw_faangband_artifact_name(artifact_name);") in fragment
assert "if (localized) artifact_name = localized;" in OBJ
assert 'zh-tw-faangband-artifact-names.h' not in EXPORT
assert "ODESC_ZH_TW" not in EXPORT
assert "tests/native/test_faangband_artifact_names_locale.c" in RUNNER
assert "tests/test_faangband_artifact_names_source.py" in VERIFY
print("PASS: all 41 distinct FAangband artifacts localized in zh-TW only")
