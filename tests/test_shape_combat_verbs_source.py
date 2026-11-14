#!/usr/bin/env python3
"""Verify full Angband/FAangband shape-attack verb coverage and UI isolation."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
COMBAT = (CPP / "zh-tw-combat.h").read_text(encoding="utf-8")
ATTACK = (CPP / "player-attack.c").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/shape.txt").read_text(encoding="utf-8")
FA = (ROOT / "variant_lib/faangband/gamedata/shape.txt").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def shape_verbs(source):
    return set(re.findall(r"^blow:([^\r\n]+)$", source, re.M))

ang = shape_verbs(ANG)
fa = shape_verbs(FA)
assert ang == fa
assert len(ang) == 14

melee = COMBAT.split("static inline const char *angdroid_melee_verb_zh_tw(", 1)[1]
melee = melee.split("static inline const char *angdroid_ranged_verb_zh_tw(", 1)[0]
pairs = re.findall(r'^\s*\{"([^"\n]+)", "([^"\n]+)"\},', melee, re.M)
mapping = dict(pairs)
assert len(pairs) == len(mapping)
assert ang <= set(mapping)
assert all(mapping[verb] != verb and mapping[verb].strip() for verb in ang)
assert 'if (!english) return NULL;' in COMBAT
assert 'return NULL;' in COMBAT

# Preserve exact canonical blow selection, turn logic and English message fallback.
assert "my_strcpy(verb, blow->name, sizeof(verb));" in ATTACK
assert "angdroid_melee_verb_zh_tw(verb)" in ATTACK
assert "angdroid_zh_tw_enabled() ?" in ATTACK
assert 'msgt(msg_type, "You %s %s%s.", verb, m_name, dmg_text);' in ATTACK
assert "if (verb_zh && (!melee_hit_types[i].text || quality_zh))" in ATTACK
assert "test_shape_combat_verbs.c" in NATIVE
assert "test_shape_combat_verbs_source.py" in VERIFY
print("PASS: all 14 Angband/FAangband shape attack verbs and UI-only fallback")
