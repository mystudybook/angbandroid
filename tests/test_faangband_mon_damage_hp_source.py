#!/usr/bin/env python3
"""Guard production FAangband int16_t monster HP updates against overflow."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-util.c")
MONSTER = read("app/src/main/cpp/faangband/monster.h")
HELPER = read("app/src/main/cpp/faangband/mon-damage-hp.h")
NATIVE = read("tests/native/test_fa_mon_damage_hp.c")
RUNNER = read("scripts/test-fa-mon-damage-hp.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

nonplayer = SOURCE.split("bool mon_take_nonplayer_hit(", 1)[1].split(
    "bool mon_take_hit(", 1)[0]
player = SOURCE.split("bool mon_take_hit(", 1)[1].split(
    "void kill_arena_monster(", 1)[0]

assert "int16_t hp;" in MONSTER and "int16_t maxhp;" in MONSTER
assert '#include "mon-damage-hp.h"' in SOURCE
assert "int64_t remaining = (int64_t)hp - (int64_t)damage;" in HELPER
assert "if (remaining < INT16_MIN) return INT16_MIN;" in HELPER
assert "if (remaining > INT16_MAX) return INT16_MAX;" in HELPER
assert "return (int16_t)remaining;" in HELPER

assert "t_mon->hp = angdroid_fa_mon_hp_after_damage(t_mon->hp, dam);" in nonplayer
assert "mon->hp = angdroid_fa_mon_hp_after_damage(mon->hp, dam);" in player
assert "t_mon->hp -= dam;" not in nonplayer
assert "mon->hp -= dam;" not in player
assert 'if (monster_is_unique(t_mon) || player->upkeep->arena_level)' in nonplayer
assert "if (dam > t_mon->hp) dam = t_mon->hp;" in nonplayer
assert nonplayer.index("if (dam > t_mon->hp) dam = t_mon->hp;") < (
    nonplayer.index("angdroid_fa_mon_hp_after_damage("))
assert "if (t_mon->hp < 0) {" in nonplayer
assert "if (mon->hp < 0) {" in player
assert "if (dam == 0) return false;" in player
assert "player_kill_monster(mon, p, note);" in player
assert "monster_death(t_mon, player, false);" in nonplayer
assert "(*fear) = monster_scared_by_damage(mon, dam);" in player

for token in (
    "INT_MIN", "INT_MAX", "INT16_MIN", "INT16_MAX",
    "angdroid_fa_mon_hp_after_damage(100, 100000)",
    "angdroid_fa_mon_hp_after_damage(100, INT_MIN)",
    "for (hp = INT16_MIN; hp <= INT16_MAX; ++hp)",
):
    assert token in NATIVE, token
assert "test_fa_mon_damage_hp.c" in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-mon-damage-hp.sh" in VERIFY
assert "test_faangband_mon_damage_hp_source.py" in VERIFY

print("PASS: FAangband player/non-player monster HP clamping source contract")
