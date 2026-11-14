#!/usr/bin/env python3
"""FAangband spell, breath and melee lore display-only regressions.

Derive coverage from game-data records; never edit parser data to localize it.
"""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

SOURCE = read("app/src/main/cpp/faangband/mon-lore.c")
TERMS = read("app/src/main/cpp/faangband/mon-lore-terms.h")
SPELLS = read("variant_lib/faangband/gamedata/monster_spell.txt")
METHODS = read("variant_lib/faangband/gamedata/blow_methods.txt")
EFFECTS = read("variant_lib/faangband/gamedata/blow_effects.txt")
NATIVE = read("tests/native/test_faangband_lore_spells_attacks_locale.c")
RUNNER = read("scripts/test-fa-lore-spells-attacks-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

def source_labels(data, prefix):
    return list(dict.fromkeys(
        line[len(prefix):] for line in data.splitlines()
        if line.startswith(prefix) and line[len(prefix):]
    ))

spell_terms = source_labels(SPELLS, "lore:")
method_terms = source_labels(METHODS, "desc:")
effect_terms = source_labels(EFFECTS, "desc:")
assert (len(spell_terms), len(method_terms), len(effect_terms)) == (
    116, 19, 26)
combined = list(dict.fromkeys(spell_terms + method_terms + effect_terms))
assert len(combined) == 156
assert len(spell_terms + method_terms + effect_terms) - len(combined) == 5

# mon-init.c uses string_append() for repeated lore: lines at one power
# cutoff. In that case a single source-line lookup would not match the
# assembled runtime description; reject such future data changes here.
lore_at_cutoff = 0
for line in SPELLS.splitlines():
    if line.startswith("name:") or line.startswith("power-cutoff:"):
        assert lore_at_cutoff <= 1, "Concatenated lore needs a new term: " + line
        lore_at_cutoff = 0
    elif line.startswith("lore:"):
        lore_at_cutoff += 1
assert lore_at_cutoff <= 1

pair_re = re.compile(
    r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
    re.MULTILINE)

def pairs(source):
    return [(json.loads(a), json.loads(b)) for a, b in pair_re.findall(source)]

translations = pairs(TERMS)
native = pairs(NATIVE)
assert len(translations) == len(native) == 156
assert [k for k, _ in translations] == combined
assert native == translations
assert len({k for k, _ in translations}) == 156
for en, zh in translations:
    assert en and zh and "\ufffd" not in zh, en
    assert "{" not in zh and "}" not in zh, en

assert '#include "mon-lore-terms.h"' in SOURCE
assert "static inline const char *angdroid_fa_lore_term(" in TERMS
assert "if (!english || !zh_tw) return english;" in TERMS
assert "if (strcmp(english, terms[i].english) == 0)" in TERMS
assert "return english;" in TERMS
assert '#include "app/src/main/cpp/faangband/mon-lore-terms.h"' in NATIVE

clause = SOURCE.split("static void lore_append_spell_clause(", 1)[1].split(
    "void lore_append_kills(", 1)[0]
freq = SOURCE.split("static void lore_append_spell_frequency(", 1)[1].split(
    "void lore_append_spells(", 1)[0]
spells = SOURCE.split("void lore_append_spells(", 1)[1].split(
    "void lore_append_attack(", 1)[0]
attack = SOURCE.split("void lore_append_attack(", 1)[1].split(
    "struct monster_lore *get_lore(", 1)[0]

# Damage and colors remain based on the same engine APIs and known HP gate.
for token in [
    "int count = rsf_count(f);",
    "rsf_next(f, FLAG_START)",
    "mon_spell_lore_damage(spell, race, know_hp)",
    "spell_color(player, race, spell)",
    "angdroid_fa_lore_term(",
    "mon_spell_lore_description(spell, race)",
    "angdroid_fa_lore_separator(",
    'streq(conjunction, "or")',
    'zh_tw ? "（%d）" : " (%d)"',
    "angdroid_fa_lore_sentence_end(end, zh_tw)"
]:
    assert token in clause, token

# Preserve lore->spell_flags filtering and restore ref_race after all branches.
for token in [
    "know_hp = lore->armour_known;",
    "old_ref = ref_race;",
    "ref_race = race;",
    "ref_race = old_ref;",
    "create_mon_spell_mask(current_flags, RST_INNATE, RST_NONE);",
    "create_mon_spell_mask(current_flags, RST_BREATH, RST_NONE);",
    "create_mon_spell_mask(test_flags, RST_BREATH, RST_INNATE, RST_NONE);",
    "rsf_inter(current_flags, lore->spell_flags);",
    "rsf_diff(current_flags, test_flags);",
    "rsf_copy(current_flags, lore->spell_flags);",
    "if (!rsf_is_empty(current_flags))",
    'zh_tw ? "吐出" : "breathe "',
    'zh_tw ? "施放法術" : "cast spells"',
    "rf_has(known_flags, RF_SMART)",
    "lore_append_spell_clause(tb, current_flags, know_hp, race,",
    "lore->innate_freq_known",
    "lore->spell_freq_known",
    "lore->cast_innate",
    "lore->cast_spell",
    "100 / race->freq_innate",
    "100 / race->freq_spell",
    "100 / approx_frequency",
    "MAX(((race->freq_innate + 9) / 10) * 10, 1)",
    "MAX(((race->freq_spell + 9) / 10) * 10,",
    "else if (zh_tw && (innate || breath))"
]:
    assert token in spells, token
assert spells.count("lore_append_spell_frequency(tb,") == 4
assert spells.count("lore_append_spell_clause(tb,") == 3
assert "approximate ? \"；約每 \" : \"；每 \"" in freq
assert "approximate ? \"; about \" : \"; \"" in freq
assert 'textblock_append_c(tb, COLOUR_L_GREEN, "%d", interval);' in freq
assert 'textblock_append_c(tb, COLOUR_L_GREEN, "1");' in freq

# Unknown attacks remain hidden and original dice/hit chance math survives.
for token in [
    "rf_has(known_flags, RF_NEVER_BLOW)",
    "lore->blow_known[i]",
    "if (!race->blow[i].method || !lore->blow_known[i]) continue;",
    "z_info->mon_blows_max",
    "described_count < known_attacks - 1",
    "known_attacks < total_attacks",
    "race->blow[i].dice",
    "race->blow[i].effect->desc",
    "angdroid_fa_lore_term(",
    "blow_color(player, index)",
    "hit_chance(&c, chance_of_monster_hit_base(race, race->blow[i].effect),",
    "player->state.ac + player->state.to_a",
    "int percent = random_chance_scaled(c, 100);",
    "total_centidamage += (percent * randcalc(dice, 0, AVERAGE));",
    "total_centidamage / 100",
    'zh_tw ? "%%）" : "%%)"',
    'zh_tw ? "，" : ", "',
    " 點傷害。  "
]:
    assert token in attack, token

assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_faangband_lore_spells_attacks_locale.c" in RUNNER
assert "test-fa-lore-spells-attacks-locale.sh" in VERIFY
assert "test_faangband_lore_spells_attacks_locale_source.py" in VERIFY
print("PASS: source-derived 116 spell / 19 method / 26 effect records, 156 unique terms")
