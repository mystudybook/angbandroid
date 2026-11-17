#!/usr/bin/env python3
"""FAangband-only terrain labels, lore and damaging terrain feedback contract."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/terrain.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/terrain.txt").read_text(encoding="utf-8")
HDR = (CPP / "zh-tw-terrain-trap.h").read_text(encoding="utf-8")
UI = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
CMD = (CPP / "cmd-cave.c").read_text(encoding="utf-8")
UTIL = (CPP / "player-util.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def feature_names(text):
    return re.findall(r"^name:([^\r\n]+)$", text, re.M)

def names_in_function(name, next_name):
    text = HDR.split("static inline const char *" + name + "(", 1)[1]
    if next_name:
        text = text.split("static inline const char *" + next_name + "(", 1)[0]
    pairs = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', text, re.M)
    assert len(pairs) == len({en for en, _ in pairs})
    return dict(pairs)

fa_names, ang_names = feature_names(FA), feature_names(ANG)
assert len(fa_names) == 41 and len(ang_names) == 25
unique = set(fa_names) - set(ang_names)
assert len(unique) == 16
assert unique == {
    "Travelling Merchant", "easy path north", "hard path north",
    "easy path east", "hard path east", "easy path south",
    "hard path south", "easy path west", "hard path west", "water",
    "lowland trees", "highland trees", "grass", "road", "empty space", "sand dune",
}
terrain = names_in_function("angdroid_terrain_name", "angdroid_terrain_lore")
lore = names_in_function("angdroid_terrain_lore", "angdroid_terrain_warning")
warnings = names_in_function("angdroid_terrain_warning", "angdroid_terrain_hurt_message")
hurt = names_in_function("angdroid_terrain_hurt_message", "angdroid_trap_name")
assert set(fa_names) <= terrain.keys()
assert set(fa_names) - {"unknown grid"} <= lore.keys()
assert all(terrain[name] and lore[name] for name in unique)
canonical_warnings = {line.split(":", 1)[1]
    for line in FA.splitlines() if line.startswith(("walk-msg:", "run-msg:"))}
assert len(canonical_warnings) == 4
assert canonical_warnings == warnings.keys()
assert hurt == {"You fall into darkness!": "你墜入黑暗之中！"}
assert "hurt-msg:You fall into darkness!" in FA
assert "die-msg:falling off a precipice" in FA

assert "angdroid_terrain_name(feat->name, angdroid_zh_tw_enabled())" in UI
assert "angdroid_terrain_lore(" in UI
assert '#include "zh-tw-terrain-trap.h"' in CMD
assert "get_check(angdroid_terrain_warning(feat->run_msg," in CMD
assert "get_check(angdroid_terrain_warning(feat->walk_msg," in CMD
assert "player_check_terrain_damage(player," in CMD
assert "dam_taken > player->chp / 3" in CMD
assert "player->upkeep->running && dam_taken" in CMD
assert "take_hit(p, dam_reduced, square_feat(cave, grid)->die_msg);" in UTIL
assert "angdroid_terrain_hurt_message(" in UTIL
assert "inven_damage(p, PROJ_FIRE, dam_taken);" in UTIL
assert "test_faangband_terrain_locale.c" in NATIVE
assert "test_faangband_terrain_source.py" in VERIFY
print("PASS: FAangband 16 additional terrains, cliff warnings and display-only damage")
