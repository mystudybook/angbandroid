#!/usr/bin/env python3
"""Monster lore abilities: source-derived flags, known-only gates and grammar."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

FLAGS = read("app/src/main/cpp/faangband/list-mon-race-flags.h")
LORE = read("app/src/main/cpp/faangband/mon-lore.c")
DICT = read("app/src/main/cpp/angband/zh-tw-ui.h")
HELPER = read("app/src/main/cpp/faangband/mon-lore-display.h")
NATIVE = read("tests/native/test_faangband_lore_abilities_locale.c")
RUNNER = read("scripts/test-fa-lore-abilities-locale.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

raw = re.findall(
    r'^RF\([^,]+,\s*RFT_(?:ALTER|DET|VULN|VULN_I|RES|PROT),'
    r'\s*"([^"]+)"\)', FLAGS, re.MULTILINE)
assert len(raw) == 35, "Source flag count changed"
keys = list(dict.fromkeys(raw))
assert len(keys) == 33
assert len(raw) - len(keys) == 2
assert raw.count("fire") == 2 and raw.count("cold") == 2

pair_re = re.compile(
    r'^\s*\{("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\},',
    re.MULTILINE)
def pairs(text):
    return [(json.loads(a), json.loads(b))
            for a, b in pair_re.findall(text)]

translations = pairs(DICT)
native_cases = pairs(NATIVE)
assert [en for en, _ in native_cases] == keys
assert len(native_cases) == 33
for en, zh in native_cases:
    assert (en, zh) in translations, "Untranslated flag: " + en
    assert sum(k == en for k, _ in translations) == 1
    assert zh and "\ufffd" not in zh

clause = LORE.split("static void lore_append_clause(", 1)[1].split(
    "static void lore_append_spell_clause(", 1)[0]
abilities = LORE.split("void lore_append_abilities(", 1)[1].split(
    "void lore_append_awareness(", 1)[0]
assert '#include "mon-lore-display.h"' in LORE
assert '#include "../angband/zh-tw-ui.h"' in LORE
for token in [
    "int count = rf_count(f);", "int emitted = 0;",
    "for (flag = rf_next(f, FLAG_START); flag;",
    "angdroid_fa_lore_separator(",
    'streq(conjunction, "or")',
    "angdroid_game_ui_text(describe_race_flag(flag), zh_tw)",
    "angdroid_fa_lore_sentence_end(end, zh_tw)",
    "textblock_append_c(tb, attr,",
    'textblock_append(tb, ",");',
    "textblock_append(tb, \" \");",
    "emitted++;"
]:
    assert token in clause, token
for token in [
    "angdroid_fa_lore_separator(", 'alternatives ? "或" : "與"',
    'return "、";', "angdroid_fa_lore_sentence_end(",
    'return "。  ";'
]:
    assert token in HELPER, token

# The source bitmasks and knowledge gates remain the sole authority.
for token in [
    "create_mon_flag_mask(current_flags, RFT_ALTER, RFT_MAX);",
    "create_mon_flag_mask(current_flags, RFT_DET, RFT_MAX);",
    "create_mon_flag_mask(current_flags, RFT_VULN, RFT_VULN_I, RFT_MAX);",
    "create_mon_flag_mask(current_flags, RFT_RES, RFT_MAX);",
    "create_mon_flag_mask(test_flags, RFT_VULN, RFT_MAX);",
    "create_mon_flag_mask(test_flags, RFT_RES, RFT_MAX);",
    "create_mon_flag_mask(test_flags, RFT_VULN_I, RFT_MAX);",
    "create_mon_flag_mask(current_flags, RFT_PROT, RFT_MAX);",
    "rf_inter(current_flags, known_flags);",
    "rf_inter(test_flags, known_flags);",
    "rf_has(lore->flags, flag) && !rf_has(known_flags, flag)",
    "streq(describe_race_flag(flag), describe_race_flag(susc_flag))",
    "rf_has(known_flags, RF_UNDEAD)",
    "rf_off(current_flags, RF_IM_NETHER);",
    "lore_append_clause(tb, current_flags, COLOUR_VIOLET, start,",
    "lore_append_clause(tb, current_flags, COLOUR_L_UMBER, start,",
    "lore_append_clause(tb, current_flags, COLOUR_WHITE, start,",
    "char start[96];",
    "const bool zh_tw = angdroid_zh_tw_enabled();",
    'zh_tw ? "。  " : ".  "',
]:
    assert token in abilities, token

for flag in ["RF_UNAWARE", "RF_MULTIPLY", "RF_REGENERATE",
             "RF_FLYING", "RF_TERRITORIAL"]:
    assert "rf_has(known_flags, " + flag + ")" in abilities
for light in ["race->light > 1", "race->light == 1",
              "race->light == -1", "race->light < -1"]:
    assert light in abilities
for desc in ["牠能偽裝成其他事物。", "牠能快速繁殖。",
             "牠能快速恢復生命。", "牠能飛行。",
             "牠在自己的領地上更具攻擊性。",
             "牠能照亮四周。", "牠全身散發光芒。",
             "牠的身影籠罩在黑暗中。", "牠使四周籠罩在黑暗中。"]:
    assert desc in abilities, desc

assert "test_faangband_lore_abilities_locale.c" in RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-lore-abilities-locale.sh" in VERIFY
assert "test_faangband_lore_abilities_locale_source.py" in VERIFY
print("PASS: 35 flag source rows, 33 labels, known-flag gates and Chinese lists")
