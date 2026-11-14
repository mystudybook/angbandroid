#!/usr/bin/env python3
"""Guard monster message display locale against 58 enumerated and 65 pain keys."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-msg.c")
HELPER = read("app/src/main/cpp/faangband/mon-msg-utf8.h")
LIST = read("app/src/main/cpp/faangband/list-mon-message.h")
PAIN = read("variant_lib/faangband/gamedata/pain.txt")
DICT = read("app/src/main/cpp/angband/zh-tw-ui.h")
NATIVE = read("tests/native/test_faangband_monster_messages_locale.c")
SCRIPT = read("scripts/test-fa-monster-messages-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

list_items = re.findall(
    r'^MON_MSG\(\s*[^,]+,\s*[^,]+,\s*(?:false|true),\s*("(?:\\.|[^"\\])*")\)',
    LIST, re.MULTILINE)
list_keys = [json.loads(x) for x in list_items if json.loads(x)]
pain_keys = list(dict.fromkeys(line[len("message:"):]
                               for line in PAIN.splitlines()
                               if line.startswith("message:")))
assert len(list_keys) == 58
assert len(set(list_keys)) == 58
assert len(pain_keys) == 65
assert sum(line.startswith("message:") for line in PAIN.splitlines()) == 84
assert len(set(list_keys + pain_keys)) == 123

pair_re = re.compile(
    r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
    re.MULTILINE)

def pairs(contents):
    return [(json.loads(a), json.loads(b)) for a, b in pair_re.findall(contents)]

translations = pairs(DICT)
native_pairs = pairs(NATIVE)
assert len(native_pairs) == 123
assert [k for k, _ in native_pairs] == list_keys + pain_keys
assert len(set(k for k, _ in native_pairs)) == 123

for en, zh in native_pairs:
    assert (en, zh) in translations, "Missing dictionary entry: " + en
    assert sum(pair[0] == en for pair in translations) == 1, "Duplicate: " + en
    assert len(zh.encode("utf-8")) < 256, "Truncated UTF-8 risk: " + en
    assert zh and "\ufffd" not in zh

# The original [single|plural] parser must also parse localized Moria output.
def expand(source, plural):
    out = []
    pos = 0
    while pos < len(source):
        if source[pos] == "[":
            end = source.find("]", pos + 1)
            assert end >= 0, "Unclosed quantity marker: " + source
            chunks = source[pos + 1:end].split("|")
            assert len(chunks) in (1, 2), "Invalid quantity marker: " + source
            out.append(chunks[-1] if plural else chunks[0])
            pos = end + 1
        else:
            out.append(source[pos])
            pos += 1
    return "".join(out)

moria_key = "You hear [a|several] scream[|s] of agony!"
moria_zh = dict(native_pairs)[moria_key]
assert expand(moria_key, False) == "You hear a scream of agony!"
assert expand(moria_key, True) == "You hear several screams of agony!"
assert expand(moria_zh, False) == "你聽見一聲痛苦的尖叫！"
assert expand(moria_zh, True) == "你聽見數聲痛苦的尖叫！"

assert '#include "../angband/zh-tw-ui.h"' in SOURCE
assert "static bool get_message_text(" in SOURCE
assert "const char *localized = angdroid_game_ui_text(source," in SOURCE
assert "bool translated = (localized != source);" in SOURCE
assert "source = localized;" in SOURCE
assert "return translated;" in SOURCE
assert "case MON_MSG_95: source = race->base->pain->messages[0]; break;" in SOURCE
assert "case MON_MSG_0:  source = race->base->pain->messages[6]; break;" in SOURCE
assert "int state = MSG_PARSE_NORMAL;" in SOURCE
assert "state == MSG_PARSE_SINGLE" in SOURCE
assert "state == MSG_PARSE_PLURAL" in SOURCE
assert "bool localized = get_message_text(body, sizeof(body)," in SOURCE
assert "localized);" in SOURCE
assert "if (localized) {" in SOURCE
assert "char subject[256]" in SOURCE
assert "char body[256]" in SOURCE

# Locale handling does not bypass visibility, message stacking, or sound type.
assert "angdroid_fa_mon_msg_chinese_subject(buf, buflen," in SOURCE
assert 'angdroid_utf8_copy_complete(dst, base_capacity, "牠")' in HELPER
assert 'snprintf(number, sizeof(number), "%d 隻怪物", count)' in HELPER
assert 'static const char suffix[] = "（畫面外）";' in HELPER
assert "base_capacity = capacity - reserved_bytes;" in HELPER
assert "comma_fits = !invisible && comma;" in HELPER
assert 'angdroid_utf8_append_complete(dst, capacity, used, "，");' in HELPER
assert "angdroid_utf8_append_complete(dst, capacity, used, suffix)" in HELPER
assert "mon_msg[size_mon_msg].p_race = mon->player_race;" in SOURCE
assert 'my_strcat(buf, " (offscreen)", buflen);' in SOURCE
assert 'my_strcat(buf, " ", buflen);' in SOURCE
assert "monster_is_obvious(mon)" in SOURCE
assert "MON_MSG_FLAG_INVISIBLE" in SOURCE
assert "MON_MSG_FLAG_OFFSCREEN" in SOURCE
assert "MON_MSG_FLAG_DAMAGE" in SOURCE
assert "get_message_type(msg->msg_code, msg->race)" in SOURCE
assert "skip_subject(msg->msg_code)" in SOURCE
assert "msg->damage / msg->count" in SOURCE
assert 'localized ? "%s%s（平均 %d）"' in SOURCE
assert '"%s%s (average %d)"' in SOURCE
assert "size_mon_msg = size_mon_hist = 0;" in SOURCE

assert "-std=c99 -Wall -Wextra -Werror -pedantic" in SCRIPT
assert "test_faangband_monster_messages_locale.c" in SCRIPT
assert "test-fa-monster-messages-locale.sh" in VERIFY
assert "test_faangband_monster_messages_locale_source.py" in VERIFY
print("PASS: 58 monster messages, 65 unique pain templates, and locale fallback")
