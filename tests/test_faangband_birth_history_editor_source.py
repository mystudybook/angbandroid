#!/usr/bin/env python3
"""FAangband character-history UTF-8 scrolling and world-map chooser guards.

This is a source contract, not an Android build. Native wrap correctness is
also covered by the independently compilable C99 history test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
UI = (FA / "ui-birth.c").read_text(encoding="utf-8")
HEADER = (FA / "ui-birth-history-utf8.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def need(source, *terms):
    for term in terms:
        assert term in source, term


def section(source, start, end):
    assert start in source and end in source, (start, end)
    return source.split(start, 1)[1].split(end, 1)[0]


# The source history stays in the original buffer; offsets and insertions use
# Unicode scalar indices, while display and wrapping use terminal columns.
need(UI,
    '#include "../angband/ui-input-utf8.h"',
    '#include "ui-birth-history-utf8.h"',
    'static region fa_birth_history_area(void)',
    'area.width = MIN(71, MAX(1, Term->wid - area.col - 1));',
    'area.page_rows = MIN(5, MAX(1, Term->hgt - area.row));',
    'angdroid_input_prefill_copy(history, sizeof(history),',
    'cmdq_push(CMD_HISTORY_CHOICE);',
    'cmd_set_arg_string(cmdq_peek(), "history", history);')
editor = section(UI, "static int edit_text(", "static enum birth_stage get_history_command(")
need(editor,
    "angdroid_fa_history_wrap(buffer, (size_t)area.width,",
    "angdroid_fa_history_copy_line(original, sizeof(original),",
    "angdroid_menu_copy_columns(visible, sizeof(visible),",
    "size_t line_starts[256], line_lengths[256];",
    "first_visible = y >= area.page_rows ?",
    "area.row + y - (int)first_visible",
    "angdroid_input_cursor_columns(buffer,",
    "get_screen_loc((size_t)cursor, &x, &y, n_lines,",
    "n_lines && y >= 0 && (size_t)(y + 1) < n_lines",
    "n_lines && y > 0 && (size_t)y < n_lines",
    "case ESCAPE:",
    "case KC_BACKSPACE:",
    "case KC_DELETE:",
    "case KC_ENTER:",
    "utf8_fskip(buffer, cursor, NULL);",
    "utf8_rskip(ocurs, 1, buffer);",
    "utf32_to_utf8(encoded,",
    "buffer[len] = '\\0';")
assert "textblock_calculate_lines(" not in editor
assert "textui_textblock_place(" not in editor
assert "textblock_new()" not in editor
assert "mem_free(line_starts);" not in editor
assert "Term_gotoxy(1 + x, 19 + y);" not in editor
assert "get_screen_loc(" in editor

# The helper advances by UTF-8 codepoints, rejects byte/column confusion,
# recognizes explicit newlines, and cannot underflow in one-cell terminals.
need(HEADER,
    "angdroid_utf8_next_scalar(text + offset, &cp)",
    "angdroid_terminal_columns(cp)",
    "if (cp == '\\n')",
    "col >= width || glyph_cols > width - col",
    "scalar_pos > starts[count - 1]",
    "if (count == capacity) break;",
    "angdroid_fa_history_copy_line(",
    "if (used + bytes >= capacity) break;",
    "out[used] = '\\0';")

# Only the confirmed map item ID may be enqueued. Reading '?' help is
# informational and must not implicitly accept or cancel the map chooser.
map_handler = section(UI, "static bool map_menu_handler(",
                      "static void map_menu_browser(")
need(map_handler,
    'show_file("mapmode.txt", NULL, 0, 0);',
    "return true;",
    "current_map_index = oid;",
    "current_map_index = -1;")
need(UI,
    "static void fa_birth_map_menu_resize(struct menu *m)",
    "fa_birth_map_menu_resize /* rotate or resize */",
    "cmdq_push(CMD_CHOOSE_MAP);",
    "ui_event result;",
    "result = menu_select(m, 0, true);",
    "if (result.type == EVT_SELECT && current_map_index >= 0 &&",
    "confirmed = current_map_index;",
    "current_map_index = confirmed;",
    'cmd_set_arg_choice(cmdq_peek(), "choice", map);')
assert 'cmd_set_arg_choice(cmdq_peek(), "choice",\n\t\t\t\t\tm->cursor);' not in UI

# Translated quickstart and history top-line prompts fit terminal cells.
need(UI,
    "static void fa_birth_prompt_left(const char *message, int row)",
    'fa_birth_prompt_left(angdroid_game_ui_text(',
    "message, (size_t)Term->wid);")

assert "test-fa-birth-history-utf8.sh" in RUNNER
assert "test_faangband_birth_history_editor_source.py" in RUNNER
print("PASS: FA CJK history editor scroll, cancel, map choice and resize contracts")
