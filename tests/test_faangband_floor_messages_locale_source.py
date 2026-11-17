#!/usr/bin/env python3
"""FAangband floor messages: localize visible strings, preserve scan + capacity."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SRC=(ROOT/"app/src/main/cpp/faangband/ui-display.c").read_text(
    encoding="utf-8")
RUN=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
body=SRC.split("static void see_floor_items(",1)[1].split(
    "static void ui_enter_init(",1)[0] if "static void ui_enter_init(" in SRC else (
    SRC.split("static void see_floor_items(",1)[1])
for token in (
    "scan_floor(floor_list, floor_max, player,",
    "OFLOOR_SENSE | OFLOOR_VISIBLE",
    "if (inven_carry_okay(floor_list[i]))",
    "if (floor_num == 1)",
    "if (!can_pickup)",
    "else if (blind)",
    'ODESC_PREFIX | ODESC_BASE |',
    'ODESC_PREFIX | ODESC_FULL |',
    '(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)',
    '"你沒有空間攜帶%s。"',
    '"你摸到%s。"',
    '"你看到%s。"',
    '"你沒有空間攜帶下列物品："',
    '"你感覺地上有下列物品："',
    '"你看到下列物品："',
    'msg("You %s %s.", p, o_name);',
    'prt(format("You %s: ", p), 0, 0);',
    "show_floor(floor_list, floor_num, OLIST_WEIGHT, NULL);",
    "e = inkey_ex();",
    "Term_event_push(&e);",
    "mem_free(floor_list);",
):
    assert token in body, token
assert "test_faangband_floor_messages_locale_source.py" in RUN
print("PASS: FA floor items translated, scan/capacity/events and English fallback preserved")
