#!/usr/bin/env python3
"""Batch 254: FAangband dynamic monster messages stay localized and gated.

These are source guards, not gameplay or Android/NDK execution tests.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SPELL = read("app/src/main/cpp/faangband/mon-spell.c")
UTIL = read("app/src/main/cpp/faangband/mon-util.c")
LOCALE = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
VERIFY = read("scripts/verify-zh-tw.sh")

spell = SPELL.split("static void spell_message(", 1)[1].split(
    "const struct monster_spell *monster_spell_by_index(", 1)[0]
shape_message = UTIL.split(
    "static void message_monster_changes_shape(", 1)[1].split(
    "bool monster_change_shape(", 1)[0]
shape_change = UTIL.split("bool monster_change_shape(", 1)[1].split(
    "bool monster_revert_shape(", 1)[0]
shape_revert = UTIL.split("bool monster_revert_shape(", 1)[1]
player_kill = UTIL.split("static void player_kill_monster(", 1)[1].split(
    "/* Player level */", 1)[0]
mimic = UTIL.split("void become_aware(", 1)[1].split(
    "/* Clear the mimicry */", 1)[0]

# The previous report-this-bug strings must remain unchanged in English.
english_diagnostics = (
    '"No message-invis for monster "',
    '"spell %d cast by %s.  "',
    '"No message-miss for monster spell %d "',
    '"No message-vis for monster spell %d "',
    '"Please report this bug."',
    '"cast by %s.  Please report this bug."',
)
for phrase in english_diagnostics:
    assert phrase in spell, phrase

for phrase in (
    '"缺少視野外施法訊息，請回報此問題。"',
    '"怪物 %s 的法術 %d 缺少未命中訊息，請回報此問題。"',
    '"怪物 %s 的法術 %d 缺少可見施法訊息，請回報此問題。"',
):
    assert phrase in spell, phrase

# Unseen diagnostics deliberately omit both caster identity and spell ID.
unseen = spell.split('in_cursor = level->blind_message;', 1)[1].split(
    "else if (in_cursor[0] == '\\0')", 1)[0]
assert 'if (angdroid_zh_tw_enabled()) {' in unseen
zh_unseen = unseen.split("if (angdroid_zh_tw_enabled()) {", 1)[1].split(
    "} else {", 1)[0]
assert "mon->race->name" not in zh_unseen
assert "spell->index" not in zh_unseen
assert "msg(\"缺少視野外施法訊息，請回報此問題。\");" in zh_unseen

# Still choose alternate or built-in messages before entering any diagnostic.
for token in (
    "MON_ALTMSG_UNSEEN", "MON_ALTMSG_MISS", "MON_ALTMSG_SEEN",
    "level->blind_message", "level->miss_message", "level->message",
    "if (!seen) {", "if (t_mon) {", "} else if (!hits) {",
    "else if (in_cursor[0] == '\\0') {",
    "const char *selected = in_cursor;",
    "angdroid_fa_spell_message_locale(selected,",
    "angdroid_fa_spell_template_fits(",
    "msgt(spell->msgt, \"%s\", safe_buf);",
    "msgt(spell->msgt, \"%s\", buf);",
):
    assert token in spell, token
assert spell.count("if (angdroid_zh_tw_enabled()) {") == 3

# Shapechange messages have one shared formatter but keep both original
# source call sites and the original visibility, race and redraw gates.
assert '#include "mon-spell-message-locale.h"' in UTIL
assert '#include "../angband/zh-tw-ui.h"' in UTIL
for token in (
    "angdroid_zh_tw_enabled()",
    "MDESC_STANDARD & ~MDESC_COMMA",
    "monster_desc(original_name, sizeof(original_name), mon, mode)",
    'msgt(MSG_GENERIC, "%s %s", original_name, "shimmers and changes!");',
    'msgt(MSG_GENERIC, "%s的外形閃爍並改變了！", display_name);',
    "rf_has(mon->race->flags, RF_PLAYER_GHOST)",
    "monster_is_shape_unique(mon)",
    "angdroid_fa_spell_ghost_subject(",
    "angdroid_fa_spell_subject(",
    "angdroid_fa_spell_offscreen_subject(",
):
    assert token in shape_message, token
assert shape_change.count("message_monster_changes_shape(mon);") == 1
assert shape_revert.count("message_monster_changes_shape(mon);") == 1
assert shape_change.index("monster_is_obvious(mon)") < (
    shape_change.index("message_monster_changes_shape(mon);"))
assert shape_revert.index("monster_is_obvious(mon)") < (
    shape_revert.index("message_monster_changes_shape(mon);"))
assert "if (player->upkeep->health_who == mon)" in shape_change
assert "if (player->upkeep->health_who == mon)" in shape_revert
assert "player->upkeep->redraw |= (PR_MONLIST)" in shape_change
assert "player->upkeep->redraw |= (PR_MONLIST)" in shape_revert
assert "square_light_spot(cave, mon->grid)" in shape_change
assert "square_light_spot(cave, mon->grid)" in shape_revert

# Mimic revelation remains visible only after square_isseen(), with
# all original camouflage/object mutations intact.
assert 'msg("原來%s是一隻怪物！", o_name);' in mimic
assert 'msg("The %s was really a monster!", o_name);' in mimic
assert "square_isseen(c, obj->grid)" in mimic
assert mimic.index("square_isseen(c, obj->grid)") < mimic.index(
    'msg("原來%s是一隻怪物！", o_name);')

# The Soul Siphon feature remains a gameplay calculation, not a new
# condition added by translation.
for token in (
    '"你獲得了法力。"',
    '"You gain mana."',
    '"你吸收了過多法力！"',
    '"You absorb too much mana!"',
    "player_has(player, PF_SOUL_SIPHON)",
    "take_hit(player, damroll(2, 8), \"mana burn\")",
    "player->csp = MIN(player->csp + gain, player->msp)",
):
    assert token in UTIL, token

for english, chinese in (
    ('"You have killed %s."', '"你擊殺了%s。"'),
    ('"You have destroyed %s."', '"你摧毀了%s。"'),
    ('"You have slain %s."', '"你擊敗了%s。"'),
):
    assert english in player_kill and chinese in player_kill, english

for token in (
    'char m_name[256];',
    'char zh_name[320];',
    "monster_is_shape_unique(mon)",
    "angdroid_fa_spell_subject(m_name,",
    "if (!monster_is_visible(mon))",
    "else if (monster_is_destroyed(mon))",
    "notice_stuff(p);",
    "if (strlen(note) <= 1)",
    'msgt(soundfx, "%s%s", m_name, note);',
    "monster_revert_shape(mon);",
):
    assert token in player_kill, token
assert player_kill.index("if (!monster_is_visible(mon))") < (
    player_kill.index("else if (monster_is_destroyed(mon))"))
assert "angdroid_fa_spell_subject(" in LOCALE

assert "test_faangband_combat_messages_locale_source.py" in VERIFY
print("PASS: 3 diagnostics, 2 shape notices, mimic, mana, kill messages")
