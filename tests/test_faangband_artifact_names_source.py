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
FA_DIR = ROOT / "app/src/main/cpp/faangband"
FA_OBJ = (FA_DIR / "obj-desc.c").read_text(encoding="utf-8")
FA_MODE = (FA_DIR / "obj-desc.h").read_text(encoding="utf-8")
FA_RESOLVER = (FA_DIR / "obj-desc-artifact-zh-tw.h").read_text(encoding="utf-8")
FA_LIST = (FA_DIR / "ui-object.c").read_text(encoding="utf-8")
FA_STORE = (FA_DIR / "ui-store.c").read_text(encoding="utf-8")
FA_KNOWLEDGE = (FA_DIR / "ui-knowledge.c").read_text(encoding="utf-8")
FA_EXPORT = (FA_DIR / "save-charoutput.c").read_text(encoding="utf-8")
FA_SPOILER = (FA_DIR / "ui-spoil.c").read_text(encoding="utf-8")


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
# FAangband has a separate Native object-description implementation.
# Verify the 138 + 41 lookup dictionaries cover all 126 FA artifact names.
full_dictionary = {name for name, _ in original} | {name for name, _ in translated}
assert set(fa_names) <= full_dictionary
assert len(set(fa_names)) == 126
assert len(set(fa_names) & canonical) == 85
assert "ODESC_ZH_TW = 0x800" in FA_MODE
assert '#include "droid.h"' in FA_OBJ
assert '#include "obj-desc-artifact-zh-tw.h"' in FA_OBJ
assert '#include "../angband/obj-desc-suffix-utf8.h"' in FA_OBJ
assert '#include "../angband/zh-tw-artifact-names.h"' in FA_RESOLVER
assert '#include "../angband/zh-tw-faangband-artifact-names.h"' in FA_RESOLVER
assert "angdroid_zh_tw_artifact_name(english)" in FA_RESOLVER
assert "angdroid_zh_tw_faangband_artifact_name(english)" in FA_RESOLVER
assert "return translated ? translated : english;" in FA_RESOLVER
assert "if (!english || !translate) return english;" in FA_RESOLVER

# Names change only for identified artifacts in explicit UI, never in the
# shared canonical data, English fallback, spoilers, or terse descriptions.
fa_start = FA_OBJ.index("static size_t obj_desc_name(")
fa_end = FA_OBJ.index("static bool obj_desc_show_armor(", fa_start)
fa_block = FA_OBJ[fa_start:fa_end]
assert "if (object_is_known_artifact(obj)) {" in fa_block
assert "const char *artifact_name = angdroid_faangband_artifact_ui_name(" in fa_block
assert "(mode & ODESC_ZH_TW)" in fa_block
assert "!(mode & (ODESC_SPOIL | ODESC_TERSE))" in fa_block
assert "angdroid_zh_tw_enabled()" in fa_block
assert "angdroid_obj_desc_append_suffix_atomic(buf, max," in fa_block
assert "obj->artifact->name, translated_ui" in fa_block
assert "if (object_is_known_artifact(obj)) {" in FA_OBJ
assert 'ODESC_PREFIX | ODESC_BASE | ODESC_SPOIL, NULL);' in FA_KNOWLEDGE
assert "ODESC_ZH_TW" not in FA_EXPORT
assert "ODESC_ZH_TW" not in FA_SPOILER

# Explicitly opted-in paths: inventory, recall/examine, known artifact detail,
# and store/home browsing and context. Terse inventory stays unflagged.
assert FA_LIST.count("ODESC_ZH_TW") == 4
assert "ODESC_PREFIX | ODESC_FULL | ODESC_TERSE," in FA_LIST
assert FA_KNOWLEDGE.count("ODESC_ZH_TW") == 1
assert FA_STORE.count("ODESC_ZH_TW") == 5
assert "tests/native/test_faangband_artifact_name_ui.c" in RUNNER
assert "tests/test_faangband_artifact_names_source.py" in VERIFY

print("PASS: all 126 FA artifacts covered by 138+41 dictionaries with safe Native UI gating")
