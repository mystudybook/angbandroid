#!/usr/bin/env python3
"""FAangband direction and targeting must survive portrait/landscape changes.

Source-level contracts only; strict host C99 is run by test-fa-direction-target.sh.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
DIRECTION = (FA / "ui-input.c").read_text(encoding="utf-8")
TARGET = (FA / "ui-target.c").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def between(text, start, end):
    assert start in text and end in text, (start, end)
    return text.split(start, 1)[1].split(end, 1)[0]


def contains(text, *fragments):
    for fragment in fragments:
        assert fragment in text, fragment


direction = between(DIRECTION,
    "static bool textui_get_rep_dir(int *dp, bool allow_5)",
    ' * Get an "aiming direction"')
contains(direction,
    "char visible[256];",
    "if (ke.type == EVT_RESIZE) continue;",
    "angdroid_menu_copy_columns(visible, sizeof(visible), prompt,",
    "ke = inkey_ex();",
    "if (width < 3)",
    'prt("", 0, 0);',
    "bool resized = false;",
    "inkey_scan = SCAN_INSTANT;",
    "inkey_scan = player->opts.lazymove_delay;",
    "inkey_scan = SCAN_OFF;",
    "dir = 0;",
    "resized = true;",
    "if (!dir && !resized) bell();")
assert direction.count("if (ke.type == EVT_RESIZE) continue;") == 2
assert direction.index("angdroid_menu_copy_columns(") < direction.index(
    "prt(visible, 0, 0);")
assert direction.index("ke = inkey_ex();\n                inkey_scan = SCAN_OFF;") < (
    direction.index("resized = true;"))
assert 'prt(angdroid_game_ui_text(' not in direction
assert "dir_transitions[dir][this_dir]" in direction
assert "pathfind_direction_to(from, to)" in direction
assert "(*dp) = dir;" in direction
assert "if (dir == 5 && !allow_5)" in direction

contains(TARGET, '#include "../angband/menu-text-fit.h"')
footer = between(TARGET, "static void fa_target_draw_short_help(int row)",
                 "void target_display_help(bool monster, bool object, bool free)")
contains(footer,
    "if (!Term || Term->hgt <= 0 || row < 0 || row >= Term->hgt)",
    "int width = Term ? Term->wid - 1 : 0;",
    "angdroid_menu_copy_columns(visible, sizeof(visible), prompt,",
    "prt(visible, row, 0);",
    'prt("", row, 0);')
assert footer.index("angdroid_menu_copy_columns(") < footer.index(
    "prt(visible, row, 0);")
target = TARGET.split("bool target_set_interactive(int mode, int x, int y)", 1)[1]
contains(target,
    "help_prompt_loc = hgt > 0 ? hgt - 1 : 0;",
    "Term_get_size(&current_wid, &current_hgt);",
    "if (current_wid != wid || current_hgt != hgt)",
    "wid = current_wid;",
    "hgt = current_hgt;",
    "if (!help) fa_target_draw_short_help(help_prompt_loc);")
assert target.count("fa_target_draw_short_help(help_prompt_loc);") == 3
assert "target_set_monster(0);" in target
assert "soft_kbd_clear(true);" in target

direction_host = (ROOT / "tests/native/generate_fa_direction_input_host.py").read_text(
    encoding="utf-8")
target_host = (ROOT / "tests/native/generate_fa_target_footer_host.py").read_text(
    encoding="utf-8")
runner = (ROOT / "scripts/test-fa-direction-target.sh").read_text(
    encoding="utf-8")
contains(direction_host,
    "match.group(0)", "textui_get_rep_dir", "SCAN_INSTANT",
    "EVT_RESIZE", "inkey_scan == SCAN_OFF", "No",
    "A resize while waiting for lazy second key")
contains(target_host,
    "found.group(0)", "fa_target_draw_short_help", "invalid_row",
    "width = 1; width < 80", "Term = NULL;")
contains(runner,
    "generate_fa_direction_input_host.py",
    "generate_fa_target_footer_host.py",
    "-std=c99 -Wall -Wextra -Werror -pedantic")
contains(VERIFY,
    "test-fa-direction-target.sh",
    "test_faangband_direction_target_source.py")
print("PASS: FA direction and targeting source/rotation contracts")
