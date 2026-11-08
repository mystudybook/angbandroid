#!/usr/bin/env python3
"""Keep monster-list hallucination and sort UI hints localized only on screen."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
MON = (CPP / "ui-mon-list.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-monster-list-ui.h").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
special = MON.split("static bool monster_list_format_special(", 1)[1].split(
    "static void monster_list_format_textblock(", 1)[0]
interactive = MON.split("void monster_list_show_interactive(", 1)[1].split(
    "void monster_list_force_subwindow_update(", 1)[0]
for required in (
    '#include "zh-tw-monster-list-ui.h"',
    "angdroid_monster_list_hallucination_text(zh_tw)",
    "angdroid_monster_list_hallucination_columns(zh_tw)",
    "angdroid_monster_list_sort_hint(",
):
    assert required in MON, required
assert "player->timed[TMD_IMAGE] > 0" in special
assert "textblock_append_c(tb, COLOUR_ORANGE, \"%s\", message)" in special
assert "*max_height_result = 1" in special
assert "angdroid_monster_list_sort_hint(" in interactive
assert 'soft_kbd_flash("x")' in interactive
assert "while (ch.code == 'x')" in interactive
assert "sort_exp = !sort_exp" in interactive
assert "angdroid_utf8_copy_complete(buf, sizeof(buf)" in interactive
assert "strlen(message)" not in special
assert "angdroid_utf8_display_columns(" in HEADER
assert '"Your hallucinations are too wild to see things clearly.\\n"' in HEADER
assert '"Press \'x\' to turn ON \'sort by exp\'"' in HEADER
assert 'test_monster_list_special_locale.c' in RUN
assert 'test_monster_list_special_locale_source.py' in VERIFY
print("PASS: monster-list special hint zh-TW source contract")
