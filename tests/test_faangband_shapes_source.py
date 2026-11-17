#!/usr/bin/env python3
"""FAangband-only shape UI names, flying flag and original data isolation."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
STANDARD = (ROOT / "variant_lib/angband/gamedata/shape.txt").read_text(encoding="utf-8")
FA = (ROOT / "variant_lib/faangband/gamedata/shape.txt").read_text(encoding="utf-8")
FLAGS = (ROOT / "variant_lib/faangband/gamedata/player_property.txt").read_text(encoding="utf-8")
HELPER = (CPP / "zh-tw-faangband-shapes.h").read_text(encoding="utf-8")
GENERAL = (CPP / "zh-tw-shape-lore.h").read_text(encoding="utf-8")
UI = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def shapes(text):
    return set(re.findall(r"^name:(.+)$", text, re.M))

standard = shapes(STANDARD)
fa = shapes(FA)
added = fa - standard
assert len(standard) == 9
assert len(fa) == 23
assert len(added) == 14
pairs = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', HELPER, re.M)
assert len(pairs) == 14
assert {name for name, _ in pairs} == added
assert len({name for name, _ in pairs}) == 14
assert all(translation and translation != name for name, translation in pairs)
assert sum(name.endswith("dragon") for name, _ in pairs) == 12
assert "player-flags:FLYING" in FA
assert "code:FLYING\nname:Flying\ndesc:You can fly." in FLAGS
assert 'strcmp(english, "You can fly.") == 0' in HELPER
assert '"你能飛行。"' in HELPER
assert '#include "zh-tw-faangband-shapes.h"' in GENERAL
assert "angdroid_faangband_shape_name(english, zh_tw)" in GENERAL
assert "angdroid_faangband_shape_ability_desc(english, zh_tw)" in GENERAL
assert 'angdroid_shape_name(s->name, angdroid_zh_tw_enabled())' in UI
assert 'angdroid_shape_name(sarray[i]->name, angdroid_zh_tw_enabled())' in UI
assert "angdroid_shape_misc_desc(ability->desc," in UI
assert 'test_faangband_shapes_locale.c' in NATIVE
assert 'tests/test_faangband_shapes_source.py' in VERIFY
print("PASS: 14 FAangband-only shapes, flying flag and display-only paths")
