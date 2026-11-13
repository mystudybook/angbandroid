#!/usr/bin/env python3
"""Source contracts for FAangband monster lore and Chinese grammar."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SRC = read("app/src/main/cpp/faangband/mon-lore.c")
RACE = read("app/src/main/cpp/faangband/list-mon-race-flags.h")
DICT = read("app/src/main/cpp/angband/zh-tw-ui.h")
NATIVE = read("tests/native/test_faangband_lore_display_locale.c")
RUNNER = read("scripts/test-fa-lore-display-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

pair_re = re.compile(
    r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
    re.MULTILINE)
def pairs(text):
    return [(json.loads(a), json.loads(b))
            for a, b in pair_re.findall(text)]

full_pairs = pairs(DICT)
tests = pairs(NATIVE)
assert len(tests) == 30
assert len({k for k, _ in tests}) == 30
awareness = SRC.split("static const char *lore_describe_awareness(", 1)[1].split(
    "static const char *lore_describe_speed(", 1)[0]
speed = SRC.split("static const char *lore_describe_speed(", 1)[1].split(
    "static void lore_adjective_speed(", 1)[0]
aware_keys = re.findall(r'\{\s*\d+,\s*"([^"]+)"\s*\}', awareness)
speed_keys = re.findall(r'\{\s*\d+,\s*"([^"]+)"\s*\}', speed)
race_keys = re.findall(
    r'^RF\([^,]+,\s*RFT_RACE_[AN],\s*"([^"]+)"\)',
    RACE, re.MULTILINE)
assert len(aware_keys) == 10
assert len(speed_keys) == 8
assert len(race_keys) == 10
expected = (aware_keys + ["is ever vigilant for"] +
            speed_keys + ["erroneously"] + race_keys)
assert [en for en, _ in tests] == expected
assert len(set(expected)) == 30
for en, zh in tests:
    assert (en, zh) in full_pairs, "Missing live dict key: " + en
    assert sum(actual[0] == en for actual in full_pairs) == 1
    assert zh and "\ufffd" not in zh

def function(name):
    return SRC.split(name + "(", 1)[1].split("\n/**", 1)[0]

kills = function("lore_append_kills")
toughness = function("lore_append_toughness")
exp = function("lore_append_exp")
drops = function("lore_append_drop")
movement = function("lore_append_movement")
aware = function("lore_append_awareness")
friends = function("lore_append_friends")
adjective = function("lore_adjective_speed")
multiplier = function("lore_multiplier_speed")

# Preserve known-info gating and exact gameplay formula inputs. Rendering only.
assert '#include "../angband/zh-tw-ui.h"' in SRC
assert "rf_has(known_flags, RF_UNIQUE)" in kills
assert "race->max_num == 0" in kills
for name in ["lore->deaths", "lore->pkills", "lore->tkills"]:
    assert name in kills
assert 'textblock_append(tb, "\\n");' in kills
assert "lore->armour_known && angdroid_zh_tw_enabled()" in toughness
for term in ["race->avg_hp", "race->ac",
             "hit_chance(&c, chance_of_melee_hit_base(player, weapon), race->ac);",
             "random_chance_scaled(c, 100)"]:
    assert term in toughness
assert "if (!race->rarity) return;" in exp
for term in ["race->mexp * race->level / player->lev",
             "race->mexp * race->level % player->lev",
             "exp_fraction", 'textblock_append_c(tb, COLOUR_BLUE, "%s 點經驗值", buf);']:
    assert term in exp
assert "if (!lore->drop_known) return;" in drops
assert "mon_create_drop_count(race, true, false, &nspec)" in drops
for flag in ["RF_DROP_GREAT", "RF_DROP_GOOD", "RF_ONLY_ITEM", "RF_ONLY_GOLD"]:
    assert flag in drops
for term in ["n == 1", "n == 2", "nspec == 1", "nspec == 2",
             "牠可能攜帶", "特定物品"]:
    assert term in drops

assert "lore->sleep_known" in aware
assert "lore_describe_awareness(race->sleep)" in aware
assert "angdroid_game_ui_text(aware, true)" in aware
assert "10 * race->hearing" in aware
assert "race->friends || race->friends_base" in friends
assert "rf_has(known_flags, RF_GROUP_AI)" in friends
assert "這隻" in movement
assert "create_mon_flag_mask(flags, RFT_RACE_A, RFT_MAX);" in movement
assert "create_mon_flag_mask(flags, RFT_RACE_N, RFT_MAX);" in movement
assert "angdroid_game_ui_text(describe_race_flag(f), true)" in movement
for term in ["race->level == 0", "RF_FORCE_DEPTH",
             "flags_test(known_flags, RF_SIZE, RF_RAND_50, RF_RAND_25",
             "RF_NEVER_MOVE", "OPT(player, effective_speed)",
             "lore_adjective_speed(tb, race);",
             "lore_multiplier_speed(tb, race);"]:
    assert term in movement
assert "angdroid_game_ui_text(lore_describe_speed(race->speed), true)" in adjective
assert "race->speed == 110" in adjective
assert "extract_energy[race->speed]" in multiplier
assert "extract_energy[player->state.speed]" in multiplier
assert "player->state.speed > race->speed" in multiplier
assert "player->state.speed < race->speed" in multiplier
assert "zh_tw ? \"以正常速度的 \" : \"at \"" in multiplier
assert "zh_tw ? \"你的速度\" : \"the same as you\"" in multiplier

for fn in [kills, toughness, exp, drops, movement, aware, friends]:
    assert "angdroid_zh_tw_enabled()" in fn
    assert "return;" in fn
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_faangband_lore_display_locale.c" in RUNNER
assert "test-fa-lore-display-locale.sh" in VERIFY
assert "test_faangband_lore_display_locale_source.py" in VERIFY
print("PASS: 30 source-derived labels and 7 monster lore display branches")
