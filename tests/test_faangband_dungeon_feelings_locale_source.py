#!/usr/bin/env python3
"""Require fully localized FAangband dungeon feelings without game-rule edits.

The canonical English text arrays and feeling codes stay untouched. Only
screen render text changes, while the original English branch remains.
"""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "app/src/main/cpp/faangband/cmd-cave.c").read_text(
    encoding="utf-8")
HEADER = (ROOT / "app/src/main/cpp/angband/zh-tw-ui.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_fa_dungeon_feelings_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-fa-dungeon-feelings-locale.sh").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")
THEMED_DATA = (ROOT / "variant_lib/faangband/gamedata/themed.txt").read_text(
    encoding="utf-8")
THEMED_PARSER = (ROOT / "app/src/main/cpp/faangband/generate.c").read_text(
    encoding="utf-8")



def source_array(name):
    expression = (
        r"static const char \*" + re.escape(name) +
        r"\[\]\s*=\s*\{(.*?)\};"
    )
    match = re.search(expression, SOURCE, re.DOTALL)
    assert match, "Missing canonical feeling array " + name
    return re.findall(r'"((?:\\.|[^"\\])*)"', match.group(1))


object_keys = source_array("obj_feeling_text")
monster_keys = source_array("mon_feeling_text")
assert len(object_keys) == 11
assert len(monster_keys) == 10
assert len(set(object_keys + monster_keys)) == 21

# The nine message: fields are not level identifiers. They remain in data,
# and only their displayed level->typ value is localized.
themed_messages = [line.partition(":")[2] for line in THEMED_DATA.splitlines()
                   if line.startswith("message:")]
assert len(themed_messages) == 9
assert len(set(themed_messages)) == len(themed_messages)
assert 'parser_reg(p, "message str message", parse_themed_message);' in THEMED_PARSER
assert 'v->typ = string_make(parser_getstr(p, "message"));' in THEMED_PARSER
for key in themed_messages:
    source_key = json.dumps(key, ensure_ascii=False)
    assert source_key + "," in HEADER, "Missing themed zh-TW message: " + key
    assert source_key + "," in NATIVE, "Missing themed C99 regression: " + key


for key in object_keys + monster_keys:
    source_key = json.dumps(key, ensure_ascii=False)
    assert source_key + "," in HEADER, "Missing zh-TW feeling: " + key
    assert source_key + "," in NATIVE, "No C99 regression case: " + key

body = SOURCE.split("void display_feeling(bool obj_only)", 1)[1].split(
    "void do_cmd_feeling(void)", 1)[0]
assert body.count("angdroid_zh_tw_enabled()") == 5
assert body.count("angdroid_game_ui_text(") >= 3
assert "if (!OPT(player, birth_feelings)) return;" in body
assert "if (!player->depth)" in body
assert "if (player->themed_level)" in body
assert 'msg("%s", angdroid_game_ui_text(level->typ,' in body
assert "angdroid_zh_tw_enabled()" in body
assert "if (obj_only)" in body
assert "disturb(player);" in body
assert "if (cave->feeling_squares < z_info->feeling_need)" in body
assert 'msg("You feel that %s", obj_feeling_text[obj_feeling]);' in body
assert 'msg("%s.", mon_feeling_text[mon_feeling]);' in body
assert 'msg("%s%s %s", mon_feeling_text[mon_feeling], join,' in body
assert 'msg("你感覺到：%s。", angdroid_game_ui_text(' in body
assert 'msg("%s。", angdroid_game_ui_text(' in body
assert 'msg("%s%s%s。",' in body
assert 'join_zh = "，但";' in body
assert 'join_zh = "，而且";' in body
assert 'join = ", yet";' in body
assert 'join = ", and";' in body
assert "obj_feeling >= N_ELEMENTS(obj_feeling_text)" in body
assert "mon_feeling >= N_ELEMENTS(mon_feeling_text)" in body
assert body.index("obj_feeling >= N_ELEMENTS") < body.index("if (obj_only)")
assert body.index("mon_feeling >= N_ELEMENTS") < body.index(
    "if (cave->feeling_squares")
assert "player->upkeep->energy_use" not in body
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_fa_dungeon_feelings_locale.c" in RUNNER
assert "test-fa-dungeon-feelings-locale.sh" in VERIFY
assert "test_faangband_dungeon_feelings_locale_source.py" in VERIFY
print("PASS: 21 dungeon feelings and 9 themed messages, English fallback and array bounds")
