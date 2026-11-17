#!/usr/bin/env python3
"""Verify FAangband's six unique on-increase messages and exact UI translations."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
ANG = (ROOT / "variant_lib/angband/gamedata/player_timed.txt").read_text(encoding="utf-8")
FA = (ROOT / "variant_lib/faangband/gamedata/player_timed.txt").read_text(encoding="utf-8")
TRANSLATIONS = (CPP / "zh-tw-timed-messages.h").read_text(encoding="utf-8")
PLAYER = (CPP / "player-timed.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def messages(data):
    result = set()
    for line in data.splitlines():
        if line.startswith("grade:"):
            parts = line.split(":")
            for part in parts[4:6]:
                if len(part) > 1:
                    result.add(part)
        elif line.startswith(("on-end:", "on-increase:", "on-decrease:")):
            result.add(line.split(":", 1)[1])
    return result

standard = messages(ANG)
fa = messages(FA)
only_fa = fa - standard
elements = ("acid", "electricity", "fire", "cold", "poison", "confusion")
expected = {f"Your resistance to {el} feels renewed!" for el in elements}
assert len(fa) == len(standard) == 148
assert only_fa == expected

pairs = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', TRANSLATIONS, re.M)
mapping = dict(pairs)
assert len(mapping) == len(pairs)
assert all(source in mapping and mapping[source] != source for source in fa)
for source in expected:
    assert mapping[source].startswith("你的")
    assert mapping[source].endswith("抗性效果已延長！")
assert 'angdroid_timed_message(new_grade->up_msg, angdroid_zh_tw_enabled())' in PLAYER
assert 'angdroid_timed_message(effect->on_increase, angdroid_zh_tw_enabled())' in PLAYER
assert "if (!english || !zh_tw) return english;" in TRANSLATIONS
assert 'test_faangband_timed_renewals.c' in NATIVE
assert 'test_faangband_timed_renewals_source.py' in VERIFY
print("PASS: 6 FAangband-only timed renewals and all 148 messages covered")
