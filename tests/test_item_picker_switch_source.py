#!/usr/bin/env python3
"""Item picker: only actionable tab switch keys, and FA CJK-safe prompts.

The switch-key helper is also tested by a native C99 program. This file
checks engine integration, not Android phone end-to-end behavior.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
SHARED = (CPP / "angband/item-picker-switch-keys.h").read_text(
    encoding="utf-8")
ZH = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_picker_switch_keys.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert "static inline void angdroid_item_picker_switch_keys(" in SHARED
for key in ("keys[count++] = '/';", "keys[count++] = '|';",
            "keys[count++] = '-';", "keys[count] = '\\0';"):
    assert key in SHARED, key
assert "if (!keys)" in SHARED

for variant in ("angband", "faangband"):
    path = CPP / variant / "ui-object.c"
    src = path.read_text(encoding="utf-8")
    inc = ('#include "item-picker-switch-keys.h"'
           if variant == "angband"
           else '#include "../angband/item-picker-switch-keys.h"')
    assert inc in src
    picker = src.split("static struct object *item_menu(", 1)[1].split(
        "/**\n * Let the user select an object", 1
    )[0]
    assert "char switch_keys[4];" in picker
    assert "angdroid_item_picker_switch_keys(" in picker
    assert "m->switch_keys = switch_keys;" in picker
    assert 'm->switch_keys = "/|-";' not in picker
    for part in (
        "player->upkeep->command_wrk != USE_INVEN",
        "player->upkeep->command_wrk != USE_EQUIP",
        "player->upkeep->command_wrk != USE_QUIVER",
        "player->upkeep->command_wrk != USE_FLOOR",
        "((q1 <= q2) || allow_all)",
        "((f1 <= f2) || allow_all)",
        "(item_mode & USE_INVEN)",
        "(item_mode & USE_EQUIP)",
    ):
        assert part in picker, (variant, part)
    assert picker.index("angdroid_item_picker_switch_keys(") < picker.index(
        "evt = menu_select(m, 0, true);")
    # An allowed tab switch must reopen a new item list; an Esc must not.
    assert "if (evt.type == EVT_SWITCH && !newmenu)" in picker

fa = (CPP / "faangband/ui-object.c").read_text(encoding="utf-8")
assert '#include "../angband/zh-tw-ui.h"' in fa
assert "static char header[288];" in fa
menu_header = fa.split("static void menu_header(void)", 1)[1].split(
    "/**\n * Get an item tag", 1
)[0]
for original in ("Inven:", "Equip:", "Quiver:",
                 "Floor:", "Throwing items:", " ESC"):
    assert f'angdroid_game_ui_text("{original}"' in menu_header, original
    assert f'{{"{original}",' in ZH, original
assert "char out_val[256];" in menu_header

item_picker = fa.split("bool textui_get_item(", 1)[1].split(
    "/**\n * ------------------------------------------------------------------------\n * Object recall", 1
)[0]
assert "size_t left = angdroid_menu_copy_columns(" in item_picker
assert "int second_col = (int)left + 1;" in item_picker
assert "if (second_col < Term->wid)" in item_picker
assert "angdroid_menu_copy_columns(clipped, sizeof(clipped)," in item_picker
assert "(int)angdroid_utf8_display_columns(pmt)" in item_picker
assert "prt(header, 0, strlen(pmt) + 1);" not in item_picker

for marker in ("available_tab_combinations();",
               "no_unusable_tab_is_advertised();",
               "mask < 8", "strcmp(keys, expected)"):
    assert marker in NATIVE, marker
assert "test_item_picker_switch_keys.c" in RUN
assert "test_item_picker_switch_source.py" in VERIFY

print("PASS: item picker tab switches and FA bilingual CJK-safe prompts")
