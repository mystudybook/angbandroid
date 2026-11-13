#!/usr/bin/env python3
"""FAangband race spell overrides: exact source keys and safe UTF-8 display."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

DATA = read("variant_lib/faangband/gamedata/monster.txt")
SPELL_FLAGS = read("app/src/main/cpp/faangband/list-mon-spells.h")
INIT = read("app/src/main/cpp/faangband/mon-init.c")
SPELL = read("app/src/main/cpp/faangband/mon-spell.c")
LOCALE = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
ALT = read("app/src/main/cpp/faangband/mon-spell-messages-alternate-zh-tw.h")
INC = read("app/src/main/cpp/angband/zh-tw-mon-spell-alt.inc")
C99 = read("tests/native/test_faangband_spell_alternates_utf8.c")
RUNNER = read("scripts/test-fa-spell-alternates-utf8.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

# Monster-specific messages are spell-index overrides, not global defaults.
records = re.findall(
    r'^(message-(?:vis|invis|miss)):([^:\n]+):([^\n]*)$',
    DATA, re.MULTILINE)
assert len(records) == 49
assert [sum(kind == k for kind, _, _ in records)
        for k in ("message-vis", "message-invis", "message-miss")] == [
            3, 46, 0]
assert not any(not msg.strip() for _, _, msg in records)
keys = list(dict.fromkeys(msg for _, _, msg in records))
assert len(keys) == 17, keys
assert len(set(keys)) == 17
assert all(spell.isupper() for _, spell, _ in records)
spell_flags = set(re.findall(r'^RSF\((\w+),', SPELL_FLAGS, re.MULTILINE))
assert all(spell in spell_flags for _, spell, _ in records)

# Reuse the existing Angband localization data rather than maintaining
# a different translation for identical FAangband source messages.
pair_re = re.compile(
    r'^\s*\{\s*("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\s*\},',
    re.MULTILINE)
def pairs(src):
    return [(json.loads(a), json.loads(b))
            for a, b in pair_re.findall(src)]

translations = pairs(INC)
assert len(translations) == 17
assert [en for en, _ in translations] == keys
assert len({en for en, _ in translations}) == 17
assert all(zh and '\ufffd' not in zh for _, zh in translations)
native_cases = pairs(C99)
assert native_cases == translations
def tags(s):
    return sorted(re.findall(r'\{([a-z]+)\}', s))
for en, zh in translations:
    assert tags(en) == tags(zh), en
    assert len(zh.encode("utf-8")) < 1024

# Preserve original parser priority (alternate first) and empty-string
# suppression, then do exact localization before expansion of tags.
for token in [
    'add_alternate_spell_message(r, s_idx, MON_ALTMSG_SEEN, msg)',
    'add_alternate_spell_message(r, s_idx, MON_ALTMSG_UNSEEN, msg)',
    'add_alternate_spell_message(r, s_idx, MON_ALTMSG_MISS, msg)',
    'alt->message = string_make("")',
    'parser_reg(p, "message-vis sym spell ?str message",',
    'parser_reg(p, "message-invis sym spell ?str message",',
    'parser_reg(p, "message-miss sym spell ?str message",',
]:
    assert token in INIT, token
assert '#include "../angband/zh-tw-mon-spell-alt.inc"' in ALT
assert '#include "mon-spell-messages-alternate-zh-tw.h"' in LOCALE
assert "angdroid_fa_spell_message_alternate(english, true)" in LOCALE
assert 'if (!english || !zh_tw) return english;' in ALT
assert 'if (strcmp(messages[i].en, english) == 0)' in ALT
assert 'return english;' in ALT

message = SPELL.split("static void spell_message(", 1)[1].split(
    "const struct monster_spell *monster_spell_by_index(", 1)[0]
for token in [
    'find_alternate_spell_message(mon->race,',
    'MON_ALTMSG_UNSEEN',
    'MON_ALTMSG_MISS',
    'MON_ALTMSG_SEEN',
    'level->blind_message',
    'level->miss_message',
    'level->message',
    "if (t_mon) {",
    "if (in_cursor[0] == '\\0') {",
    "const char *selected = in_cursor;",
    "angdroid_fa_spell_message_locale(selected,",
    "localized = (in_cursor != selected);",
    "next = strchr(in_cursor, '{');",
    "switch (spell_tag_lookup(tag))",
    "msgt(spell->msgt, \"%s\", safe_buf);",
    "msgt(spell->msgt, \"%s\", buf);",
]:
    assert token in message, token
assert message.index('MON_ALTMSG_UNSEEN') < message.index(
    "const char *selected = in_cursor;")
assert message.index('MON_ALTMSG_MISS') < message.index(
    "const char *selected = in_cursor;")
assert message.index('MON_ALTMSG_SEEN') < message.index(
    "const char *selected = in_cursor;")
assert message.index("const char *selected = in_cursor;") < (
    message.index("next = strchr(in_cursor, '{');"))
assert message.count(
    "if (!localized && !strchr(punct, *in_cursor)) {") == 2

# Long Chinese messages must never send a split UTF-8 scalar to msgt.
assert "char buf[1024]" in message
assert "char safe_buf[sizeof(buf)];" in message
assert "angdroid_utf8_copy_complete(safe_buf, sizeof(safe_buf), buf);" in message
assert "if (localized) {" in message
assert "char m_name[256];" in message
assert "char m_poss[256];" in message
assert "rf_has(mon->race->flags, RF_UNIQUE)" in message
assert "rf_has(mon->race->flags, RF_PLAYER_GHOST)" in message
assert '#include "../curses/unicode_display.h"' in LOCALE
assert 'angdroid_utf8_copy_complete(output, capacity, "這隻")' in LOCALE
assert "angdroid_utf8_append_complete(output, capacity, used, noun)" in LOCALE
assert 'angdroid_utf8_copy_complete(output, capacity, "某個東西")' in LOCALE
assert 'angdroid_utf8_copy_complete(output, capacity, "牠")' in LOCALE
assert 'strnfmt(' not in LOCALE
assert 'snprintf(' not in LOCALE

assert "test_faangband_spell_alternates_utf8.c" in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-spell-alternates-utf8.sh" in VERIFY
assert "test_faangband_spell_alternates_utf8_source.py" in VERIFY
print("PASS: 49 source alternates, 17 translations, strict visibility and UTF-8")
