#!/usr/bin/env python3
"""Compile the REAL FAangband Term_redraw_section() against C99 terminal mocks.

Catches: drawing only the continuation half, carrying a previous row's
expanded boundary into the next, inverted/off-screen rectangles, and
clobbering pending dirty bounds belonging to other updates.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "app/src/main/cpp/faangband/ui-term.c").read_text(
    encoding="utf-8")
match = re.search(
    r"^errr Term_redraw_section\(int x1, int y1, int x2, int y2\)\n"
    r"\{.*?^\}", SOURCE, flags=re.MULTILINE | re.DOTALL)
if not match:
    raise SystemExit("Missing real FAangband Term_redraw_section()")

HEADER = r"""
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "app/src/main/cpp/curses/unicode_layout.h"

typedef int errr;
typedef struct terminal_cells {
    int *a[4];
    wchar_t *c[4];
} terminal_cells;
typedef struct fake_term {
    int wid, hgt, y1, y2;
    int x1[4], x2[4];
    terminal_cells *scr, *old;
} fake_term;
static fake_term device;
static fake_term *Term = &device;
static terminal_cells current, previous;
static int now_attrs[4][16], before_attrs[4][16];
static wchar_t now_chars[4][16], before_chars[4][16];
static int fresh_calls;

static void Term_fresh(void) { ++fresh_calls; }

static void reset(void)
{
    int y, x;
    memset(&device, 0, sizeof(device));
    memset(&current, 0, sizeof(current));
    memset(&previous, 0, sizeof(previous));
    memset(now_attrs, 0, sizeof(now_attrs));
    memset(before_attrs, 0, sizeof(before_attrs));
    memset(now_chars, 0, sizeof(now_chars));
    memset(before_chars, 0, sizeof(before_chars));
    device.wid = 12; device.hgt = 4;
    device.scr = &current; device.old = &previous;
    device.y1 = device.hgt; device.y2 = -1;
    fresh_calls = 0;
    Term = &device;
    for (y = 0; y < 4; ++y) {
        device.x1[y] = device.wid;
        device.x2[y] = -1;
        current.a[y] = now_attrs[y];
        current.c[y] = now_chars[y];
        previous.a[y] = before_attrs[y];
        previous.c[y] = before_chars[y];
        for (x = 0; x < 12; ++x) {
            now_attrs[y][x] = 1;
            before_attrs[y][x] = 1;
            now_chars[y][x] = L' ';
            before_chars[y][x] = L'*';
        }
    }
}
"""
TEST = r"""
static int checks;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    /* A two-row redraw must preserve each row's independent bounds. */
    reset();
    now_chars[0][1] = L'中';
    now_chars[0][2] = ANGDROID_TEXT_CONTINUATION;
    CHECK(Term_redraw_section(2, 0, 3, 1) == 0);
    CHECK(device.x1[0] == 1 && device.x2[0] == 3);
    CHECK(device.x1[1] == 2 && device.x2[1] == 3);
    CHECK(device.y1 == 0 && device.y2 == 1 && fresh_calls == 1);
    CHECK(before_chars[0][1] == 0 && before_chars[0][2] == 0);
    CHECK(before_chars[1][1] == L'*' && before_chars[1][2] == 0);

    /* Including a wide lead must also redraw its trailing cell. */
    reset();
    now_chars[1][5] = L'甲';
    now_chars[1][6] = ANGDROID_TEXT_CONTINUATION;
    CHECK(Term_redraw_section(5, 1, 5, 1) == 0);
    CHECK(device.x1[1] == 5 && device.x2[1] == 6);
    CHECK(before_chars[1][5] == 0 && before_chars[1][6] == 0);

    /* Old glyphs matter too: the new screen may already be ASCII. */
    reset();
    before_chars[2][7] = L'乙';
    before_chars[2][8] = ANGDROID_TEXT_CONTINUATION;
    CHECK(Term_redraw_section(8, 2, 8, 2) == 0);
    CHECK(device.x1[2] == 7 && device.x2[2] == 8);
    CHECK(before_chars[2][7] == 0 && before_chars[2][8] == 0);

    /* Preserve legacy big-tile padding behavior (attribute 255). */
    reset();
    before_attrs[0][4] = 255;
    CHECK(Term_redraw_section(4, 0, 4, 0) == 0);
    CHECK(device.x1[0] == 3 && device.x2[0] == 4);
    CHECK(before_chars[0][3] == 0 && before_chars[0][4] == 0);

    /* Preserve pending unrelated dirty rows and earlier columns. */
    reset();
    device.y1 = 0; device.y2 = 2;
    device.x1[1] = 0; device.x2[1] = 1;
    CHECK(Term_redraw_section(5, 1, 6, 1) == 0);
    CHECK(device.y1 == 0 && device.y2 == 2);
    CHECK(device.x1[1] == 0 && device.x2[1] == 6);
    CHECK(before_chars[1][5] == 0 && before_chars[1][6] == 0);

    /* Partially outside the visible rectangle still clips safely. */
    reset();
    CHECK(Term_redraw_section(-50, -50, 2, 1) == 0);
    CHECK(device.y1 == 0 && device.y2 == 1);
    CHECK(device.x1[0] == 0 && device.x2[0] == 2);
    CHECK(before_chars[0][0] == 0 && before_chars[0][3] == L'*');

    reset();
    CHECK(Term_redraw_section(10, 2, 90, 99) == 0);
    CHECK(device.y1 == 2 && device.y2 == 3);
    CHECK(device.x1[3] == 10 && device.x2[3] == 11);

    /* Invalid rectangles must not call refresh or modify old cells. */
    reset();
    CHECK(Term_redraw_section(10, 0, 2, 1) == -1);
    CHECK(Term_redraw_section(0, 3, 1, 1) == -1);
    CHECK(Term_redraw_section(12, 0, 13, 1) == -1);
    CHECK(Term_redraw_section(0, -5, 3, -1) == -1);
    CHECK(Term_redraw_section(0, 4, 3, 8) == -1);
    CHECK(Term_redraw_section(-8, 0, -1, 1) == -1);
    CHECK(fresh_calls == 0 && before_chars[0][0] == L'*');
    CHECK(device.y1 == 4 && device.y2 == -1);

    reset();
    Term = NULL;
    CHECK(Term_redraw_section(0, 0, 1, 1) == -1);
    CHECK(fresh_calls == 0);

    printf("PASS: %d production FA redraw bounds/continuation checks\n", checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_redraw_section_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    HEADER + "\n" + match.group(0) + "\n" + TEST, encoding="utf-8")
