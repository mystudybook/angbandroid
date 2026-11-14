#!/usr/bin/env python3
"""Batch 258: complete Chinese monster-event glyphs and grouping metadata.

This verifies the production source gates. It is not a simulated game test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-msg.c")
HELPER = read("app/src/main/cpp/faangband/mon-msg-utf8.h")
NATIVE = read("tests/native/test_faangband_mon_msg_utf8.c")
RUNNER = read("scripts/test-fa-mon-msg-utf8.sh")
VERIFY = read("scripts/verify-zh-tw.sh")
REPOSITORY = read("app/src/main/cpp/faangband/list-mon-message.h")
PAIN_DATA = read("variant_lib/faangband/gamedata/pain.txt")
DICT = read("app/src/main/cpp/angband/zh-tw-ui.h")
FORMATTER = read("app/src/main/cpp/faangband/message.c")

stack = SOURCE.split("static bool stack_message(", 1)[1].split(
    "static int what_delay(", 1)[0]
regular = SOURCE.split("bool add_monster_message(", 1)[1].split(
    "bool add_monster_message_show_damage(", 1)[0]
damage = SOURCE.split("bool add_monster_message_show_damage(", 1)[1].split(
    "static void get_subject(", 1)[0]
subject = SOURCE.split("static void get_subject(", 1)[1].split(
    "/* State machine constants", 1)[0]
parser = SOURCE.split("static bool get_message_text(", 1)[1].split(
    "#undef MSG_PARSE_NORMAL", 1)[0]
show = SOURCE.split("static void show_message(", 1)[1].split(
    "void show_monster_messages(", 1)[0]

# p_race is part of the existing stack identity. It must be assigned in
# both fresh-entry constructors, including damage notifications.
for key in (
    "mon_msg[i].p_race == mon->player_race",
    "mon_msg[i].race == mon->race",
    "mon_msg[i].flags == flags",
    "mon_msg[i].msg_code == msg_code",
    "mon_msg[i].count++",
    "mon_msg[i].damage += damage",
):
    assert key in stack, key
for part in (regular, damage):
    assert "mon_msg[size_mon_msg].race = mon->race;" in part
    assert "mon_msg[size_mon_msg].p_race = mon->player_race;" in part
    assert "mon_msg[size_mon_msg].flags = flags;" in part
    assert "mon_msg[size_mon_msg].msg_code = msg_code;" in part
    assert "mon_msg[size_mon_msg].count = 1;" in part
    assert "store_monster(mon, msg_code);" in part
    assert "player->upkeep->notice |= PN_MON_MESSAGE;" in part
assert "MON_MSG_FLAG_DAMAGE" in damage

# Localized display uses exact source-selected names, not independent
# monster race lookups that could reveal unseen or off-panel identities.
assert '#include "mon-msg-utf8.h"' in SOURCE
assert "angdroid_fa_mon_msg_chinese_subject(buf, buflen," in subject
assert "race->name, race->plural, p_race ? p_race->name : NULL," in subject
for token in (
    "rf_has(race->flags, RF_PLAYER_GHOST) && cave->ghost",
    "rf_has(race->flags, RF_UNIQUE)",
    "rf_has(race->flags, RF_NAME_COMMA)",
    "count, invisible, offscreen,",
    'my_strcpy(buf, "It", buflen);',
    'strnfmt(buf, buflen, "%d monsters", count);',
    'strnfmt(buf, buflen, "The %s%s", p_name, race->name);',
    'my_strcat(buf, " (offscreen)", buflen);',
    'my_strcat(buf, " ", buflen);',
):
    assert token in subject, token
assert "if (localized) {" in subject
assert subject.index("angdroid_fa_mon_msg_chinese_subject") < (
    subject.index('my_strcpy(buf, "It", buflen);'))

# Complete Chinese scalars in a clipped body and always parse until the
# end of the singular/plural brackets; the original message code is intact.
for token in (
    "case MON_MSG_95:", "case MON_MSG_75:", "case MON_MSG_50:",
    "case MON_MSG_35:", "case MON_MSG_20:", "case MON_MSG_10:",
    "case MON_MSG_0:",
    "angdroid_game_ui_text(source,",
    "bool translated = (localized != source);",
    "source = localized;",
    "for (size_t i = 0; i < maxlen; i++)",
    "bool writable = true;",
    "if (!writable) continue;",
    "if (translated) {",
    "angdroid_utf8_next_scalar(source + i, &cp);",
    "if (n > buflen - pos - 1)",
    "memcpy(buf + pos, source + i, n);",
    "i += n - 1;",
    "buf[pos++] = '?';",
    "buf[pos++] = cur;",
    "assert(state == MSG_PARSE_NORMAL);",
    "buf[pos] = '\\0';",
    "return translated;",
):
    assert token in parser, token
assert "i < maxlen && pos < buflen - 1" not in parser
assert parser.index("angdroid_game_ui_text(source,") < parser.index(
    "for (size_t i = 0; i < maxlen; i++)")
assert parser.index("assert(state == MSG_PARSE_NORMAL);") > parser.index(
    "for (size_t i = 0; i < maxlen; i++)")

# New UTF-8 subject helper must reserve the full display suffix before
# copying long names and must not create half of a ghost identity.
for token in (
    'static const char suffix[] = "（畫面外）";',
    'angdroid_utf8_copy_complete(dst, base_capacity, "牠")',
    'snprintf(number, sizeof(number), "%d 隻怪物", count)',
    'snprintf(number, sizeof(number), "%d 隻", count)',
    "suffix_fits = offscreen && capacity > sizeof(suffix);",
    "if (suffix_fits) reserved_bytes = sizeof(suffix) - 1;",
    "base_capacity = capacity - reserved_bytes;",
    'if (comma_fits)',
    'reserved_bytes += sizeof("，") - 1;',
    '"angdroid_utf8_append_complete(dst, capacity, used, suffix)"',
    "if (invisible)",
    "else if (ghost_name)",
    "else if (unique)",
    "if (name_bytes < base_capacity", # boundary gate can be combined
    "angdroid_utf8_append_complete(",
    "comma_fits = !invisible && comma;",
    'angdroid_utf8_append_complete(dst, capacity, used, suffix);',
    'angdroid_utf8_append_complete(dst, capacity, used, "，");',
    "const char *visible_race =",
    "if (qualifier_len < remaining &&",
    "race_len <= remaining - qualifier_len - 1",
    "count == 1 || !plural ? race_name : plural",
):
    if token == "if (name_bytes < base_capacity":
        assert "name_bytes < base_capacity" in HELPER
    else:
        assert token in HELPER, token

# Damage grouping, sound category, singular punctuation, and averaging
# arithmetic remain the original calculations.
assert 'char subject[256] = "";' in show
assert "char body[256];" in show
assert "get_message_type(msg->msg_code, msg->race)" in show
assert "get_message_text(body, sizeof(body)," in show
assert "get_subject(subject, sizeof(subject)," in show
assert "msg->p_race," in show
assert "msg->count > 1" in show
assert "skip_subject(msg->msg_code)" in show
assert "MON_MSG_FLAG_INVISIBLE" in show
assert "MON_MSG_FLAG_OFFSCREEN" in show
assert "MON_MSG_FLAG_DAMAGE" in show
assert 'localized ? "%s%s（%d）" :' in show
assert '"%s%s (%d)"' in show
assert 'localized ? "%s%s（平均 %d）" :' in show
assert '"%s%s (average %d)"' in show
assert "msg->damage / msg->count" in show
assert "msg->damage % msg->count" in show
assert 'msgt(msg_type, "%s%s", subject, body);' in show

for token in (
    "這隻獸人", "3 隻獸人們", "這隻半精靈 獸人",
    "這隻獸人（畫面外），", "這隻半精靈 獸人（畫面外），",
    "非常非常漫長的玩家種族前綴",
    "艾莉絲（幽靈戰士）", "艾莉絲（幽靈戰士）（畫面外）",
    "4 隻怪物", "牠（畫面外）", "這隻獸人，",
    "超級無敵漫長的獸人種族名稱", "valid_utf8(narrow)",
    "ends_with(narrow, offscreen)", "char one[1]",
):
    assert token in NATIVE, token
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_faangband_mon_msg_utf8.c" in RUNNER
assert "test-fa-mon-msg-utf8.sh" in VERIFY
assert "test_faangband_mon_msg_utf8_source.py" in VERIFY
assert "MON_MSG(HIT_HARD," in REPOSITORY
assert "MON_MSG(DIE," in REPOSITORY
# A composed subject and body already fit well below the actual message.c
# variadic formatting buffer. The game uses two distinct 256-byte arrays,
# so enlarging or reformatting msgt() is unnecessary.
import json
import re
pattern = re.compile(
    r'^\\s*\\{\\s*("(?:\\\\.|[^"\\\\])*"),\\s*("(?:\\\\.|[^"\\\\])*")\\s*\\},',
    re.MULTILINE)
dictionary = {json.loads(en): json.loads(zh)
              for en, zh in pattern.findall(DICT)}
list_keys = []
for line in REPOSITORY.splitlines():
    if line.startswith("MON_MSG("):
        quoted = re.findall(r'"(?:\\\\.|[^"\\\\])*"', line)
        if quoted:
            value = json.loads(quoted[-1])
            if value:
                list_keys.append(value)
pain_keys = list(dict.fromkeys(
    line.split("message:", 1)[1] for line in PAIN_DATA.splitlines()
    if line.startswith("message:")))
assert len(list_keys) == 58
assert len(pain_keys) == 65
assert all(key in dictionary for key in list_keys + pain_keys)
assert max(len(dictionary[key].encode("utf-8"))
           for key in list_keys + pain_keys) == 42
assert 'void msgt(unsigned int type, const char *fmt, ...)' in FORMATTER
assert 'char buf[1024];' in FORMATTER
assert 'vstrnfmt(buf, sizeof(buf), fmt, vp);' in FORMATTER
# Maximum NUL-terminated subject and message body, plus the signed
# integer damage/average suffix. Both intact and under the 1023-byte cap.
max_damage_tail = len("（平均 -2147483648）".encode("utf-8"))
assert 255 + 255 + max_damage_tail < 1024
assert 'char subject[256] = "";' in show
assert 'char body[256];' in show

print("PASS: stacked damage race identity, plural/ghost/hidden UTF-8 subjects")
