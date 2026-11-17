#!/usr/bin/env python3
"""Guard FAangband fear percentage boundaries and unchanged normal RNG flow."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-util.c")
HELPER = read("app/src/main/cpp/faangband/mon-fear-health.h")
NATIVE = read("tests/native/test_fa_mon_fear_health.c")
RUNNER = read("scripts/test-fa-mon-fear-health.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

fear = SOURCE.split("static bool monster_scared_by_damage(", 1)[1].split(
    "bool mon_take_nonplayer_hit(", 1)[0]
assert '#include "mon-fear-health.h"' in SOURCE
assert "if (dam <= 0) return false;" in fear
assert fear.index("if (dam <= 0) return false;") < fear.index(
    "int tmp = randint1(dam);")
assert "if (current_fear) {" in fear
assert "} else if (monster_can_be_scared(mon)) {" in fear
assert "angdroid_fa_mon_fear_health_percentage(" in fear
assert "mon->hp, mon->maxhp, &percentage" in fear
assert fear.index("angdroid_fa_mon_fear_health_percentage(") < fear.index(
    "bool low_hp = randint1(10) >= percentage;")
assert "(100L * mon->hp) / mon->maxhp" not in fear
for fragment in (
    "bool low_hp = randint1(10) >= percentage;",
    "bool big_hit = (dam >= mon->hp) && (randint0(100) < 80);",
    "int time = randint1(10);",
    "if ((dam >= mon->hp) && (percentage > 7)) {",
    "time += 20;",
    "time += (11 - percentage) * 5;",
    "MON_TMD_FLG_NOMESSAGE | MON_TMD_FLG_NOFAIL",
):
    assert fragment in fear, fragment
assert "max_hp <= 0" in HELPER
assert "!percentage" in HELPER
assert "(100L * current_hp) / max_hp" in HELPER
assert "int16_t current_hp" in HELPER
assert "test_fa_mon_fear_health.c" in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-mon-fear-health.sh" in VERIFY
assert "test_faangband_mon_fear_health_source.py" in VERIFY
for fragment in (
    "INT16_MIN", "INT16_MAX", "percentage == 777",
    "for (hp = 0; hp <= INT16_MAX; hp += 131)",
):
    assert fragment in NATIVE, fragment

print("PASS: FAangband fear max HP and RNG-boundary source contract")
