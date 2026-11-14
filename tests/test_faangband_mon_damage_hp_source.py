#!/usr/bin/env python3
"""Guard production FAangband int16_t monster HP updates against overflow."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-util.c")
MONSTER = read("app/src/main/cpp/faangband/monster.h")
HELPER = read("app/src/main/cpp/faangband/mon-damage-hp.h")
PROJECTION = read("app/src/main/cpp/faangband/project-mon.c")
ATTACK = read("app/src/main/cpp/faangband/effect-handler-attack.c")
GENERAL = read("app/src/main/cpp/faangband/effect-handler-general.c")
MON_MAKE = read("app/src/main/cpp/faangband/mon-make.c")
MON_DATA = read("variant_lib/faangband/gamedata/monster.txt")
PARSER = read("app/src/main/cpp/faangband/parser.c")
GHOST_DATA = read("variant_lib/faangband/gamedata/ghost.txt")
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

# Batch 267: clamp generated int HP before writing to int16_t maxhp.
spawn = MON_MAKE.split("/* Uniques keep fixed HP", 1)[1].split(
    "/* And start out fully healthy */", 1)[0]
assert '#include "mon-damage-hp.h"' in MON_MAKE
assert "static inline int16_t angdroid_fa_mon_maxhp_from_roll(" in HELPER
assert "if (rolled_hp < 1) return 1;" in HELPER
assert "if (rolled_hp > INT16_MAX) return INT16_MAX;" in HELPER
assert "int generated_hp = rf_has(race->flags, RF_UNIQUE) ?" in spawn
assert "race->avg_hp : MAX(mon_hp(race, RANDOMISE), 1);" in spawn
assert "mon->maxhp = angdroid_fa_mon_maxhp_from_roll(generated_hp);" in spawn
assert "mon->maxhp = mon_hp(race, RANDOMISE);" not in spawn
assert "mon->hp = mon->maxhp;" in MON_MAKE
assert "angdroid_fa_mon_maxhp_from_roll(INT_MAX)" in NATIVE
assert "for (roll = -50000; roll <= 50000; roll += 97)" in NATIVE

# The current bundled monster.txt is below the 16-bit HP ceiling.
races = []
entry = None
for line in MON_DATA.splitlines():
    if line.startswith("name:"):
        entry = {"name": line[5:], "hp": None, "flags": ""}
        races.append(entry)
    elif entry is not None and line.startswith("hit-points:"):
        # Match the game's parser_getint(): strtol() reads an integer
        # prefix even when legacy lines contain "6d9"/"4d9"/"10d20".
        raw = line.partition(":")[2]
        match = re.match(r"^[+-]?\d+", raw)
        assert match is not None, (entry["name"], raw)
        entry["hp"] = int(match.group())
        entry["raw_hp"] = raw
    elif entry is not None and line.startswith("flags:"):
        entry["flags"] += " " + line.partition(":")[2]
defined = [r for r in races if r["hp"] is not None]
assert len(races) == 624
assert len(defined) >= 610
assert "v->u.ival = strtol(tok, &z, 0);" in PARSER
legacy_hp = sorted(re.findall(
    r"^hit-points:(\d+d\d+)$", MON_DATA, re.MULTILINE))
assert legacy_hp == ["10d20", "4d9", "6d9"]
# These three values are also present in NickMcConnell/FAangband main.
# The parser currently accepts the integer prefix, not dice notation.
legacy_monsters = {
    r["name"]: (r["raw_hp"], r["hp"])
    for r in defined
    if re.fullmatch(r"\d+d\d+", r["raw_hp"])
}
assert legacy_monsters == {
    "petty-dwarven rogue": ("6d9", 6),
    "gazing eye": ("4d9", 4),
    "animist": ("10d20", 10),
}
parse_int = PARSER.split("if (t == PARSE_T_INT) {", 1)[1].split(
    "} else if (t == PARSE_T_UINT)", 1)[0]
assert "v->u.ival = strtol(tok, &z, 0);" in parse_int
assert "if (z == tok)" in parse_int
assert "if (*z" not in parse_int
assert all(r["hp"] > 0 for r in defined)
assert max(r["hp"] for r in defined) == 20000
nonunique = [r for r in defined if "UNIQUE" not in
             r["flags"].replace("|", " ").split()]
assert max(r["hp"] for r in nonunique) == 7500

def max_possible_hp(hp):
    dev = (((hp * 10) // 8) + 5) // 10
    if hp > 1:
        dev += 1
    return hp + 4 * dev

assert max(max_possible_hp(r["hp"]) for r in nonunique) == 11256

modifiers = [(int(a), int(b)) for a, b in re.findall(
    r'^hit-points:(\d+):(\d+)$', GHOST_DATA, re.MULTILINE)]
assert modifiers and all(a > 0 and b > 0 for a, b in modifiers)
ghosts = [r for r in defined if "PLAYER_GHOST" in
          r["flags"].replace("|", " ").split()]
assert max(r["hp"] for r in ghosts) == 4000
assert max(a / b for a, b in modifiers) <= 4 / 3

# Heal events must check the remaining space before adding; both player
# projections and monster spells use the same tested C99 helper.
assert "static inline int16_t angdroid_fa_mon_hp_after_healing(" in HELPER
assert "amount <= 0 || maxhp <= 0" in HELPER
assert "amount >= (int64_t)maxhp - (int64_t)hp" in HELPER

project_heal = PROJECTION.split(
    "static void project_monster_handler_MON_HEAL(", 1)[1].split(
    "static void project_monster_handler_MON_SPEED(", 1)[0]
project_attack = PROJECTION.split(
    "static bool project_m_monster_attack(", 1)[1].split(
    "static bool project_m_player_attack(", 1)[0]
self_heal = ATTACK.split("bool effect_handler_MON_HEAL_HP(", 1)[1].split(
    "bool effect_handler_MON_HEAL_KIN(", 1)[0]
kin_heal = ATTACK.split("bool effect_handler_MON_HEAL_KIN(", 1)[1].split(
    " * Cast a bolt spell", 1)[0]
quake = ATTACK.split("/* Preserve lethal quake damage before storing int16 HP. */", 1)[1].split(
    "if (mon->hp < 0)", 1)[0]
mana = GENERAL.split("/* Heal the monster */", 1)[1].split(
    "/* Redraw mana */", 1)[0]

for code in (PROJECTION, ATTACK, GENERAL):
    assert '#include "mon-damage-hp.h"' in code
assert "angdroid_fa_mon_hp_after_healing(" in project_heal
assert "context->mon->hp, context->mon->maxhp, context->dam" in project_heal
assert "context->mon->hp += context->dam;" not in project_heal
assert "angdroid_fa_mon_hp_after_damage(mon->hp, dam)" in project_attack
assert "mon->hp -= dam;" not in project_attack
for body in (self_heal, kin_heal):
    assert "angdroid_fa_mon_hp_after_healing(" in body
    assert "mon->hp, mon->maxhp, amount" in body
assert "mon->hp += amount;" not in self_heal
assert "MIN(mon->hp + amount, mon->maxhp)" not in kin_heal
assert "angdroid_fa_mon_hp_after_damage(" in quake
assert "mon->hp, m_dam" in quake
assert "mon->hp -= m_dam;" not in quake
assert "angdroid_fa_mon_hp_after_healing(" in mana
assert "(int64_t)6 * drain" in mana
assert "mon->hp += (6 * drain);" not in mana

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
    "angdroid_fa_mon_hp_after_healing(100, 200, INT64_MAX)",
    "angdroid_fa_mon_hp_after_healing(100, 0, 50)",
    "for (maxhp = 1; maxhp <= INT16_MAX; maxhp += 173)",
):
    assert token in NATIVE, token
assert "test_fa_mon_damage_hp.c" in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-mon-damage-hp.sh" in VERIFY
assert "test_faangband_mon_damage_hp_source.py" in VERIFY

print("PASS: FAangband player/non-player monster HP clamping source contract")
