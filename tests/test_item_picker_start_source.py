#!/usr/bin/env python3
"""Angband/FAangband start with an allowed populated item tab.

Check real source integration. The standalone C99 test executes the actual
selection policy for all four-source eligibility/visibility combinations.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
SHARED = (CPP / "angband/item-picker-start-source.h").read_text(
    encoding="utf-8"
)
NATIVE = (ROOT / "tests/native/test_item_picker_start_source.c").read_text(
    encoding="utf-8"
)
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for label in (
    "ANGDROID_PICKER_INVEN",
    "ANGDROID_PICKER_EQUIP",
    "ANGDROID_PICKER_QUIVER",
    "ANGDROID_PICKER_FLOOR",
    "angdroid_item_picker_initial_source(",
    "options->allowed & options->nonempty & 0x0fu",
    "options->current == options->source[i]",
    "options->prefer_quiver",
    "selectable & options->requested",
):
    assert label in SHARED, label

for game in ("angband", "faangband"):
    src = (CPP / game / "ui-object.c").read_text(encoding="utf-8")
    inc = ('#include "item-picker-start-source.h"'
           if game == "angband"
           else '#include "../angband/item-picker-start-source.h"')
    assert inc in src, game
    picker = src.split("bool textui_get_item(", 1)[1].split(
        "/* Clean up */", 1
    )[0]
    start = picker.split("/* Require at least one legal choice */", 1)[1].split(
        "while (true) {", 1
    )[0]

    # Throwing selection still takes priority; a permitted already-populated
    # source is kept, but a permitted empty source cannot hide a nonempty one.
    assert "if (show_throwing && throwing_num)" in start
    assert "player->upkeep->command_wrk = SHOW_THROWING;" in start
    assert "angdroid_item_picker_start_options start =" in start
    assert "angdroid_item_picker_initial_source(&start)" in start
    assert start.index("if (show_throwing && throwing_num)") < start.index(
        "angdroid_item_picker_initial_source(&start)"
    )
    for tag in (
        "(allow_inven ? ANGDROID_PICKER_INVEN : 0u)",
        "(allow_equip ? ANGDROID_PICKER_EQUIP : 0u)",
        "(allow_quiver ? ANGDROID_PICKER_QUIVER : 0u)",
        "(allow_floor ? ANGDROID_PICKER_FLOOR : 0u)",
        "(i1 <= i2 ? ANGDROID_PICKER_INVEN : 0u)",
        "(equip_has_item ? ANGDROID_PICKER_EQUIP : 0u)",
        "(q1 <= q2 ? ANGDROID_PICKER_QUIVER : 0u)",
        "(f1 <= f2 ? ANGDROID_PICKER_FLOOR : 0u)",
        "(use_inven ? ANGDROID_PICKER_INVEN : 0u)",
        "(use_equip ? ANGDROID_PICKER_EQUIP : 0u)",
        "(use_quiver ? ANGDROID_PICKER_QUIVER : 0u)",
        "(use_floor ? ANGDROID_PICKER_FLOOR : 0u)",
        "quiver_tags && cmd != CMD_USE",
    ):
        assert tag in start, (game, tag)

    # CMD_NULL allows displaying empty equipment slots; do not confuse
    # those labels with actual eligible equipment objects.
    assert "bool equip_has_item = false;" in picker
    equipment = picker.split("/* Accept equipment */", 1)[1].split(
        "/* Restrict quiver indexes */", 1
    )[0]
    assert "if (allow_equip)" in equipment
    assert "slot_object(player, equipped_slot)" in equipment
    assert "if (equipped && object_test(tester, equipped))" in equipment
    assert "equip_has_item = true;" in equipment
    assert "if ((e1 <= e2) || allow_all)" in equipment

    # No dummy items. Leave existing empty-tab navigation and cleanup intact.
    assert "menu_setpriv(m, 1," not in src
    assert "wipe_obj_list();" in picker
    assert "screen_load();" in picker

for needle in (
    "all_availability_combinations();",
    "important_user_scenarios();",
    "CHECK(angdroid_item_picker_initial_source(&opts) == 22);",
    "CHECK(angdroid_item_picker_initial_source(&opts) == 44);",
    "CHECK(angdroid_item_picker_initial_source(&opts) == 33);",
    "CHECK(angdroid_item_picker_initial_source(NULL) == 0);",
):
    assert needle in NATIVE, needle

assert "test_item_picker_start_source.c" in RUNNER
assert "test_item_picker_start_source.py" in VERIFY
print("PASS: two variants choose a populated initial tab, retain fallback")
