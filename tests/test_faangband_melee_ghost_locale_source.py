#!/usr/bin/env python3
"""Source contracts for FAangband melee effect text and 14 ghost challenges."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
BLOWS = (ROOT / "app/src/main/cpp/faangband/mon-blows.c").read_text(
    encoding="utf-8")
WORLD = (ROOT / "app/src/main/cpp/faangband/game-world.c").read_text(
    encoding="utf-8")
DICT = (ROOT / "app/src/main/cpp/angband/zh-tw-ui.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_faangband_melee_ghost_locale.c").read_text(
    encoding="utf-8")
SCRIPT = (ROOT / "scripts/test-fa-melee-ghost-locale.sh").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

pair_re = re.compile(r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
                     re.MULTILINE)
def pairs(text):
    return [(json.loads(a), json.loads(b)) for a, b in pair_re.findall(text)]

native_pairs = pairs(NATIVE)
dictionary_pairs = pairs(DICT)
assert len(native_pairs) == 27
assert len(set(key for key, _ in native_pairs)) == 27
for key, value in native_pairs:
    assert (key, value) in dictionary_pairs, "Missing translation: " + key
    assert sum(item[0] == key for item in dictionary_pairs) == 1, (
        "Duplicate production key: " + key)

ghost_match = re.search(
    r'static const char \*challenge_text\[14\] = \{(.*?)\};',
    WORLD, re.DOTALL)
assert ghost_match, "Ghost challenge source array missing"
ghost_keys = [json.loads(item) for item in re.findall(
    r'"(?:\\.|[^"\\])*"', ghost_match.group(1))]
assert len(ghost_keys) == 14
assert len(set(ghost_keys)) == 14
assert [key for key, _ in native_pairs[:14]] == ghost_keys
assert 'challenge_text[randint0(14)]' in WORLD
render = WORLD.split("static void ghost_challenge(void)", 1)[1].split(
    "static void decrease_timeouts(void)", 1)[0]
assert render.count("randint0(14)") == 1
assert 'angdroid_game_ui_text(challenge, true)' in render
assert 'msg("%s（%s）%s"' in render
assert 'msg("%s, the %s %s"' in render
assert "cave->ghost->name" in render
assert "race->name" in render

for key, _ in native_pairs[14:]:
    assert json.dumps(key) in BLOWS, "Missing original melee text: " + key
    assert "angdroid_game_ui_text(" + json.dumps(key) in BLOWS
for fmt_en, fmt_zh in [
    ("You take %d damage.", "你受到了 %d 點傷害。"),
    ("%s tries to steal %s %s, but fails.", "%s想偷走你的%s，但失敗了。"),
    ("%s %s (%c) was stolen!", "你的%s（%c）被偷走了！"),
    ("%d coins were stolen!", "%d 枚錢幣被偷走了！"),
    ("Your %s (%c) was eaten!", "你的%s（%c）被吃掉了！"),
    ("One of your %s (%c) was eaten!", "你的一件%s（%c）被吃掉了！"),
]:
    assert json.dumps(fmt_en) in BLOWS
    assert json.dumps(fmt_zh, ensure_ascii=False) in BLOWS
assert '"%s想偷走你的一件%s，但失敗了。"' in BLOWS
assert '"你的一件%s（%c）被偷走了！"' in BLOWS
assert "monster_blow_method_action(method, -1)" in BLOWS
assert 'msgt(method->msgt, "%s %s%s"' in BLOWS
assert "player_exp_lose(context->p, d / 10, false);" in BLOWS
assert "player_exp_lose(context->p, d, false);" in BLOWS
assert "monster_carry(cave, context->mon, stolen)" in BLOWS
assert "gear_to_label(context->p, obj)" in BLOWS
assert 'gear_object_for_use(context->p, obj, 1, false,' in BLOWS
assert '#include "../angband/zh-tw-ui.h"' in BLOWS
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in SCRIPT
assert "test_faangband_melee_ghost_locale.c" in SCRIPT
assert "test-fa-melee-ghost-locale.sh" in VERIFY
assert "test_faangband_melee_ghost_locale_source.py" in VERIFY
print("PASS: 27 melee/ghost keys, original formats and single ghost RNG draw")
