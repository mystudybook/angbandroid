#!/usr/bin/env python3
"""FAangband projection text coverage for real UI summaries and monster lash."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/projection.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/projection.txt").read_text(encoding="utf-8")
TERMS = (CPP / "zh-tw-projection-terms.h").read_text(encoding="utf-8")
LASH = (CPP / "zh-tw-mon-spell-lash.h").read_text(encoding="utf-8")
SPELL = (CPP / "ui-spell.c").read_text(encoding="utf-8")
MON = (CPP / "mon-spell.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def projections(data):
    rows, current = [], {}
    for line in data.splitlines():
        match = re.match(r"^(code|name|type|desc|player-desc|blind-desc|lash-desc):(.+)$", line)
        if not match:
            continue
        key, value = match.groups()
        if key == "code":
            if current:
                rows.append(current)
            current = {"code": value}
        else:
            current[key] = value
    if current:
        rows.append(current)
    return rows

def translations(data):
    pairs = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', data, re.M)
    mapped = dict(pairs)
    assert len(mapped) == len(pairs)
    return mapped

fa, ang = projections(FA), projections(ANG)
assert len(fa) == 62 and len(ang) == 56
old_codes = {row["code"] for row in ang}
extra_codes = {row["code"] for row in fa} - old_codes
assert extra_codes == {
    "STORM", "DRAGONFIRE", "HELLFIRE",
    "DISP_DEMON", "DISP_DRAGON", "HOLD_UNDEAD",
}
projection_keys = {
    row[key] for row in fa for key in ("name", "desc", "player-desc")
    if key in row
}
terms = translations(TERMS)
assert projection_keys <= terms.keys()
assert all(terms[k] and terms[k] != k for k in projection_keys)
lash_keys = {row["lash-desc"] for row in fa if "lash-desc" in row}
lash = translations(LASH)
assert lash_keys <= lash.keys()
assert terms["dragonfire"] == lash["dragonfire"] == "龍焰"
assert terms["hellfire"] == lash["hellfire"] == "地獄火"

# Verify these are presentation-only mappings, not canonical projection mutations.
assert '#include "zh-tw-projection-terms.h"' in SPELL
assert "angdroid_projection_description(" in SPELL
assert "effect_projection(e), angdroid_zh_tw_enabled());" in SPELL
assert "angdroid_mon_spell_lash(type_name, true)" in MON
assert "angdroid_mon_spell_lash(type_name, false)" in MON
assert "if (!english || !zh_tw) return english;" in TERMS
assert "if (!english || !zh_tw) return english;" in LASH
assert "test_faangband_projection_locale.c" in RUNNER
assert "test_faangband_projection_source.py" in VERIFY
print("PASS: 62 FAangband projections, 6 unique codes, UI text coverage and lash")
