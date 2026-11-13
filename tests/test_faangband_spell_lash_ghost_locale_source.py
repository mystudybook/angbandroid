#!/usr/bin/env python3
"""FAangband batch 251: all lash descriptors and safe ghost/unique names."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(filename):
    return (ROOT / filename).read_text(encoding="utf-8")

DATA = read("variant_lib/faangband/gamedata/projection.txt")
LASH = read("app/src/main/cpp/angband/zh-tw-mon-spell-lash.h")
MON_DESC = read("app/src/main/cpp/faangband/mon-desc.c")
SPELL = read("app/src/main/cpp/faangband/mon-spell.c")
LOCALE = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
NATIVE = read("tests/native/test_faangband_spell_lash_ghost_locale.c")
RUNNER = read("scripts/test-fa-spell-lash-ghost-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

source = list(dict.fromkeys(
    line.removeprefix("lash-desc:")
    for line in DATA.splitlines() if line.startswith("lash-desc:")
))
assert len(source) == 25
assert len(set(source)) == 25
pair_re = re.compile(
    r'^\s*\{\s*("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\s*\},',
    re.MULTILINE)
def pairs(s):
    return [(json.loads(a), json.loads(b)) for a, b in pair_re.findall(s)]

translations = pairs(LASH)
tests = pairs(NATIVE)
assert len(translations) == len(tests) == 25
assert {en for en, _ in translations} == set(source)
assert tests == [(en, dict(translations)[en]) for en in source]
assert len({en for en, _ in translations}) == 25
assert all(en and zh and "\ufffd" not in zh for en, zh in translations)

# Prove the game still reads both lash tokens from the projection data,
# not from untrusted spell text or a modified game-content field.
body = SPELL.split("static void spell_message(", 1)[1].split(
    "const struct monster_spell *monster_spell_by_index(", 1)[0]
assert '#include "../angband/zh-tw-mon-spell-lash.h"' in SPELL
assert '#include "mon-spell-message-locale.h"' in SPELL
assert '#include "mon-lore-terms.h"' not in SPELL
assert body.count("projections[type].lash_desc;") == 2
assert body.count("angdroid_mon_spell_lash(type_name, localized)") == 2
assert "case SPELL_TAG_TYPE:" in body
assert "case SPELL_TAG_OF_TYPE:" in body
assert 'localized ? "（" : " of "' in body
assert 'strnfcat(buf, sizeof(buf), &end, "）");' in body
assert "if (type_name) {" in body

# Real player ghosts derive their identity from the saved ghost name and
# race, not a guessed / translated monster. No extra visibility is exposed.
for token in [
    'rf_has(mon->race->flags, RF_PLAYER_GHOST)',
    'rf_has(t_mon->race->flags, RF_PLAYER_GHOST)',
    'monster_is_shape_unique(mon)',
    'monster_is_shape_unique(t_mon)',
    'angdroid_fa_spell_ghost_subject(',
    'cave->ghost->name',
    'mon->race->name',
    't_mon->race->name',
    'angdroid_fa_spell_subject(m_name,',
    'if (!localized && !strchr(punct, *in_cursor)) {',
    'msgt(spell->msgt, "%s", safe_buf);',
    'msgt(spell->msgt, "%s", buf);'
]:
    assert token in body, token
assert body.count('if (!localized && !strchr(punct, *in_cursor)) {') == 2
assert body.count('angdroid_fa_spell_ghost_subject(') == 2

for token in [
    'rf_has(mon->race->flags, RF_PLAYER_GHOST)',
    'cave->ghost->name',
    'format(", the %s", mon->race->name)',
    'monster_is_shape_unique(mon)',
    '" (offscreen)"'
]:
    assert token in MON_DESC, token

for token in [
    'static inline const char *angdroid_fa_spell_ghost_subject(',
    'if (!full || !ghost_name || !race_name || !zh_tw ||',
    'strncmp(full, ghost_name, name_len)',
    'strncmp(tail, ", the ", 6)',
    'strncmp(tail, race_name, race_len)',
    'strcmp(tail, " (offscreen)")',
    'angdroid_utf8_copy_complete(output, capacity, ghost_name)',
    'angdroid_utf8_append_complete(output, capacity, used, race_name)',
    'angdroid_utf8_append_complete(output, capacity, used, "（")',
    'angdroid_utf8_append_complete(output, capacity, used, "）")',
    '"（畫面外）"',
    'return full;'
]:
    assert token in LOCALE, token
assert '#include "../curses/unicode_display.h"' in LOCALE
assert 'snprintf(' not in LOCALE
assert '"test_faangband_spell_lash_ghost_locale.c"' in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-spell-lash-ghost-locale.sh" in VERIFY
assert "test_faangband_spell_lash_ghost_locale_source.py" in VERIFY

print("PASS: 25 source-exact lash descriptors, special ghosts, unique-name gates")
