#!/usr/bin/env python3
"""FAangband source-exact battle spell notifications and visibility gates."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

DATA = read("variant_lib/faangband/gamedata/monster_spell.txt")
SOURCE = read("app/src/main/cpp/faangband/mon-spell.c")
VISIBLE = read("app/src/main/cpp/faangband/mon-spell-messages-visible-zh-tw.h")
OTHER = read("app/src/main/cpp/faangband/mon-spell-messages-other-zh-tw.h")
WRAPPER = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
NATIVE = read("tests/native/test_faangband_spell_messages_locale.c")
SHELL = read("scripts/test-fa-spell-messages-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

def unique_lines(prefix):
    return list(dict.fromkeys(
        line[len(prefix):] for line in DATA.splitlines()
        if line.startswith(prefix)
    ))

visible = unique_lines("message-vis:")
unseen = unique_lines("message-invis:")
miss = unique_lines("message-miss:")
saved = unique_lines("message-save:")
assert (len(visible), len(unseen), len(miss), len(saved)) == (
    116, 80, 11, 14)
all_keys = visible + unseen + miss + saved
assert len(all_keys) == 221
assert len(set(all_keys)) == 221

# The parser concatenates multiple messages under one spell power cutoff.
# An exact source-line lookup is safe only while each field appears at
# most once within a level. Fail closed if game-data becomes concatenated.
message_fields = ("message-vis:", "message-invis:",
                  "message-miss:", "message-save:")
counts = {}
for line in DATA.splitlines():
    if line.startswith("name:") or line.startswith("power-cutoff:"):
        assert all(n <= 1 for n in counts.values()), counts
        counts = {}
    for field in message_fields:
        if line.startswith(field):
            counts[field] = counts.get(field, 0) + 1
assert all(n <= 1 for n in counts.values())

pair_re = re.compile(
    r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
    re.MULTILINE)

def pairs(text):
    return [(json.loads(en), json.loads(zh))
            for en, zh in pair_re.findall(text)]

vis_pairs = pairs(VISIBLE)
other_pairs = pairs(OTHER)
cases = pairs(NATIVE)
assert len(vis_pairs) == 116
assert len(other_pairs) == 105
assert [en for en, _ in vis_pairs] == visible
assert [en for en, _ in other_pairs] == unseen + miss + saved
assert cases == vis_pairs + other_pairs
assert len(cases) == 221

def tokens(s):
    return sorted(re.findall(r'\{([a-z]+)\}', s))

for en, zh in cases:
    assert en and zh and "\ufffd" not in zh
    assert tokens(en) == tokens(zh), (en, zh)
    assert len(zh.encode("utf-8")) < 1024, en
assert set(tok for en, _ in cases for tok in tokens(en)) == {
    "name", "pronoun", "target", "type", "oftype"
}

# Check the actual runtime selection and fallback, not merely a dictionary.
spell_message = SOURCE.split("static void spell_message(", 1)[1].split(
    "const struct monster_spell *monster_spell_by_index(", 1)[0]
save_process = SOURCE.split("void do_mon_spell(", 1)[1].split(
    "/* Types of monster spells", 1)[0]
assert '#include "mon-spell-message-locale.h"' in SOURCE
assert '#include "mon-lore-terms.h"' in SOURCE
assert '#include "../angband/zh-tw-ui.h"' in SOURCE
for token in [
    "if (!seen) {", "if (t_mon) {", "} else if (!hits) {",
    "MON_ALTMSG_UNSEEN", "MON_ALTMSG_MISS", "MON_ALTMSG_SEEN",
    "level->blind_message", "level->miss_message", "level->message",
    "else if (in_cursor[0] == '\\0') {",
    "const char *selected = in_cursor;",
    "angdroid_fa_spell_message_locale(selected,",
    "localized = (in_cursor != selected);",
    "next = strchr(in_cursor, '{');",
    "monster_desc(m_name, sizeof(m_name),",
    "monster_desc(m_poss, sizeof(m_poss), mon, MDESC_PRO_VIS | MDESC_POSS)",
    "angdroid_fa_spell_subject(m_name,",
    "rf_has(mon->race->flags, RF_UNIQUE)",
    "rf_has(mon->race->flags, RF_PLAYER_GHOST)",
    "rf_has(t_mon->race->flags, RF_UNIQUE)",
    "rf_has(t_mon->race->flags, RF_PLAYER_GHOST)",
    'localized ? "牠的" : m_poss',
    'localized ? "你" : "you"',
    "angdroid_fa_lore_term(type_name, localized)",
    "msgt(spell->msgt, \"%s\", buf);"
]:
    assert token in spell_message, token

assert spell_message.index("if (!seen) {") < spell_message.index(
    "const char *selected = in_cursor;")
assert spell_message.index("} else if (!hits) {") < spell_message.index(
    "const char *selected = in_cursor;")
assert spell_message.index("const char *selected = in_cursor;") < (
    spell_message.index("next = strchr(in_cursor, '{');"))
assert spell_message.index("next = strchr(in_cursor, '{');") < (
    spell_message.index("switch (spell_tag_lookup(tag))"))
assert "if (!english || !zh_tw) return english;" in WRAPPER
assert "if (translated != english) return translated;" in WRAPPER
assert "return angdroid_fa_spell_message_other(english, true);" in WRAPPER
assert "if (!english || !localized || !output || !capacity)" in WRAPPER
assert 'snprintf(output, capacity, "這隻%s", noun)' in WRAPPER
assert "return english;" in WRAPPER

# No change in saving throw, rune or effect processing.
for token in [
    "if (spell->hit == 100)",
    "if (spell->hit == 0)",
    "disturb(player);", "spell_message(mon, spell, seen, hits);",
    "if (hits) {", "level->save_message && (target_midx <= 0)",
    "randint0(100) < player->state.skills[SKILL_SAVE]",
    "angdroid_fa_spell_message_locale(",
    "spell_check_for_fail_rune(spell);",
    "effect_do(spell->effect, source_monster(mon->midx)"
]:
    assert token in save_process, token
assert "msg(\"%s\", angdroid_fa_spell_message_locale(" in save_process

assert "test_faangband_spell_messages_locale.c" in SHELL
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in SHELL
assert "test-fa-spell-messages-locale.sh" in VERIFY
assert "test_faangband_spell_messages_locale_source.py" in VERIFY
print("PASS: 116 visible, 80 unseen, 11 misses, 14 saves, 221 templates")
