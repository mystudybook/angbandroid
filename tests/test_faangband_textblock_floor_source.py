#!/usr/bin/env python3
"""FAangband textblock pager and floor selection resize-safe source contracts.

The paired host C99 tests in scripts/test-fa-textblock-floor.sh compile
the actual production functions extracted from ui-output.c and ui-target.c.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
OUTPUT = (FA / "ui-output.c").read_text(encoding="utf-8")
TARGET = (FA / "ui-target.c").read_text(encoding="utf-8")
OBJECT = (FA / "ui-object.c").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def section(source, start, end):
    assert start in source and end in source, (start, end)
    return source.split(start, 1)[1].split(end, 1)[0]


def require(source, *parts):
    for part in parts:
        assert part in source, part


area = section(OUTPUT, "static void display_area(",
               "void textui_textblock_place(")
require(area,
        "if (!Term || area.width <= 0 || area.page_rows <= 0",
        "line_from < n_lines ? n_lines - line_from : 0",
        "if (!text || !attrs || !line_starts || !line_lengths) available = 0;",
        "Term_erase(area.col, area.row + (int)i, area.width);",
        "angdroid_terminal_columns(cp)",
        "if (width > (size_t)area.width - cells) break;")
assert "line_lengths[line_from + i]" in area

pager = section(OUTPUT, "struct keypress textui_textblock_show(",
                " * text_out hook for screen display")
require(pager, "screen_save();", "screen_load();",
        "input = inkey_ex();", "if (input.type == EVT_RESIZE",
        "full = region_calculate(orig_area);",
        "if (term_width <= 1 || term_height <= 0)",
        "full.width = MAX(0, MIN(full.width, term_width - full.col - 1));",
        "full.page_rows = MAX(0, MIN(full.page_rows, term_height - full.row));",
        "show_header = header && full.page_rows >= 3;",
        "body_rows = full.page_rows - (show_header ? 1 : 0) -",
        "n_lines = textblock_calculate_display_lines(",
        "if (layout_width != area.width)",
        "if (n_lines <= (size_t)body_rows)",
        "start_line = n_lines - (size_t)body_rows;",
        "pager = n_lines > (size_t)body_rows;",
        "start_line += remaining < step ? remaining : step;",
        "mem_free(line_starts);", "mem_free(line_lengths);")
assert "ch = inkey();" not in pager
assert "area.page_rows -= 2;" not in pager
assert "n_lines - area.page_rows" not in pager
assert "textblock_calculate_lines(" not in pager

# Only native screen textblocks use display-cell wrapping. File and
# export layout keeps the original wchar-oriented contract unchanged.
TEXTCORE = (FA / "z-textblock.c").read_text(encoding="utf-8")
TEXTHEAD = (FA / "z-textblock.h").read_text(encoding="utf-8")
require(TEXTCORE,
        '#include "../curses/unicode_width.h"',
        "size_t textblock_calculate_display_lines(",
        "angdroid_terminal_columns(",
        "if (columns > width - cells) break;",
        "if (cursor == begin) ++cursor;",
        "if (begin < length && text[begin] == L'\\n') ++begin;")
require(TEXTHEAD, "size_t textblock_calculate_display_lines(")
assert "size_t textblock_calculate_lines(" in TEXTCORE
assert "n_lines = textblock_calculate_lines(" in TEXTCORE


normalize = section(OUTPUT, "static struct keypress fa_textblock_key_from_event(",
                    "static void fa_textblock_print_bounded(")
require(normalize, "event.type == EVT_KBRD || event.type == EVT_BUTTON",
        "event.type == EVT_ESCAPE || event.type == EVT_MOUSE",
        "event.mouse.button == 1", "key.type = EVT_KBRD;")
footer = section(OUTPUT, "static void fa_textblock_print_bounded(",
                 "struct keypress textui_textblock_show(")
require(footer, "angdroid_menu_copy_columns(visible, sizeof(visible), label,",
        "c_put_str(COLOUR_L_BLUE, visible, row, col);")

floor_index = section(TARGET, "static int fa_target_floor_list_index(",
                      "static ui_event fa_target_floor_list_inkey(")
require(floor_index,
        "strchr(all_letters_nohjkl,",
        "press.type == EVT_MOUSE",
        "press.mouse.button != 1",
        "y < 1 || y >= height || y - 1 >= floor_count",
        "x < 0 || x >= width - 1",
        "press.key.code >= 33 && press.key.code <= 126",
        "return y - 1;")
assert "press.key.code - 'a'" not in floor_index
# show_floor() uses exactly this label map; h/j/k/l are intentionally skipped.
require(OBJECT, "all_letters_nohjkl[i]", "row = 1;", "show_obj_list(mode);")

floor_input = section(TARGET, "static ui_event fa_target_floor_list_inkey(",
                      "static bool aux_object(")
require(floor_input,
        "press = inkey_ex();",
        "if (press.type == EVT_RESIZE)",
        "if (!Term || Term->wid < 3 || Term->hgt < 2)",
        "show_floor(floor_list, floor_count,",
        "fa_target_draw_bounded_line(description, 0);",
        "press.key.code = ESCAPE;",
        "if (press.type == EVT_BUTTON)")
floor_call = section(TARGET, "static bool aux_object(",
                     "static bool aux_terrain(")
require(floor_call,
        "screen_save();", "screen_load();",
        "fa_target_floor_list_inkey(",
        "pos = fa_target_floor_list_index(auxst->press,",
        "if (0 <= pos && pos < floor_num)")
assert floor_call.index("pos = fa_target_floor_list_index(") < (
    floor_call.index("screen_load();"))
assert "pos = auxst->press.mouse.y - 1;" not in floor_call
assert "pos = auxst->press.key.code -" not in floor_call

pager_fixture = (ROOT / "tests/native/generate_fa_textblock_pager_host.py").read_text(
    encoding="utf-8")
floor_fixture = (ROOT / "tests/native/generate_fa_floor_list_input_host.py").read_text(
    encoding="utf-8")
runner = (ROOT / "scripts/test-fa-textblock-floor.sh").read_text(
    encoding="utf-8")
require(pager_fixture, "textui_textblock_show", "display_area",
        "extract_core(", "textblock_calculate_display_lines",
        "EVT_RESIZE", "check_column_lines()",
        "lines_allocated == lines_freed", "bad_coordinates == 0")
require(floor_fixture, "fa_target_floor_list_index",
        "fa_target_floor_list_inkey", "all_letters_nohjkl",
        "press.key.code = 'm';", "rendered == 2",
        "next == 1 && rendered == 0")
require(runner,
        "generate_fa_textblock_pager_host.py",
        "generate_fa_floor_list_input_host.py",
        "-std=c99 -Wall -Wextra -Werror -pedantic")
require(VERIFY, "test-fa-textblock-floor.sh",
        "test_faangband_textblock_floor_source.py")
print("PASS: FA textblock pager and floor list UI source contracts")
