#!/usr/bin/env python3
"""Guard the 57 FAangband melee action templates and their display-only locale."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
def source(path):
    return (ROOT / path).read_text(encoding="utf-8")

DATA = source("variant_lib/faangband/gamedata/blow_methods.txt")
BLOWS = source("app/src/main/cpp/faangband/mon-blows.c")
DICT = source("app/src/main/cpp/angband/zh-tw-ui.h")
TEST = source("tests/native/test_faangband_melee_actions_locale.c")
RUNNER = source("scripts/test-fa-melee-actions-locale.sh")
VERIFY = source("scripts/verify-zh-tw.sh")

pair_re = re.compile(
    r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
    re.MULTILINE)
def pairs(text):
    return [(json.loads(en), json.loads(zh))
            for en, zh in pair_re.findall(text)]

action_keys = [line[4:] for line in DATA.splitlines()
               if line.startswith("act:")]
assert len(action_keys) == 57
assert len(set(action_keys)) == 57
test_pairs = pairs(TEST)
dictionary = pairs(DICT)
assert len(test_pairs) == 59
assert [pair[0] for pair in test_pairs[:57]] == action_keys
assert [pair[0] for pair in test_pairs[57:]] == [
    "You stand your ground!", "You resist the effects!"]
assert len(set(key for key, _ in test_pairs)) == 59

# All source tags survive translation exactly; game data is not rewritten.
tag_re = re.compile(r'\{(?:target|oftarget|has)\}')
for en, zh in test_pairs:
    assert (en, zh) in dictionary, "Missing translation: " + en
    assert sum(item[0] == en for item in dictionary) == 1, (
        "Duplicate translation key: " + en)
    assert sorted(tag_re.findall(en)) == sorted(tag_re.findall(zh)), (
        "Changed source template tag: " + en)
    if en in action_keys:
        assert "{" not in tag_re.sub("", zh), "Unknown translated tag"
        assert "}" not in tag_re.sub("", zh), "Unknown translated tag"

assert "char *monster_blow_method_action(const struct blow_method *method, int midx)" in BLOWS
assert "return monster_blow_method_action_for_display(method, midx, false, NULL);" in BLOWS
assert "static char *monster_blow_method_action_for_display(" in BLOWS
assert "in_cursor = angdroid_game_ui_text(msg->act_msg, zh_tw);" in BLOWS
assert "bool translated = (in_cursor != msg->act_msg);" in BLOWS
assert "if (was_translated) *was_translated = translated;" in BLOWS

action_function = BLOWS.split(
    "static char *monster_blow_method_action_for_display(", 1)[1].split(
    "/* Public API remains English", 1)[0]
assert action_function.count("randint0(method->num_messages)") == 1
assert "msg = msg->next;" in action_function
assert 'translated ? "你" : "you"' in action_function
assert 'translated ? "你的" : "your"' in action_function
assert 'translated ? "有" : "has"' in action_function
assert 'translated ? "有" : "have"' in action_function
assert "strnfcat(buf, 1024, &end" in action_function
assert "return string_make(buf);" in action_function

for target in ["-1", "t_idx"]:
    assert ("monster_blow_method_action_for_display(method, " + target +
            ", true,") in BLOWS
    assert "monster_blow_method_action(method, " + target + ")" in BLOWS
assert 'translated ? "%s%s%s"' in BLOWS
assert 'translated ? "%s%s%s (%d)"' in BLOWS
assert '"%s %s%s"' in BLOWS
assert '"%s %s%s (%d)"' in BLOWS
for char in ['"。"','"！"','"？"']:
    assert 'suffix(act, ' + char + ')' in BLOWS
assert "string_free(act);" in BLOWS
assert "msgt(method->msgt" in BLOWS

assert 'angdroid_game_ui_text(save_msg,' in BLOWS
assert 'OF_PROT_FEAR, true, "You stand your ground!"' in BLOWS
assert 'OF_FREE_ACT, true, "You resist the effects!"' in BLOWS
assert "player_inc_timed(context->p, type, amount" in BLOWS
assert "monster_damage_target(context, false)" in BLOWS
assert '#include "../angband/zh-tw-ui.h"' in BLOWS
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_faangband_melee_actions_locale.c" in RUNNER
assert "test-fa-melee-actions-locale.sh" in VERIFY
assert "test_faangband_melee_actions_locale_source.py" in VERIFY
print("PASS: all 57 FAangband melee actions and 2 save messages are localized")
