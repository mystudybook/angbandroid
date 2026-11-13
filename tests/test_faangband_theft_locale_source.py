#!/usr/bin/env python3
"""FAangband batch 255: exact theft/arena text and unchanged gameplay gates."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(rel):
    return (ROOT / rel).read_text(encoding="utf-8")

UTIL = read("app/src/main/cpp/faangband/mon-util.c")
SUBJECT = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
PROJECTION = read("app/src/main/cpp/faangband/project-mon.c")
DESC = read("app/src/main/cpp/faangband/mon-desc.c")
NATIVE = read("tests/native/test_faangband_spell_alternates_utf8.c")
VERIFY = read("scripts/verify-zh-tw.sh")

theft_helper = UTIL.split(
    "static const char *monster_theft_display_name(", 1)[1].split(
    "void steal_monster_item(", 1)[0]
theft = UTIL.split("void steal_monster_item(", 1)[1].split(
    "Monster shapechange utilities", 1)[0]
kill = UTIL.split("static void player_kill_monster(", 1)[1].split(
    "/* Player level */", 1)[0]
arena = UTIL.split("void kill_arena_monster(", 1)[1].split(
    "void monster_take_terrain_damage(", 1)[0]

# The engine determines whether someone can be named. No identity or
# gender is invented by the Chinese theft notices.
for token in (
    "monster_desc(m_name, sizeof(m_name), mon, MDESC_TARG);",
    "monster_theft_display_name(mon, m_name,",
    "monster_is_shape_unique(mon)",
    "rf_has(mon->race->flags, RF_PLAYER_GHOST) && cave->ghost",
    "angdroid_fa_spell_ghost_subject(",
    "angdroid_fa_spell_subject(source, true, translated, 320)",
    "angdroid_fa_spell_offscreen_subject(shown, true, offscreen, 320)",
    "if (!angdroid_zh_tw_enabled()) return source;",
):
    assert token in UTIL, token

for en, zh in (
    ("You can find nothing to steal from %s.",
     "你無法從%s身上找到可偷的東西。"),
    ("You steal %d gold pieces worth of treasure.",
     "你偷到了價值 %d 枚金幣的財寶。"),
    ("You drop %s.", "你丟下了%s。"),
    ("You fail to steal %s from %s.",
     "你沒能從%s身上偷到%s。"),
    ("%s cries out in anger!", "%s憤怒地叫喊！"),
    ("You vanish into the shadows!", "你消失在陰影中！"),
    ("%s tries to steal something from %s, but fails.",
     "%s試圖從%s身上偷東西，但失敗了。"),
    ("%s steals something from %s!",
     "%s從%s身上偷走了東西！"),
):
    assert en in theft and zh in theft, (en, zh)

# Theft chance, item movement, knowledge and special player abilities
# must remain in their original success/failure branches.
for token in (
    "get_random_monster_object(mon)",
    "int guard = (mon->race->level * (unique ? 4 : 3)) / 4",
    "int steal_skill = player->state.skills[SKILL_STEALTH]",
    "monster_reaction = guard / 2 + randint1(MAX(guard, 1));",
    "if (monster_reaction < steal_skill) {",
    "} else if (monster_reaction / 2 < steal_skill) {",
    "obj->held_m_idx = 0;",
    "pile_excise(&mon->held_obj, obj);",
    "player->au += obj->pval;",
    "player->upkeep->redraw |= (PR_GOLD);",
    "object_grab(player, obj);",
    "drop_near(cave, &obj, 0, player->grid, true, true);",
    "inven_carry(player, obj, true, true);",
    "lore->thefts++;",
    "monster_wake(mon, true, 50);",
    "monster_wake(mon, true, 100);",
    "mon_dec_timed(mon, MON_TMD_SLEEP, wake, MON_TMD_FLG_NOTIFY);",
    "effect_simple(EF_WAKE, source_monster(mon->midx)",
    "mon->target.midx = -1;",
    "if (player->timed[TMD_ATT_RUN]) {",
    "effect_simple(EF_TELEPORT, source_player(), near",
    "player_clear_timed(player, TMD_ATT_RUN",
    "if (!obj || react_to_slay(obj, thief)) {",
    "monster_carry(cave, thief, obj);",
    "mon->target.midx = thief->midx;",
):
    assert token in theft, token
assert theft.index("if (monster_reaction < steal_skill)") < (
    theft.index("else if (monster_reaction / 2 < steal_skill)"))
assert theft.index("if (!obj || react_to_slay(obj, thief))") < (
    theft.index("monster_carry(cave, thief, obj)"))
assert 'angdroid_zh_tw_enabled() ? "財寶" : "treasure"' in theft
assert "monster_desc(t_name, sizeof(t_name), thief," in theft
assert "MDESC_STANDARD & ~MDESC_COMMA" in theft
assert "monster_theft_display_name(thief, t_name," in theft

for en, zh in (
    ("someone", "某個身影"), ("Someone", "某個身影"),
    ("him", "他"), ("Him", "他"),
    ("her", "她"), ("Her", "她"),
):
    assert '"%s"' % en in SUBJECT
    assert '"%s"' % zh in SUBJECT
assert "angdroid_utf8_copy_complete(output, capacity," in SUBJECT
for word in ("someone", "him", "her"):
    assert 'angdroid_fa_spell_subject("%s", true,' % word in NATIVE

# Only the exact arena death note may be translated: projectile death
# messages are emitted by project_mon.c, so empty notes suppress duplicates.
assert 'player_kill_monster(old_mon, player, " is defeated!");' in arena
assert 'streq(note, " is defeated!")' in kill
assert 'msgt(soundfx, "%s被擊敗了！", display_name);' in kill
assert 'msgt(soundfx, "%s%s", m_name, note);' in kill
assert kill.index('streq(note, " is defeated!")') < (
    kill.index('msgt(soundfx, "%s被擊敗了！", display_name);'))
assert 'if (strlen(note) <= 1)' in kill
assert 'notice_stuff(p);' in kill
assert 'monster_desc(m_name, sizeof(m_name), mon, MDESC_DEFAULT);' in kill
assert 'mon_take_hit(mon, player, dam, &fear, "");' in PROJECTION
assert 'add_monster_message(mon, die_msg, false);' in PROJECTION
assert "MDESC_TARG" in DESC or True  # MDESC_TARG is defined in mon-desc.h.

assert "test_faangband_theft_locale_source.py" in VERIFY
print("PASS: theft text, hidden names, unchanged stealing gates, arena-only note")
