#!/usr/bin/env python3
"""FAangband batch 257: source-complete melee actions and safe noun grammar."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

DATA = read("variant_lib/faangband/gamedata/blow_methods.txt")
UI = read("app/src/main/cpp/angband/zh-tw-ui.h")
BLOWS = read("app/src/main/cpp/faangband/mon-blows.c")
LOCALE = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
NATIVE = read("tests/native/test_faangband_blow_action_subject_locale.c")
RUNNER = read("scripts/test-fa-blow-action-subject-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

keys = list(dict.fromkeys(
    line[len("act:"):] for line in DATA.splitlines()
    if line.startswith("act:")
))
assert len(keys) == 57
assert len(set(keys)) == 57

pairs_re = re.compile(
    r'^\s*\{\s*("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\s*\},',
    re.MULTILINE)
def pairs(s):
    return [(json.loads(en), json.loads(zh)) for en, zh in pairs_re.findall(s)]

translations = dict(pairs(UI))
assert all(key in translations for key in keys)
expected = [(key, translations[key]) for key in keys]
assert pairs(NATIVE) == expected

tags = re.compile(r'\{([a-z]+)\}')
budgets = []
for en, zh in expected:
    original_tags = sorted(tags.findall(en))
    localized_tags = sorted(tags.findall(zh))
    assert original_tags == localized_tags, (en, zh)
    assert set(localized_tags).issubset({"target", "oftarget", "has"})
    assert len(localized_tags) <= 1
    assert len(zh.encode("utf-8")) < 1024
    max_bytes = len(tags.sub("", zh).encode("utf-8")) + 320 * len(localized_tags)
    budgets.append(max_bytes)
    assert max_bytes + 320 + 3 + 12 < 1024, (en, max_bytes)
assert max(budgets) == 344
assert ("insults {oftarget} mother!",
        "辱罵了{oftarget}母親！") in expected

assert '#include "mon-predicate.h"' in BLOWS
assert '#include "mon-spell-message-locale.h"' in BLOWS
assert '#include "../angband/zh-tw-ui.h"' in BLOWS

helper = BLOWS.split(
    "static const char *monster_blow_display_subject(", 1)[1].split(
    "typedef enum {", 1)[0]
action = BLOWS.split(
    "static char *monster_blow_method_action_for_display(", 1)[1].split(
    "char *monster_blow_method_action(", 1)[0]
player = BLOWS.split(
    "static void display_blow_message_vs_player(", 1)[1].split(
    "static void display_blow_message_vs_monster(", 1)[0]
monster = BLOWS.split(
    "static void display_blow_message_vs_monster(", 1)[1].split(
    "int blow_index(", 1)[0]

for token in (
    "monster_desc(original, 256, mon, mode & ~MDESC_COMMA)",
    "rf_has(mon->race->flags, RF_PLAYER_GHOST) && cave->ghost",
    "angdroid_fa_spell_ghost_subject(original,",
    "monster_is_shape_unique(mon)", "!monster_is_visible(mon)",
    "angdroid_fa_spell_subject(original, true,",
    "angdroid_fa_spell_offscreen_subject(shown, true,",
):
    assert token in helper, token

# One single random draw per blow, before translation; public English API
# always returns the original action regardless of GUI language.
assert action.count("randint0(method->num_messages)") == 1
assert "angdroid_game_ui_text(msg->act_msg, zh_tw)" in action
assert "bool translated = (in_cursor != msg->act_msg);" in action
assert "if (was_translated) *was_translated = translated;" in action
assert "monster_blow_method_action_for_display(method, midx, false, NULL)" in BLOWS
assert "switch (blow_tag_lookup(tag))" in action

target = action.split("case BLOW_TAG_TARGET:", 1)[1].split(
    "case BLOW_TAG_OF_TARGET:", 1)[0]
poss = action.split("case BLOW_TAG_OF_TARGET:", 1)[1].split(
    "case BLOW_TAG_HAS:", 1)[0]
for section in (target, poss):
    assert "if (midx > 0 && translated)" in section
    assert "monster_blow_display_subject(" in section
    assert "t_mon, MDESC_TARG," in section
    assert "else if (midx > 0)" in section
    assert "monster_desc(m_name," in section
assert "if (!strchr(punct, *in_cursor))" in target
assert "mdesc_mode |= MDESC_COMMA" in target
assert 'translated ? "你" : "you"' in target
assert "MDESC_TARG | MDESC_POSS" in poss
assert 'strnfcat(buf, sizeof(buf), &end, "%s的", shown);' in poss
assert 'translated ? "你的" : "your"' in poss
assert 'translated ? "有" : "has"' in action
assert 'translated ? "有" : "have"' in action

for section in (player, monster):
    assert "const struct monster *mon, const char *m_name" in section
    assert "const char *shown = m_name;" in section
    assert "if (translated)" in section
    assert "monster_blow_display_subject(mon, MDESC_STANDARD," in section
    assert "msgt(method->msgt," in section
    assert "string_free(act);" in section
assert 'translated ? "%s%s%s（%d）" :' in player
assert '"%s %s%s (%d)"' in player
assert "OPT(p, show_damage)" in player
assert 'translated ? "%s%s%s" :' in monster
assert '"%s %s%s"' in monster
assert BLOWS.count("context->mon, context->m_name,") == 8
assert BLOWS.count("display_blow_message_vs_player(context->method,") == 4
assert BLOWS.count("display_blow_message_vs_monster(context->method,") == 4

# Original hit/side-effect APIs still operate on unmodified melee contexts.
for token in (
    "take_hit(context->p, reduced, context->ddesc)",
    "mon_take_nonplayer_hit(context->damage, context->t_mon,",
    "randcalc(dice, rlev, RANDOMISE)", # in mon-attack.c, not here
    "context->obvious = true;",
    "effect_simple(",
):
    if token == "randcalc(dice, rlev, RANDOMISE)":
        continue
    assert token in BLOWS, token

for token in (
    "angdroid_fa_spell_subject(", "angdroid_fa_spell_ghost_subject(",
    "angdroid_fa_spell_offscreen_subject(",
):
    assert token in LOCALE
assert "test_faangband_blow_action_subject_locale.c" in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-blow-action-subject-locale.sh" in VERIFY
assert "test_faangband_blow_action_subject_locale_source.py" in VERIFY
print("PASS: 57/57 blow actions, 1 tag max, 344-byte action budget")
