#!/usr/bin/env python3
"""Batch 256: real FAangband spell-fail and melee-notification localization.

This validates actual code branches without claiming to execute monster AI.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-attack.c")
LOCALE = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
DESC = read("app/src/main/cpp/faangband/mon-desc.h")
RUNNER = read("scripts/verify-zh-tw.sh")

helper = SOURCE.split(
    "static const char *monster_attack_display_name(", 1)[1].split(
    "This file deals with monster attacks", 1)[0]
cast = SOURCE.split("thrown_spell = choose_attack_spell(", 1)[1].split(
    "/* A spell was cast */", 1)[0]
avoid = SOURCE.split("static bool player_avoid_blow(", 1)[1].split(
    "int adjust_dam_armor(", 1)[0]
to_player = SOURCE.split("bool make_attack_normal(", 1)[1].split(
    "bool monster_attack_monster(", 1)[0]
to_monster = SOURCE.split("bool monster_attack_monster(", 1)[1]

assert '#include "mon-spell-message-locale.h"' in SOURCE
assert '#include "../angband/zh-tw-ui.h"' in SOURCE

# Fresh monster_desc() re-obeys the visibility gate. In particular,
# no branch pulls raw mon->race->name for an unseen actor or target.
for token in (
    "monster_desc(name, 256, mon, mode & ~MDESC_COMMA);",
    "rf_has(mon->race->flags, RF_PLAYER_GHOST) && cave->ghost",
    "angdroid_fa_spell_ghost_subject(",
    "monster_is_shape_unique(mon)",
    "!monster_is_visible(mon)",
    "angdroid_fa_spell_subject(name, true, translated, 320)",
    "angdroid_fa_spell_offscreen_subject(shown, true,",
    "return angdroid_fa_spell_offscreen_subject(",
):
    assert token in helper, token
assert "monster_desc" in DESC
assert "MDESC_STANDARD" in DESC and "MDESC_TARG" in DESC
assert 'mon->race->name' in helper  # Only for the already-known ghost suffix.
assert "shown = angdroid_fa_spell_subject" in helper
assert "angdroid_zh_tw_enabled()" not in helper  # Call only from zh branch.

# All existing English display messages must remain in their own else
# branches, with the original spell/attack chance and outcome predicates.
for en, zh, body in (
    ('"%s tries to cast a spell, but fails."',
     '"%s試圖施法，但失敗了。"', cast),
    ('"You gain mana."', '"你獲得了法力。"', cast),
    ('"%s is repelled."', '"%s被防護力量擊退了。"', avoid),
    ('"You evade the attack!"', '"你閃避了攻擊！"', avoid),
    ('"%s misses you."', '"%s沒有擊中你。"', to_player),
    ('"%s misses %s."', '"%s沒有擊中%s。"', to_monster),
):
    assert en in body and zh in body, (en, zh)

assert "if (!mon_spell_is_innate(thrown_spell) && (randint0(100) < failrate))" in cast
assert "monster_is_camouflaged(mon)" in cast
assert "become_aware(cave, mon)" in cast
assert "do_mon_spell(thrown_spell, mon, seen);" in cast
assert "player_has(player, PF_POWER_SIPHON)" in cast
assert "monster_is_visible(mon)" in cast
assert "player->csp = MIN(player->csp + gain, player->msp);" in cast
assert "player->upkeep->redraw |= PR_MANA;" in cast
assert "rsf_on(lore->spell_flags, thrown_spell);" in cast

assert "p->timed[TMD_PROTEVIL] > 0" in avoid
assert "monster_is_evil(mon)" in avoid
assert "randint0(100) + p->lev > 50" in avoid
assert "return true;" in avoid
assert "randint1(100) <= p->state.evasion_chance" in avoid
assert "!p->timed[TMD_PARALYZED]" in avoid
assert "return false;" in avoid

for body in (to_player, to_monster):
    assert 'msg("錯誤：找不到%s的攻擊效果處理器。", effect->name);' in body
    assert 'msg("ERROR: Effect handler not found for %s.", effect->name);' in body
    assert "effect_handler(&context);" in body
    assert "effect_handler != NULL" in body
    assert "STUN_DAM_REDUCTION" in body
    assert "lore->blows[ap_cnt].times_seen" in body
assert "check_hit(p, chance_of_monster_hit(mon, effect))" in to_player
assert "player_avoid_blow(mon, m_name, p)" in to_player
assert "monster_is_visible(mon) &&\tmethod->miss" in to_player
assert "disturb(p);" in to_player
assert "monster_is_visible(mon) && method->miss" in to_monster
assert "test_hit(chance_of_monster_hit(mon, effect), t_mon->race->ac)" in to_monster
assert "square_monster(cave, grid)" in to_monster
assert "MDESC_TARG, target, zh_target, offscreen_target" in to_monster
assert to_monster.count("monster_attack_display_name(") >= 2

# Preserve the prior exact English spell and combat message sources.
assert SOURCE.count('msg("ERROR: Effect handler not found for %s.", effect->name);') == 2
assert SOURCE.count('msg("錯誤：找不到%s的攻擊效果處理器。", effect->name);') == 2
assert "angdroid_fa_spell_subject(" in LOCALE
assert "angdroid_fa_spell_ghost_subject(" in LOCALE
assert "angdroid_fa_spell_offscreen_subject(" in LOCALE
assert "test_faangband_mon_attack_locale_source.py" in RUNNER
print("PASS: 6 dynamic battle notices, 2 error handlers and unchanged gates")
