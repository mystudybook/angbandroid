#!/usr/bin/env python3
"""Compare each FAangband-only player property with exact supplemental translation."""
from pathlib import Path
import re
ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/player_property.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/player_property.txt").read_text(encoding="utf-8")
EXTRA = (CPP / "zh-tw-faangband-abilities.h").read_text(encoding="utf-8")
BASE = (CPP / "zh-tw-player-ability.h").read_text(encoding="utf-8")
UI = (CPP / "ui-player-properties.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
def entries(text):
    out = []
    entry = {}
    for line in text.splitlines():
        match = re.match(r"^(type|code|name|desc):(.*)$", line)
        if not match:
            continue
        key, val = match.groups()
        if key == "type":
            if entry.get('type'): out.append(entry)
            entry = {'type': val}
        elif key == "desc":
            entry[key] = entry.get(key, '') + val
        else:
            entry[key] = val
    if entry.get('type'): out.append(entry)
    return out
standard = {(x.get('type'), x.get('code')) for x in entries(ANG)}
fa_only = [x for x in entries(FA) if x.get('type') == 'player' and x.get('code') and
           (x.get('type'), x.get('code')) not in standard]
assert len(fa_only) == 64
def pairs(segment):
    return re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', segment, re.M)
names = EXTRA.split('static inline const char *angdroid_faangband_ability_name(', 1)[1]
names = names.split('static inline const char *angdroid_faangband_ability_description(', 1)[0]
descs = EXTRA.split('static inline const char *angdroid_faangband_ability_description(', 1)[1]
name_pairs = pairs(names)
desc_pairs = pairs(descs)
assert len(name_pairs) == len(desc_pairs) == len(fa_only) == 64
assert len(set(x for x, _ in name_pairs)) == len(name_pairs)
assert len(set(x for x, _ in desc_pairs)) == 64
mapped_name = dict(name_pairs)
mapped_desc = dict(desc_pairs)
assert {x['name'] for x in fa_only} == set(mapped_name)
assert {x['desc'] for x in fa_only} == set(mapped_desc)
assert all(mapped_name[x['name']] and mapped_desc[x['desc']] for x in fa_only)
assert '#include "zh-tw-faangband-abilities.h"' in BASE
assert 'angdroid_faangband_ability_name(english, zh_tw)' in BASE
assert 'angdroid_faangband_ability_description(english, zh_tw)' in BASE
assert 'angdroid_ability_name(choices[oid].name' in UI
assert 'angdroid_ability_description(choices[oid].desc' in UI
assert 'test_faangband_abilities_locale.c' in NATIVE
assert 'test_faangband_abilities_source.py' in VERIFY
assert "You may obtain a perfect success rate with magic." not in mapped_desc
print('PASS: all 64 FAangband-only player abilities, display-only fallbacks')
