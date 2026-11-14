#!/usr/bin/env python3
"""Guard FAangband monster/world event translation without changing game data."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT / "app/src/main/cpp"
MON = (BASE / "faangband/mon-move.c").read_text(encoding="utf-8")
WORLD = (BASE / "faangband/game-world.c").read_text(encoding="utf-8")
CAVE = (BASE / "faangband/cave-square.c").read_text(encoding="utf-8")
DICT = (BASE / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
TERRAIN = (ROOT / "variant_lib/faangband/gamedata/terrain.txt").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_fa_monster_world_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-fa-monster-world-locale.sh").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

pair_re = re.compile(r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
                     re.MULTILINE)

def read_pairs(content):
    return [(json.loads(a), json.loads(b)) for a, b in pair_re.findall(content)]

native_pairs = read_pairs(NATIVE)
dictionary = read_pairs(DICT)
keys = [p[0] for p in native_pairs]
assert len(native_pairs) == 19
assert len(set(keys)) == 19
for pair in native_pairs:
    assert pair in dictionary, "Missing production translation: " + pair[0]
    assert sum(item[0] == pair[0] for item in dictionary) == 1, (
        "Duplicate translation key: " + pair[0])

# Terrain action keys are data, never translated in the parser or terrain file.
terrain_actions = {line.split(":", 1)[1] for line in TERRAIN.splitlines()
                   if line.startswith("confused-msg:")}
assert terrain_actions == {"bangs into a door", "bashes into a wall",
                           "bumps into some rocks"}
assert terrain_actions | {"stumbles"} <= set(keys)
assert 'const char *localized = angdroid_game_ui_text(action,' in MON
assert "localized != action" in MON
assert 'msg("%s %s.", m_name, action);' in MON
assert 'msg("%s%s。", m_name, localized);' in MON

mon_formatted = [
    ("%s slams against the door.", "%s猛撞門扉。"),
    ("%s fiddles with the lock.", "%s試著撬開門鎖。"),
    ("%s tries to pick up %s, but fails.", "%s試著撿起%s，但失敗了。"),
    ("%s picks up %s.", "%s撿起了%s。"),
    ("%s crushes %s.", "%s壓碎了%s。"),
    ("%s says: '%s'", "%s說：「%s」"),
    ("%s wakes up.", "%s醒了過來。"),
    ("%s %s %s.", "%s踩過了%s。"),
    ("%s %s %s.", "%s推開了%s。")
]
for en, zh in mon_formatted:
    assert json.dumps(en, ensure_ascii=False) in MON, "English monster msg: " + en
    assert json.dumps(zh, ensure_ascii=False) in MON, "Chinese monster msg: " + zh

assert 'kill_ok ? "tramples over" : "pushes past"' in MON
assert "monster_is_visible(mon) && monster_is_in_view(mon)" in MON
assert "square_set_door_lock(cave, new, k - 1);" in MON
assert "monster_carry(cave, mon, taken)" in MON
assert "square_delete_object(cave, new, obj, true, true)" in MON
assert "cave->ghost->has_spoken = true;" in MON
assert "equip_learn_flag(player, OF_AGGRAVATE);" in MON

world_static = [
    "Creatures of the darkness flee from the sunlight!",
    "The sun has risen.", "The sun has fallen.",
    "The Black Breath sickens you.",
    "The Black Breath saps your strength.",
    "The Black Breath dims your life force.",
    "You faint from the lack of food.",
    "You feel yourself yanked homewards!",
    "You feel yourself yanked away!",
    "The floor opens beneath you!",
    "You are thrown back in an explosion!"
]
cave_static = [
    "Non-existent glyph requested. Please report this bug.",
    "No stairs can be created here!", "The decoy is destroyed!"
]
for body, phrases in [(WORLD, world_static), (CAVE, cave_static)]:
    for phrase in phrases:
        assert 'angdroid_game_ui_text(' + json.dumps(phrase) in body, (
            "Untranslated source message: " + phrase)

for en, zh in [
    ("Your %s have recharged.", "你的%s已全部充能完畢。"),
    ("One of your %s has recharged.", "你的一件%s已充能完畢。"),
    ("The %s has recharged.", "%s已充能完畢。"),
    ("Your %s has recharged.", "你的%s已充能完畢。")
]:
    assert json.dumps(en) in WORLD
    assert json.dumps(zh, ensure_ascii=False) in WORLD
assert "if (obj->number > 1)" in WORLD
assert "if (all)" in WORLD
assert "} else if (obj->artifact) {" in WORLD
assert "recharged_notice(obj, true);" in WORLD
assert "recharged_notice(obj, false);" in WORLD
assert "player_change_place(player, player->home);" in WORLD
assert "player_change_place(player, target_place);" in WORLD
assert "effect_simple(EF_DESTRUCTION" in WORLD
assert "c->decoy = loc(0, 0);" in CAVE
assert 'lookup_trap("glyph of warding")' in CAVE
for code in [MON, WORLD, CAVE]:
    assert '#include "../angband/zh-tw-ui.h"' in code
    assert "angdroid_zh_tw_enabled()" in code

assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_fa_monster_world_locale.c" in RUNNER
assert "test-fa-monster-world-locale.sh" in VERIFY
assert "test_faangband_monster_world_messages_locale_source.py" in VERIFY
print("PASS: 19 translations, world events, monster actions and English fallback")
