#!/usr/bin/env python3
"""FAangband source contract: element-index labels and real resist percentages.

A display-only improvement. Unknown or invalid abilities must not dereference
race->el_info[] or change canonical race, class, savefile or command data.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
BIRTH = (FA / "ui-birth.c").read_text(encoding="utf-8")
HEADER = (FA / "ui-birth-elements-zh-tw.h").read_text(encoding="utf-8")
OFHEADER = (FA / "ui-birth-abilities-zh-tw.h").read_text(encoding="utf-8")
ENUM = (FA / "list-elements.h").read_text(encoding="utf-8")
PROPERTY = (FA / "obj-properties.h").read_text(encoding="utf-8")
FLAGS = (FA / "list-object-flags.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def require(source, *terms):
    for term in terms:
        assert term in source, term


native = re.findall(r"^ELEM\((\w+)\)$", ENUM, re.MULTILINE)
localized = re.findall(r'case ELEM_(\w+): return "([^"]+)";', HEADER)
assert len(native) == 28, native
assert len(localized) == len(native)
assert len(localized) == len(set(k for k, _ in localized))
assert set(k for k, _ in localized) == set(native)
assert all(v.strip() for _, v in localized)

require(PROPERTY,
    "#define RES_LEVEL_BASE\t\t100",
    "#define RES_LEVEL_MAX\t\t0",
)
require(HEADER,
    "static inline const char *angdroid_fa_birth_element_name(",
    "static inline int angdroid_fa_birth_element_resistance(int percentage)",
    "if (percentage == RES_LEVEL_MAX) return 2;",
    "percentage > RES_LEVEL_MAX && percentage < RES_LEVEL_BASE",
    "if (percentage > RES_LEVEL_BASE) return -1;",
    "default: return NULL;",
)
require(BIRTH,
    '#include "ui-birth-elements-zh-tw.h"',
    "static const char *fa_birth_ability_visible_name(",
    'if (zh && ability->type && streq(ability->type, "element"))',
    "element = angdroid_fa_birth_element_name(ability->index);",
    "tier = angdroid_fa_birth_element_resistance(ability->value);",
    'if (tier == 2) return format("免疫：%s", element);',
    'if (tier == 1) return format("抗性：%s", element);',
    'if (tier == -1) return format("弱點：%s", element);',
    "return angdroid_fa_birth_ability_name(",
)
assert BIRTH.count("fa_birth_ability_visible_name(") == 4
assert BIRTH.count("angdroid_fa_birth_ability_name(") == 1

# All three views filter by the real trait type, index, race/level state.
# Invalid element indexes never reach el_info[]; forged OF/PF indexes do
# not reach native bitsets either.
assert BIRTH.count("ability->index >= ELEM_MAX") == 2
assert BIRTH.count("ability->index >= PF_MAX") == 3
assert BIRTH.count("ability->index >= OF_MAX") == 3
require(BIRTH,
    "race->el_info[ability->index].res_level !=",
    "r->el_info[ability->index].res_level != ability->value",
    "pf_has(r->pflags, ability->index)",
    "of_has(c->flags, ability->index)",
)
for forbidden in ("cmdq_push(", "cmd_set_arg_", "of_on(", "pf_on(",
                  "player->", "savefile"):
    assert forbidden not in HEADER

# All real OF names now have display labels. OF_MAX is an enum sentinel.
known_of = set(re.findall(r"^OF\((\w+),", FLAGS, re.MULTILINE))
mapped_of = re.findall(r'case OF_(\w+): return "([^"]+)";', OFHEADER)
assert set(key for key, _ in mapped_of) == known_of - {"MAX"}
assert "case OF_MAX:" not in OFHEADER
for name in ("IMPAIR_HP", "IMPAIR_MANA", "AFRAID", "NO_TELEPORT",
             "DRAIN_EXP", "FRAGILE", "STICKY", "DARKNESS"):
    assert name in {k for k, _ in mapped_of}

assert "test_faangband_birth_elements_source.py" in RUNNER
assert "test-fa-birth-elements.sh" in RUNNER
print("PASS: FA 28 elements, resist tiers, all native OF flags and bounds")
