#!/usr/bin/env python3
"""FAangband-only spell display additions match their exact class.txt texts."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/class.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/class.txt").read_text(encoding="utf-8")
SUPPLEMENT = (CPP / "zh-tw-faangband-spells.h").read_text(encoding="utf-8")
NAMES = (CPP / "zh-tw-spell-names.h").read_text(encoding="utf-8")
DESCRIPTIONS = (CPP / "zh-tw-spell-descriptions.h").read_text(encoding="utf-8")
UI = (CPP / "ui-spell.c").read_text(encoding="utf-8")
KNOWLEDGE = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
LEARN = (CPP / "player-spell.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def spells(source):
    result = {}
    name = None
    for line in source.splitlines():
        if line.startswith("spell:"):
            name = line[6:].split(":", 1)[0]
            result.setdefault(name, "")
        elif line.startswith("desc:") and name:
            result[name] += line[5:]
        elif line.startswith(("name:", "book:")):
            name = None
    return result

standard = spells(ANG)
fa = spells(FA)
extra = set(fa) - set(standard)
assert extra == {"Timeless Blink", "Fume of Thangorodrim"}
assert len(standard) == 133 and len(fa) == 133
rows = re.findall(
    r'^\s*\{\s*"([^"]+)",\s*\n\s*"([^"]+)",\s*\n\s*"([^"]+)",\s*\n\s*"([^"]+)"\s*\},$',
    SUPPLEMENT, re.M)
assert len(rows) == 2
assert {name for name, _, _, _ in rows} == extra
for name, english, translated_name, translated_description in rows:
    assert fa[name] == english, (name, fa[name], english)
    assert translated_name and translated_description
    assert translated_name != name and translated_description != english
assert '#include "zh-tw-faangband-spells.h"' in NAMES
assert '#include "zh-tw-faangband-spells.h"' in DESCRIPTIONS
assert "angdroid_zh_tw_faangband_spell_name(en)" in NAMES
assert "return fa_name ? fa_name : en;" in NAMES
assert "angdroid_zh_tw_faangband_spell_description(name, english)" in DESCRIPTIONS
assert "return fa_description ? fa_description : english;" in DESCRIPTIONS
assert "strcmp(english_description, entry->english_description) != 0" in SUPPLEMENT
assert "if (!en || !zh_tw) return en;" in NAMES
assert "if (!english || !zh_tw || !name) return english;" in DESCRIPTIONS
assert "angdroid_zh_tw_spell_name(spell->name," in UI
assert "angdroid_zh_tw_spell_description(spell->name," in UI
assert "angdroid_zh_tw_spell_name(spell->name, true)" in KNOWLEDGE
assert "angdroid_zh_tw_spell_name(spell->name, true)" in LEARN
assert "test_faangband_spells_locale.c" in NATIVE
assert "tests/test_faangband_spells_source.py" in VERIFY
print("PASS: two FAangband-only spells use exact descriptions and existing UI paths")
