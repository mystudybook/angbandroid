#!/usr/bin/env python3
"""Guard FAangband's native partial wide-cell overwrite handling.

Native C99 execution compiles extracted production functions via
scripts/test-fa-terminal-wide-overwrite.sh. No game rules or files change.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "app/src/main/cpp/faangband/ui-term.c").read_text(
    encoding="utf-8")
HOST = (ROOT / "tests/native/generate_fa_terminal_wide_overwrite_host.py").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-fa-terminal-wide-overwrite.sh").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")


def section(start, end):
    assert start in SOURCE and end in SOURCE, (start, end)
    return SOURCE.split(start, 1)[1].split(end, 1)[0]


assert '#include "../curses/unicode_layout.h"' in SOURCE
raw = section("static void Term_queue_char_raw(", "static void Term_clear_wide_neighbor(")
cleanup = section("static void Term_clear_wide_neighbor(", "void Term_queue_char(")
queued = section("void Term_queue_char(", "void Term_big_queue_char(")
fast = section("void Term_queue_chars(", "static void Term_fresh_row_pict(")
draw = section("errr Term_draw(", "errr Term_addch(")
stream = section("errr Term_addch(", "errr Term_addstr(")
erase = section("errr Term_erase(", "errr Term_clear(")
what = section("errr Term_what(", "errr Term_flush(")

assert "Term_queue_char_raw(t, partner, y, COLOUR_WHITE, L' ', 0, 0);" in cleanup
assert "angdroid_wide_partner_column(t->wid, x," in cleanup
assert "t->scr->c[y][x] == ANGDROID_TEXT_CONTINUATION" in cleanup
assert "!angdroid_is_wide_text(incoming, attr)" in cleanup
assert "if (partner < 0) return;" in cleanup
assert "if (!t || !t->scr || x < 0 || y < 0 ||" in queued
assert "Term_clear_wide_neighbor(t, x, y, c, a);" in queued
assert "Term_queue_char_raw(t, x, y, a, c, ta, tc);" in queued
assert queued.index("Term_clear_wide_neighbor(") < (
    queued.index("Term_queue_char_raw("))
assert "t->dblh_hook" in raw
assert "if (!ta) ta = ota;" in raw and "if (!tc) tc = otc;" in raw
assert "Term_clear_wide_neighbor(Term, x, y, glyph, a);" in fast
assert "wchar_t glyph = angdroid_fixed_cell_glyph(*s, a);" in fast
assert "angdroid_fixed_cell_glyph(c, a)" in draw
assert "c = angdroid_stream_glyph(c);" in stream
assert "span = angdroid_is_wide_text(c, a) ? 2 : 1;" in stream
assert "if (x + span > w)" in stream
assert "Term_queue_char(Term, x + 1, y, a," in stream
assert "ANGDROID_TEXT_CONTINUATION, 0, 0);" in stream
assert "Term->scr->cx += span;" in stream
assert "Term->scr->cx++;" not in stream
block = section("errr Term_addstr(", "errr Term_putch(")
assert "angdroid_fit_terminal_text(s, (size_t)count," in block
assert "fit = angdroid_fit_terminal_text(s, (size_t)count," in block
assert "res = Term_addch(a, s[i]);" in block
assert "Term_queue_chars(" not in block
assert "text_mbstowcs(s, buf, 1023);" in block
assert "s[1023] = L'\\0';" in block
assert "If a partial erase touches either half" in erase
assert "Term_clear_wide_neighbor(Term, column, y, L' ', COLOUR_WHITE);" in erase
assert erase.index("Term_clear_wide_neighbor(Term, column") < (
    erase.index("/* Scan every column */"))
assert "angdroid_public_grid_glyph(Term->scr->c[y][x])" in what

for item in ("Term_queue_char_raw", "Term_clear_wide_neighbor",
             "Term_queue_char", "Term_queue_chars", "Term_draw",
             "Term_addch", "Term_addstr", "Term_erase", "Term_what"):
    assert f'"{item}"' in HOST, item
for expected in (
    "set_wide(3);",
    "Term_queue_char(Term, 4, 0, 1, L'X', 0, 0);",
    "Term_queue_chars(4, 0, 1, 1, overwrite);",
    "Term_erase(4, 0, 1)",
    "Term_erase(3, 0, 1)",
    "Term_what(4, 0, &attr, &ch)",
    "Term_draw(3, 0, 1, L'中')",
    "screen.x1[0] <= 3 && screen.x2[0] >= 4",
    'Term_addstr(-1, 1, "A中B")',
    'Term_addstr(-1, 1, "甲乙")',
    "Term_addstr(2, 1,",
    "Term_addch(1, L'甲')",
    "active.cx == 4 && active.cu == 0",
    "angdroid_utf8_next_scalar(",

):
    assert expected in HOST, expected
assert "void Term_queue_char(NULL" not in HOST
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "generate_fa_terminal_wide_overwrite_host.py" in RUNNER
assert "test-fa-terminal-wide-overwrite.sh" in VERIFY
assert "test_fa_terminal_wide_overwrite_source.py" in VERIFY

print("PASS: FAangband CJK partner cells, partial erase and map safety")
