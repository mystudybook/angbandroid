#!/usr/bin/env python3
"""Two variant-only spell descriptions match FAangband original rules exactly."""
from pathlib import Path
import re
import json

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
ORIGINAL = (ROOT / "variant_lib/angband/gamedata/class.txt").read_text(encoding="utf-8")
FA = (ROOT / "variant_lib/faangband/gamedata/class.txt").read_text(encoding="utf-8")
STANDARD = (CPP / "zh-tw-spell-descriptions.h").read_text(encoding="utf-8")
SUPPLEMENT = (CPP / "zh-tw-faangband-spells.h").read_text(encoding="utf-8")
NAMES = (CPP / "zh-tw-spell-names.h").read_text(encoding="utf-8")
UI = (CPP / "ui-spell.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def entries(source):
    records = []
    current = None
    for line in source.splitlines():
        if line.startswith("spell:"):
            current = [line[6:].split(":", 1)[0], ""]
            records.append(current)
        elif line.startswith("desc:") and current is not None:
            current[1] += line[5:]
        elif line.startswith(("book:", "name:")):
            current = None
    return records

format_three = re.compile(
    r'^\s*\{\s*("(?:[^"\\]|\\.)*"),\s*'
    r'("(?:[^"\\]|\\.)*"),\s*'
    r'("(?:[^"\\]|\\.)*")\s*\},$', re.M)
canonical = {
    (json.loads(match[1]), json.loads(match[2]))
    for match in format_three.finditer(STANDARD)
}
assert len(canonical) == 135

# Two names are completely FA-only (already translated in batch 161).
new_names = {"Timeless Blink", "Fume of Thangorodrim"}
fa_rows = entries(FA)
standard_rows = entries(ORIGINAL)
assert len(fa_rows) == len(standard_rows) == 163
uncovered = [(name, desc) for name, desc in fa_rows
             if (name, desc) not in canonical and name not in new_names]
assert len(uncovered) == 3
assert uncovered.count(next(x for x in uncovered if x[0] == "Word of Recall")) == 2
assert sum(name == "Berserk Strength" for name, _ in uncovered) == 1
expected = set(map(tuple, uncovered))
assert len(expected) == 2

block = SUPPLEMENT.split("variants[ANGDROID_ZH_TW_FAANGBAND_VARIANT_DESC_COUNT]", 1)[1]
block = block.split("    };", 1)[0]
actual = [
    tuple(json.loads(part) for part in match.groups())
    for match in format_three.finditer(block)
]
assert len(actual) == 2
assert {(name, original) for name, original, _ in actual} == expected
assert all(traditional and traditional != original for _, original, traditional in actual)
assert "14+1d20" in next(zh for name, en, zh in actual if name == "Word of Recall")
assert "四個地點" in next(zh for name, en, zh in actual if name == "Word of Recall")
berserk = next(zh for name, en, zh in actual if name == "Berserk Strength")
assert "72" in berserk and "75" not in berserk
assert "strcmp(english_description, variants[i].english) == 0" in SUPPLEMENT
assert "return angdroid_zh_tw_faangband_variant_description(" in SUPPLEMENT
assert "angdroid_zh_tw_faangband_spell_description(name, english)" in STANDARD
assert "if (!english || !zh_tw || !name) return english;" in STANDARD
assert 'angdroid_zh_tw_spell_description(spell->name,' in UI
assert "tests/native/test_faangband_variant_spell_descriptions.c" in RUN
assert "tests/test_faangband_variant_spell_descriptions_source.py" in VERIFY
assert "ANGDROID_ZH_TW_FAANGBAND_SPELL_COUNT 2" in SUPPLEMENT
assert "angdroid_zh_tw_faangband_spell_name(en)" in NAMES
print("PASS: FAangband 2 variant-specific descriptions and untouched canonical rules")
