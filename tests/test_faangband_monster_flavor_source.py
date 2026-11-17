#!/usr/bin/env python3
"""Ensure FAangband monster names and lore are source-exact without stale Angband text."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
def content(path):
    return (ROOT / path).read_text(encoding="utf-8")
FA = content("variant_lib/faangband/gamedata/monster.txt")
ANG = content("variant_lib/angband/gamedata/monster.txt")
HDR = content("app/src/main/cpp/angband/zh-tw-faangband-monster-flavor.h")
NAMES = content("app/src/main/cpp/angband/zh-tw-monsters.h")
UNIQUE = content("app/src/main/cpp/angband/zh-tw-unique-monsters.h")
ORIGINAL_LORE = content("app/src/main/cpp/angband/zh-tw-lore.h")
LORE_SOURCE = content("app/src/main/cpp/angband/mon-lore.c")
LORE_UI = content("app/src/main/cpp/angband/ui-mon-lore.c")
NATIVE = content("scripts/test-native-utf8.sh")
VERIFY = content("scripts/verify-zh-tw.sh")

def monsters(data):
    records, current = [], None
    for line in data.splitlines():
        if line.startswith("name:"):
            if current is not None:
                records.append(current)
            current = {"name": line[5:], "desc": ""}
        elif current is not None and line.startswith("desc:"):
            current["desc"] += line[5:]
    if current is not None:
        records.append(current)
    return records

def section(first, last):
    start = HDR.split(first, 1)[1]
    return start.split(last, 1)[0]

def pairs(s):
    result = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', s, re.M)
    assert len(result) == len({x[0] for x in result})
    return dict(result)

def triples(s):
    result = re.findall(
        r'^\s*\{ "([^"\n]+)", "([^"\n]+)", "([^"\n]+)" \},$', s, re.M)
    assert len(result) == len({x[0] for x in result})
    return {name: (source, zh) for name, source, zh in result}

fa, ang = monsters(FA), monsters(ANG)
assert len(fa) == len(ang) == 624
by_ang = {x["name"]: x["desc"] for x in ang}
by_fa = {x["name"]: x["desc"] for x in fa}
exclusive = set(by_fa) - set(by_ang)
changed = {name for name in set(by_fa) & set(by_ang)
           if by_fa[name] != by_ang[name]}
assert len(exclusive) == 92
assert len(changed) == 8

names = pairs(section(
    "static const struct { const char *en, *zh; } names[] = {", "    };"))
flavor = triples(section(
    "static const struct {const char *name, *source, *zh;} flavors[] = {", "    };"))
standard = pairs(section(
    "static const struct {const char *name, *source;} standard[] = {", "    };"))
assert len(names) == 92
assert len(flavor) == 99
assert set(names) == exclusive
assert set(flavor) == (set(names) - {"skeleton lord"}) | changed
assert set(standard) == changed
assert all(flavor[name] == (by_fa[name], flavor[name][1])
           and flavor[name][1] for name in flavor)
assert all(by_ang[name] == standard[name] for name in changed)
assert all(names[name] for name in names)
assert all(flavor[name][1] for name in names if name != "skeleton lord")
assert by_fa["skeleton lord"] == ""
assert "skeleton lord" not in flavor
assert set(names).isdisjoint(set(re.findall(
    r'^\s*\{ "([^"\n]+)", "[^"\n]+" \},$', NAMES, re.M)))
assert set(names).isdisjoint(set(re.findall(
    r'^\s*\{ "([^"\n]+)", "[^"\n]+" \},$', UNIQUE, re.M)))
assert "return canonical_text;" in HDR
assert "return NULL;" in HDR
assert "strcmp(canonical_text, flavors[i].source) == 0" in HDR
assert "strcmp(canonical_text, standard[j].source) == 0" in HDR
assert "return angdroid_faangband_monster_name(english);" in NAMES
assert "return angdroid_faangband_monster_name(english);" in UNIQUE
assert "angdroid_faangband_monster_flavor(race->name, race->text)" in LORE_SOURCE
assert "angdroid_zh_tw_monster_flavor(race->name)" in LORE_SOURCE
assert 'localized ? localized : race->text' in LORE_SOURCE
assert 'angdroid_zh_tw_monster_list_name(race->name)' in LORE_UI
assert 'angdroid_zh_tw_unique_monster_name(race->name)' in LORE_UI
assert "#define ANGDROID_ZH_TW_MONSTER_LORE 623" in ORIGINAL_LORE
assert "#define ANGDROID_ZH_TW_MONSTER_NAMES 522" in NAMES
assert "#define ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES 101" in UNIQUE
assert "tests/native/test_faangband_monster_flavor.c" in NATIVE
assert "tests/native/test_faangband_monster_flavor_remaining.c" in NATIVE
assert "tests/test_faangband_monster_flavor_source.py" in VERIFY
print("PASS: 92 FA-only names, 91 FA-only lore and 8 divergent descriptions")
