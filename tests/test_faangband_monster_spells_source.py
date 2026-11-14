#!/usr/bin/env python3
"""FAangband-exclusive monster symbol text, spell lore, and rendered messages."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
def content(path):
    return (ROOT / path).read_text(encoding="utf-8")
FA_BASE = content("variant_lib/faangband/gamedata/monster_base.txt")
ANG_BASE = content("variant_lib/angband/gamedata/monster_base.txt")
FA_SPELL = content("variant_lib/faangband/gamedata/monster_spell.txt")
ANG_SPELL = content("variant_lib/angband/gamedata/monster_spell.txt")
SYMBOL = content("app/src/main/cpp/angband/zh-tw-symbol-query.h")
SYMBOL_FA = content("app/src/main/cpp/angband/zh-tw-faangband-monster-base.h")
LORE = content("app/src/main/cpp/angband/zh-tw-mon-lore-terms.h")
LORE_FA = content("app/src/main/cpp/angband/zh-tw-faangband-mon-spell-lore.h")
SPELL = content("app/src/main/cpp/angband/zh-tw-mon-spell-text.h")
SPELL_FA = content("app/src/main/cpp/angband/zh-tw-faangband-mon-spell-messages.h")
UI = content("app/src/main/cpp/angband/ui-knowledge.c")
MON_LORE = content("app/src/main/cpp/angband/mon-lore.c")
MON_SPELL = content("app/src/main/cpp/angband/mon-spell.c")
RUNNER = content("scripts/test-native-utf8.sh")
VERIFY = content("scripts/verify-zh-tw.sh")

def bases(s):
    rows, name, desc = [], None, ""
    for line in s.splitlines():
        if line.startswith("name:"):
            if name is not None:
                rows.append((name, desc))
            name, desc = line[5:], ""
        elif name is not None and line.startswith("desc:"):
            desc += line[5:]
    if name is not None:
        rows.append((name, desc))
    return rows

def values(source, field):
    return set(re.findall(r"^" + re.escape(field) + r":(.+)$", source, re.M))

def pairs(source):
    found = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', source, re.M)
    assert len(found) == len({en for en, _ in found})
    return dict(found)

fa_base, ang_base = bases(FA_BASE), bases(ANG_BASE)
assert len(fa_base) == len(ang_base) == 56
ang = dict(ang_base)
assert [(name, desc) for name, desc in fa_base if name not in ang] == [
    ("flying dragon", "Winged Dragon")
]
assert all(ang[name] == desc for name, desc in fa_base if name in ang)
assert '{ "flying dragon", "Winged Dragon", "有翼巨龍" },' not in SYMBOL
assert 'strcmp(name, "flying dragon") == 0' in SYMBOL_FA
assert 'strcmp(source, "Winged Dragon") == 0' in SYMBOL_FA
assert 'angdroid_faangband_monster_base_text(base_name,' in SYMBOL
assert 'angdroid_symbol_monster_base_text(' in UI

ang_lore, fa_lore = values(ANG_SPELL, "lore"), values(FA_SPELL, "lore")
extra_lore = fa_lore - ang_lore
assert len(extra_lore) == 7
assert extra_lore == pairs(LORE_FA).keys()
assert 'angdroid_faangband_mon_spell_lore(en, zh_tw)' in LORE
assert 'angdroid_zh_tw_monster_spell(' in MON_LORE

kinds = ("message-vis", "message-invis", "message-miss", "message-save")
fa_messages = set().union(*(values(FA_SPELL, key) for key in kinds))
ang_messages = set().union(*(values(ANG_SPELL, key) for key in kinds))
extra_messages = fa_messages - ang_messages
assert len(extra_messages) == 14
translated = pairs(SPELL_FA)
assert translated.keys() == extra_messages
assert all(translated[s] and translated[s] != s for s in extra_messages)
for en, zh in translated.items():
    for tag in ("name", "target", "type", "oftype"):
        assert (f"{{{tag}}}" in en) == (f"{{{tag}}}" in zh)
assert translated["{name} summons swamp creaturess."] == "{name}召喚了沼澤生物。"
assert 'angdroid_faangband_mon_spell_text(english, zh_tw)' in SPELL
assert 'in_cursor = angdroid_mon_spell_text(in_cursor,' in MON_SPELL
assert 'angdroid_mon_spell_text(level->save_message,' in MON_SPELL
assert "test_faangband_monster_spells.c" in RUNNER
assert "test_faangband_monster_spells_source.py" in VERIFY
print("PASS: FA winged dragon, 7 exclusive monster lore and 14 spell messages")
