#!/usr/bin/env python3
"""Exact player_timed.txt grade and terrain/trap status-display contracts.

This checks repository source, not the Android/NDK integration build.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
GAMEDATA = ROOT / "variant_lib"
TIMED = (CPP / "angband/zh-tw-timed-status.h").read_text(encoding="utf-8")
TERRAIN = (CPP / "angband/zh-tw-terrain-trap.h").read_text(encoding="utf-8")
FA_TRAPS = (CPP / "angband/zh-tw-faangband-traps.h").read_text(encoding="utf-8")
SCRIPT = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


def pairs(text: str) -> dict[str, str]:
    entries = re.findall(r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}', text)
    result = dict(entries)
    assert len(result) == len(entries), "duplicate translation keys"
    return result


def section(text: str, start: str, end: str) -> str:
    return text.split(start, 1)[1].split(end, 1)[0]


def source_function(text: str, signature: str) -> str:
    pos = text.index(signature)
    return text[pos:text.index("\n}\n", pos) + 2]


grades = pairs(TIMED)
assert len(grades) == 66, len(grades)
assert 'if (!english || !zh_tw) return english;' in TIMED
assert 'strcmp(english, labels[i].english) == 0' in TIMED
assert 'return english;' in TIMED
for engine in ("angband", "faangband"):
    raw = (GAMEDATA / engine / "gamedata/player_timed.txt").read_text(
        encoding="utf-8")
    canonical = {
        line.split(":", 4)[3] for line in raw.splitlines()
        if line.startswith("grade:")
    }
    assert canonical == set(grades), (engine, sorted(canonical - set(grades)))

terrain_names = pairs(section(
    TERRAIN, "static inline const char *angdroid_terrain_name(",
    "static inline const char *angdroid_terrain_lore("))
trap_names = pairs(section(
    TERRAIN, "static inline const char *angdroid_trap_name(",
    "static inline const char *angdroid_trap_lore("))
trap_names.update(pairs(FA_TRAPS))
for engine in ("angband", "faangband"):
    raw_terrain = (GAMEDATA / engine / "gamedata/terrain.txt").read_text(
        encoding="utf-8")
    canonical_terrain = {
        line[5:] for line in raw_terrain.splitlines()
        if line.startswith("name:")
    }
    assert canonical_terrain <= set(terrain_names), (
        engine, sorted(canonical_terrain - set(terrain_names)))
    raw_trap = (GAMEDATA / engine / "gamedata/trap.txt").read_text(
        encoding="utf-8")
    canonical_trap = {
        line.split(":")[-1] for line in raw_trap.splitlines()
        if line.startswith("name:")
    }
    assert canonical_trap <= set(trap_names), (
        engine, sorted(canonical_trap - set(trap_names)))

for engine, helper, include in (
    ("angband", "ang_status_put", '"zh-tw-timed-status.h"'),
    ("faangband", "fa_status_put", '"../angband/zh-tw-timed-status.h"'),
):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    assert include in src, engine
    assert '"zh-tw-terrain-trap.h"' in src or (
        '"../angband/zh-tw-terrain-trap.h"' in src), engine

    timed = source_function(src, "static size_t prt_tmd(")
    assert "if (player->timed[i])" in timed, engine
    assert "while (player->timed[i] > grade->max)" in timed, engine
    assert "if (!grade->name) continue;" in timed, engine
    assert "angdroid_timed_status_name(grade->name," in timed, engine
    assert timed.count(helper + "(grade->color,") == 2, engine
    assert "grade->color" in timed, engine
    assert "i == TMD_FOOD" in timed, engine
    assert 'player->timed[i] / 100' in timed, engine
    assert 'format("%d %%",' in timed, engine
    assert "strlen(grade->name)" not in timed, engine

    terrain = source_function(src, "static size_t prt_terrain(")
    assert 'detected_trap = trap && !square_isinvis' in terrain, engine
    assert "square_isdownstairs(cave, player->grid)" in terrain, engine
    assert "square_isupstairs(cave, player->grid)" in terrain, engine
    assert "english_name = trap->kind->name" in terrain, engine
    assert "english_name = feat->name" in terrain, engine
    assert "angdroid_trap_name(english_name, true)" in terrain, engine
    assert "angdroid_terrain_name(english_name, true)" in terrain, engine
    assert "visible_name != english_name" in terrain, engine
    assert "return " + helper + "(attr, visible_name, row, col);" in terrain
    assert "my_strcpy(buf, english_name, sizeof(buf))" in terrain, engine
    assert "my_strcap(buf);" in terrain, engine
    assert 'c_put_str(attr, format("%s ", buf), row, col)' in terrain, engine
    assert "return longest_terrain_name() + 1;" in terrain, engine

assert "test_timed_status_locale.c" in NATIVE
assert "test_timed_terrain_status_source.py" in SCRIPT
print("PASS: 66 grade translations and canonical terrain/trap UI paths in both engines")
