#!/usr/bin/env python3
"""Display-only monster target names/status; retain invisibility and game rules."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")
H = read("app/src/main/cpp/angband/zh-tw-target-monster.h")
TARGET = read("app/src/main/cpp/angband/target.c")
UI = read("app/src/main/cpp/angband/ui-target.c")
DESC = read("app/src/main/cpp/angband/mon-desc.c")
NORMAL = read("app/src/main/cpp/angband/zh-tw-monsters.h")
UNIQUE = read("app/src/main/cpp/angband/zh-tw-unique-monsters.h")
FA = read("app/src/main/cpp/angband/zh-tw-faangband-monster-flavor.h")
NATIVE = read("scripts/test-native-utf8.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

status_data = H.split("static const struct { const char *en, *zh; } terms[] = {", 1)[1].split("    };", 1)[0]
pairs = re.findall(r'^\s*\{ "([^"\n]+)", "([^"\n]+)" \},$', status_data, re.M)
assert len(pairs) == len({name for name, _ in pairs}) == 18
conditions = dict(pairs)
health = (
    "unhurt", "undamaged", "somewhat wounded", "somewhat damaged",
    "wounded", "damaged", "badly wounded", "badly damaged",
    "almost dead", "almost destroyed",
)
timed = ("asleep", "held", "disenchanted", "confused", "afraid",
         "stunned", "slowed", "hasted")
assert set(health + timed) == set(conditions)
assert all(f'"{condition}"' in TARGET for condition in health)
assert all(f'", {state}"' in TARGET for state in timed)
assert all(conditions[key] and conditions[key] != key for key in conditions)

# Display-only: use the actual English descriptor as visibility proof.
assert 'strncmp(start, race_name, strlen(race_name)) != 0' in H
assert 'strcmp(suffix, " (offscreen)")' in H
assert 'strcmp(suffix, ", (offscreen)")' in H
assert 'angdroid_utf8_append_complete(' in H
assert 'angdroid_zh_tw_unique_monster_name(race_name)' in H
assert 'angdroid_zh_tw_monster_list_name(race_name)' in H
assert 'return angdroid_faangband_monster_name(english);' in NORMAL
assert 'return angdroid_faangband_monster_name(english);' in UNIQUE
assert '\"blue dragon\"' in FA
assert '\"Mughâsh the Kobold Lord\"' in FA
assert 'monster_desc(m_name, sizeof(m_name), mon, MDESC_IND_VIS);' in UI
assert 'angdroid_target_monster_display(name_zh, sizeof(name_zh),' in UI
assert 'angdroid_target_monster_status(status_zh,' in UI
assert 'angdroid_target_monster_intro(auxst->phrase1, true)' in UI
assert 'look_mon_desc(buf, sizeof(buf),' in UI
assert 'angdroid_target_monster_display(' not in DESC
assert 'void monster_desc(' in DESC

# Keep target identity/selection and macros unchanged; only outgoing messages.
assert 'monster_desc(m_name, sizeof(m_name), mon, MDESC_CAPITAL | MDESC_COMMA);' in TARGET
assert 'if (!(mode & TARGET_QUIET)) {' in TARGET
assert 'if (angdroid_zh_tw_enabled() && angdroid_target_monster_display(' in TARGET
assert 'msg("已鎖定%s。", zh_name);' in TARGET
assert 'msg("%s is targeted.", m_name);' in TARGET
assert TARGET.count('"No Available Target."') == 2
assert TARGET.count('"沒有可鎖定的目標。"') == 2
assert 'target_set_monster(mon);' in TARGET
assert 'monster_race_track(player->upkeep, mon->race);' in TARGET
assert 'health_track(player->upkeep, mon);' in TARGET
assert 'monster_is_obvious(m)' in TARGET
assert 'projectable(cave, player->grid, m->grid, PROJECT_NONE)' in TARGET
assert '!player->timed[TMD_IMAGE]' in TARGET
assert 'monster_is_obvious(mon)' in UI
assert 'if (p->wizard)' in UI
assert 'noise=%d, scent=%d' in UI
assert 'tests/native/test_target_monster_locale.c' in NATIVE
assert 'tests/test_target_monster_locale_source.py' in VERIFY
print("PASS: targeting display keeps monster visibility, health and combat grammar")
