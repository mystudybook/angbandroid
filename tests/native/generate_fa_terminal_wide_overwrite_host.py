#!/usr/bin/env python3
"""C99 host regression for production FAangband terminal cell overwrite paths.

Extracts actual functions from faangband/ui-term.c. Only a tiny terminal
structure, input-free screen grid and cursor movement are mocked.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "app/src/main/cpp/faangband/ui-term.c").read_text(
    encoding="utf-8")


def extract(name):
    match = re.search(
        rf"^(?:static )?(?:void|errr) {re.escape(name)}"
        r"\([^)]*\)\n\{.*?^\}",
        SOURCE, flags=re.MULTILINE | re.DOTALL)
    if not match:
        raise SystemExit(f"Missing production terminal function {name}()")
    return match.group(0)


# Keep declarations in dependency order: raw write, partner cleanup, public
# write, fast string write, direct draw, stream char, partial erase, inspect.
NAMES = (
    "Term_queue_char_raw", "Term_clear_wide_neighbor", "Term_queue_char",
    "Term_queue_chars", "Term_draw", "Term_addch", "Term_addstr",
    "Term_putch", "Term_putstr", "Term_erase", "Term_what",
)
ACTUAL = [extract(name) for name in NAMES]

HEADER = r"""
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "app/src/main/cpp/curses/unicode_layout.h"
#include "app/src/main/cpp/curses/unicode_display.h"

#define COLOUR_WHITE 1
typedef int errr;
typedef struct terminal_grid {
    int *a[4], *ta[4];
    wchar_t *c[4], *tc[4];
    int cx, cy, cu;
} terminal_grid;
typedef struct term {
    int wid, hgt, y1, y2;
    int x1[4], x2[4];
    int (*dblh_hook)(int, wchar_t);
    terminal_grid *scr, *old;
} term;

static term screen;
static term *Term = &screen;
static terminal_grid active, previous;
static int attrs[4][20], terrains[4][20], old_attrs[4][20];
static int old_terrains[4][20];
static wchar_t chars[4][20], tile_chars[4][20];
static wchar_t old_chars[4][20], old_tile_chars[4][20];
static int tile_height = 1;

static void text_mbstowcs(wchar_t *dst, const char *src, int capacity)
{
    size_t offset = 0;
    int n = 0;
    while (src[offset] && n < capacity - 1) {
        uint32_t cp = 0;
        size_t bytes = angdroid_utf8_next_scalar(src + offset, &cp);
        if (!bytes) break;
        dst[n++] = (wchar_t)cp;
        offset += bytes;
    }
    dst[n] = 0;
}

static errr Term_gotoxy(int x, int y)
{
    if (x < 0 || x >= Term->wid || y < 0 || y >= Term->hgt)
        return -1;
    active.cx = x; active.cy = y; active.cu = 0;
    return 0;
}
static void reset(int width)
{
    int x, y;
    memset(&screen, 0, sizeof(screen));
    memset(&active, 0, sizeof(active));
    memset(&previous, 0, sizeof(previous));
    memset(attrs, 0, sizeof(attrs));
    memset(terrains, 0, sizeof(terrains));
    memset(old_attrs, 0, sizeof(old_attrs));
    memset(old_terrains, 0, sizeof(old_terrains));
    memset(chars, 0, sizeof(chars));
    memset(tile_chars, 0, sizeof(tile_chars));
    memset(old_chars, 0, sizeof(old_chars));
    memset(old_tile_chars, 0, sizeof(old_tile_chars));
    screen.wid = width; screen.hgt = 4; screen.scr = &active;
    screen.old = &previous;
    screen.y1 = 4; screen.y2 = -1;
    for (y = 0; y < 4; ++y) {
        screen.x1[y] = width; screen.x2[y] = -1;
        active.a[y] = attrs[y]; active.c[y] = chars[y];
        active.ta[y] = terrains[y]; active.tc[y] = tile_chars[y];
        previous.a[y] = old_attrs[y];
        previous.c[y] = old_chars[y];
        previous.ta[y] = old_terrains[y];
        previous.tc[y] = old_tile_chars[y];
        for (x = 0; x < 20; ++x) {
            attrs[y][x] = old_attrs[y][x] = COLOUR_WHITE;
            chars[y][x] = old_chars[y][x] = L' ';
        }
    }
}
static void set_wide(int x)
{
    /* The FA prose painter inserts the continuation first. */
    Term_queue_char(Term, x + 1, 0, 1, ANGDROID_TEXT_CONTINUATION, 0, 0);
    Term_queue_char(Term, x, 0, 1, L'中', 0, 0);
}
"""
TEST = r"""
static unsigned long checks;
#define CHECK(cond) do { if (!(cond)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #cond); \
    return 1; } ++checks; } while (0)

int main(void)
{
    int attr = 0;
    wchar_t ch = 0;
    wchar_t input[] = {L'A', L'B', L'\0'};
    wchar_t overwrite[] = {L'Z', L'\0'};

    reset(12);
    set_wide(3);
    CHECK(chars[0][3] == L'中');
    CHECK(chars[0][4] == ANGDROID_TEXT_CONTINUATION);
    Term_queue_char(Term, 4, 0, 1, L'X', 0, 0);
    CHECK(chars[0][3] == L' ' && chars[0][4] == L'X');
    CHECK(screen.x1[0] <= 3 && screen.x2[0] >= 4);

    reset(12);
    set_wide(3);
    Term_queue_char(Term, 3, 0, 1, L'Z', 0, 0);
    CHECK(chars[0][3] == L'Z' && chars[0][4] == L' ');

    reset(12);
    set_wide(3);
    set_wide(3);
    CHECK(chars[0][3] == L'中');
    CHECK(chars[0][4] == ANGDROID_TEXT_CONTINUATION);

    reset(12);
    set_wide(3);
    Term_queue_chars(4, 0, 1, 1, overwrite);
    CHECK(chars[0][3] == L' ' && chars[0][4] == L'Z');
    CHECK(screen.x1[0] <= 3 && screen.x2[0] >= 4);

    reset(12);
    set_wide(3);
    Term_queue_chars(3, 0, 2, 1, input);
    CHECK(chars[0][3] == L'A' && chars[0][4] == L'B');

    reset(12);
    set_wide(3);
    CHECK(Term_what(4, 0, &attr, &ch) == 0);
    CHECK(ch == L' '); /* The public API must hide internal marker. */
    CHECK(chars[0][4] == ANGDROID_TEXT_CONTINUATION);

    reset(12);
    set_wide(3);
    CHECK(Term_erase(3, 0, 1) == 0);
    CHECK(chars[0][3] == L' ' && chars[0][4] == L' ');
    CHECK(screen.x2[0] >= 4);

    reset(12);
    set_wide(3);
    CHECK(Term_erase(4, 0, 1) == 0);
    CHECK(chars[0][3] == L' ' && chars[0][4] == L' ');
    CHECK(screen.x1[0] <= 3);

    reset(12);
    chars[0][5] = L'#';
    CHECK(Term_draw(3, 0, 1, L'中') == 0);
    CHECK(chars[0][3] == L'?' && chars[0][4] == L' ');
    CHECK(chars[0][5] == L'#');
    CHECK(Term_draw(4, 0, 0x80, (wchar_t)0xE0) == 0);
    CHECK(chars[0][4] == (wchar_t)0xE0);

    reset(12);
    CHECK(Term_addch(1, ANGDROID_TEXT_CONTINUATION) == 0);
    CHECK(chars[0][0] == L'?');
    CHECK(active.cx == 1);

    /* Text streams advance by physical cells, not wchar_t count.
     * Map writes through Term_draw()/Term_queue_chars() do not widen. */
    reset(12);
    CHECK(Term_addch(1, L'甲') == 0);
    CHECK(chars[0][0] == L'甲');
    CHECK(chars[0][1] == ANGDROID_TEXT_CONTINUATION);
    CHECK(active.cx == 2 && active.cu == 0);

    reset(12);
    CHECK(Term_addstr(-1, 1, "A中B") == 0);
    CHECK(chars[0][0] == L'A');
    CHECK(chars[0][1] == L'中');
    CHECK(chars[0][2] == ANGDROID_TEXT_CONTINUATION);
    CHECK(chars[0][3] == L'B');
    CHECK(active.cx == 4 && active.cu == 0);

    reset(4);
    CHECK(Term_addstr(-1, 1, "甲乙") == 2);
    CHECK(chars[0][0] == L'甲');
    CHECK(chars[0][1] == ANGDROID_TEXT_CONTINUATION);
    CHECK(chars[0][2] == L'乙');
    CHECK(chars[0][3] == ANGDROID_TEXT_CONTINUATION);
    CHECK(active.cx == 4 && active.cu == 1);

    reset(3);
    CHECK(Term_addstr(-1, 1, "甲乙") == 1);
    CHECK(chars[0][0] == L'甲');
    CHECK(chars[0][1] == ANGDROID_TEXT_CONTINUATION);
    CHECK(chars[0][2] == L' ' && active.cx == 2 && active.cu == 1);

    reset(12);
    CHECK(Term_addstr(2, 1, "A中B") == 0);
    CHECK(active.cx == 3 && chars[0][3] == L' ');
    CHECK(chars[0][1] == L'中' && chars[0][2] == ANGDROID_TEXT_CONTINUATION);

    reset(1);
    CHECK(Term_addch(1, L'中') == 1);
    CHECK(chars[0][0] == L'?' && active.cx == 1 && active.cu == 1);

    reset(3);
    CHECK(Term_gotoxy(2, 0) == 0);
    CHECK(Term_addch(1, L'中') == 1);
    CHECK(chars[0][2] == L'?' && active.cx == 3);

    reset(12);
    CHECK(Term_addch(0x80, L'中') == 0);
    CHECK(chars[0][0] == L'中' && chars[0][1] == L' ');
    CHECK(active.cx == 1);

    reset(12);
    CHECK(Term_addstr(-1, 1, "ABC") == 0);
    CHECK(chars[0][0] == L'A' && chars[0][1] == L'B');
    CHECK(chars[0][2] == L'C' && active.cx == 3);

    reset(12);
    {
        wchar_t w[] = {L'中', L'\0'};
        Term_queue_chars(3, 0, 1, 1, w);
        CHECK(chars[0][3] == L'?' && chars[0][4] == L' ');
    }

    /* Absolute-position text calls reuse the same width-aware stream
     * routine, while their input (x,y) is a physical terminal column. */
    reset(12);
    CHECK(Term_putstr(2, 0, -1, 1, "A中B") == 0);
    CHECK(chars[0][2] == L'A' && chars[0][3] == L'中');
    CHECK(chars[0][4] == ANGDROID_TEXT_CONTINUATION);
    CHECK(chars[0][5] == L'B' && active.cx == 6);
    CHECK(active.cy == 0 && active.cu == 0);

    reset(12);
    CHECK(Term_putch(4, 0, 1, L'甲') == 0);
    CHECK(chars[0][4] == L'甲');
    CHECK(chars[0][5] == ANGDROID_TEXT_CONTINUATION);
    CHECK(active.cx == 6);

    reset(12);
    CHECK(Term_putch(11, 0, 1, L'甲') == 1);
    CHECK(chars[0][11] == L'?' && active.cx == 12);
    CHECK(active.cu == 1);

    reset(12);
    CHECK(Term_putch(4, 0, 0x80, L'甲') == 0);
    CHECK(chars[0][4] == L'甲' && chars[0][5] == L' ');

    reset(12);
    CHECK(Term_putstr(10, 0, -1, 1, "A中") == 1);
    CHECK(chars[0][10] == L'A' && chars[0][11] == L' ');
    CHECK(active.cx == 11 && active.cu == 1);

    reset(12);
    CHECK(Term_putstr(-1, 0, -1, 1, "abc") == -1);
    CHECK(active.cx == 0 && chars[0][0] == L' ');

    reset(12);
    set_wide(3);
    Term_queue_char(NULL, 3, 0, 1, L'x', 0, 0);
    CHECK(chars[0][3] == L'中');

    reset(12);
    set_wide(9);
    CHECK(Term_erase(9, 0, 1) == 0);
    CHECK(chars[0][9] == L' ' && chars[0][10] == L' ');

    printf("PASS: %lu FA native CJK overwrites and partial erase assertions\n",
        checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_terminal_wide_overwrite_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    HEADER + "\n" + "\n\n".join(ACTUAL) + "\n" + TEST,
    encoding="utf-8")
