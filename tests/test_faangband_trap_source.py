#!/usr/bin/env python3
"""FAangband extra trap descriptions + both variants' exact activation strings."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
FA = (ROOT / "variant_lib/faangband/gamedata/trap.txt").read_text(encoding="utf-8")
ANG = (ROOT / "variant_lib/angband/gamedata/trap.txt").read_text(encoding="utf-8")
MAIN = (CPP / "zh-tw-terrain-trap.h").read_text(encoding="utf-8")
EXTRA = (CPP / "zh-tw-faangband-traps.h").read_text(encoding="utf-8")
MSGS = (CPP / "zh-tw-trap-activation-messages.h").read_text(encoding="utf-8")
C = (CPP / "trap.c").read_text(encoding="utf-8")
UI = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def traps(data):
    result, record = [], None
    for line in data.splitlines():
        m = re.match(r"^(name|desc|msg|msg-good|msg-bad|msg-xtra):(.*)$", line)
        if not m:
            continue
        key, val = m.groups()
        if key == "name":
            if record is not None:
                result.append(record)
            record = {"name": val.split(":")[-1]}
        elif record is not None:
            if key == "desc":
                record[key] = record.get(key, "") + val
            else:
                record[key] = val
    if record is not None:
        result.append(record)
    return result

def values(section):
    pairs = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', section, re.M)
    assert len(pairs) == len({key for key, _ in pairs})
    return dict(pairs)

fa, ang = traps(FA), traps(ANG)
assert len(fa) == 55 and len(ang) == 40
ang_names = {t["name"] for t in ang}
new = [t for t in fa if t["name"] not in ang_names]
assert len(new) == 15 and len({t["name"] for t in new}) == 15
names = EXTRA.split("static inline const char *angdroid_faangband_trap_name(", 1)[1].split(
    "static inline const char *angdroid_faangband_trap_lore(", 1)[0]
descs = EXTRA.split("static inline const char *angdroid_faangband_trap_lore(", 1)[1]
name_map, desc_map = values(names), values(descs)
assert {t["name"] for t in new} == name_map.keys()
assert {t["desc"] for t in new} == desc_map.keys()
assert all(name_map[t["name"]] and desc_map[t["desc"]] for t in new)

kinds = ("msg", "msg-good", "msg-bad", "msg-xtra")
all_messages = {t[k] for t in fa + ang for k in kinds if k in t}
assert len(all_messages) == 41
fa_msgs = {t[k] for t in fa for k in kinds if k in t}
ang_msgs = {t[k] for t in ang for k in kinds if k in t}
assert len(fa_msgs) == 40 and len(ang_msgs) == 38
msg_map = values(MSGS)
assert all_messages == msg_map.keys()
assert all(msg_map[x] and msg_map[x] != x for x in all_messages)
assert "A high-pitched whistle blares out!" in msg_map
assert "A high-pitched whistle blares through the dungeon!" in msg_map
assert "A branch hits you from above." in msg_map
assert "It was heavy!" in msg_map
for field in ("msg", "msg_good", "msg_bad", "msg_xtra"):
    assert "angdroid_trap_activation_message(" in C
    assert f"trap->kind->{field}, angdroid_zh_tw_enabled())" in C
assert "if (!en || !zh_tw) return en;" in MSGS
assert "return en;" in MSGS
assert '#include "zh-tw-faangband-traps.h"' in MAIN
assert "angdroid_faangband_trap_name(en, zh_tw)" in MAIN
assert "angdroid_faangband_trap_lore(en, zh_tw)" in MAIN
assert "angdroid_trap_name(trap->desc, angdroid_zh_tw_enabled())" in UI
assert "angdroid_trap_lore(" in UI
assert "effect_do(effect, source_trap(trap)," in C
assert "one_in_(2)" in C
assert "trf_has(trap->kind->flags, TRF_TRAP)" in C
assert "tests/native/test_faangband_trap_locale.c" in NATIVE
assert "tests/test_faangband_trap_source.py" in VERIFY
print("PASS: 15 FA trap names/lore and 41 Angband/FA activation messages")
