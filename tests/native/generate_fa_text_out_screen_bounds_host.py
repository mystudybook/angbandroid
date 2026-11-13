#!/usr/bin/env python3
"""Compile FAangband's real legacy text_out_to_screen() with strict C99 mocks.

Validate terminal geometry after rotation, constrained 256-cell scratch
arrays, bottom-edge scrolling, and the exact-wrap column erase guard.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "app/src/main/cpp/faangband/ui-output.c").read_text(
    encoding="utf-8")
match = re.search(
    r"^void text_out_to_screen\(uint8_t a, const char \*str\)\n\{.*?^\}",
    source, flags=re.MULTILINE | re.DOTALL)
if not match:
    raise SystemExit("Missing production text_out_to_screen()")

header = r"""
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
struct fake_term { int wid, hgt; };
static struct fake_term device = {80, 24};
static struct fake_term *Term = &device;
static int text_out_wrap, text_out_indent, text_out_pad;
static int xcur, ycur, invalid, erases, writes, width_override;
static wchar_t tiles[300][512];

static void Term_get_size(int *w, int *h)
{
    *w = Term->wid;
    *h = Term->hgt;
}
static int Term_locate(int *x, int *y)
{
    if (width_override) return -1;
    *x = xcur; *y = ycur;
    return (xcur >= 0 && xcur < Term->wid &&
        ycur >= 0 && ycur < Term->hgt) ? 0 : -1;
}
static int Term_gotoxy(int x, int y)
{
    if (!Term || x < 0 || y < 0 || x >= Term->wid || y >= Term->hgt) {
        ++invalid;
        return -1;
    }
    xcur = x;
    ycur = y;
    return 0;
}
static int Term_erase(int x, int y, int n)
{
    int i;
    if (!Term || x < 0 || y < 0 || x >= Term->wid || y >= Term->hgt) {
        ++invalid;
        return -1;
    }
    ++erases;
    for (i = x; i < Term->wid && i < x + n; ++i)
        tiles[y][i] = L' ';
    xcur = x;
    ycur = y;
    return 0;
}
static int Term_what(int x, int y, int *color, wchar_t *ch)
{
    if (x < 0 || x >= Term->wid || y < 0 || y >= Term->hgt) {
        ++invalid; return -1;
    }
    *color = 1;
    *ch = tiles[y][x];
    return 0;
}
static int Term_addch(int color, wchar_t ch)
{
    (void)color;
    if (!Term || xcur < 0 || xcur >= Term->wid ||
        ycur < 0 || ycur >= Term->hgt) {
        ++invalid; return -1;
    }
    ++writes;
    tiles[ycur][xcur] = ch;
    ++xcur;
    return 0;
}
static int text_iswprint(wchar_t ch)
{
    return ch >= 32 && ch != 127;
}
static int text_mbstowcs(wchar_t *to, const char *from, int cap)
{
    int i = 0;
    while (from[i] && i < cap - 1) {
        to[i] = (unsigned char)from[i];
        ++i;
    }
    to[i] = L'\0';
    return i;
}
static void reset(int width, int height)
{
    int i, j;
    Term = &device;
    device.wid = width;
    device.hgt = height;
    text_out_wrap = text_out_indent = text_out_pad = 0;
    xcur = ycur = invalid = erases = writes = width_override = 0;
    for (i = 0; i < 300; ++i)
        for (j = 0; j < 512; ++j)
            tiles[i][j] = L' ';
}
"""
tests = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    char long_text[400];
    int i;

    reset(15, 4);
    text_out_to_screen(1, "A\nB\nC\nD\nE\nF");
    CHECK(invalid == 0 && writes == 4);
    CHECK(tiles[3][0] == L'D');

    reset(4, 5);
    text_out_wrap = 4;
    text_out_to_screen(1, "a bc def ghij");
    CHECK(invalid == 0 && erases > 0);

    /* A word exactly at the rightmost cell yields n==wrap.
     * There is no valid column to clear beyond the terminal. */
    reset(4, 5);
    text_out_to_screen(1, "abcdEF");
    CHECK(invalid == 0);

    reset(400, 3);
    for (i = 0; i < 350; ++i) long_text[i] = 'x';
    long_text[350] = '\0';
    text_out_to_screen(1, long_text);
    CHECK(invalid == 0 && writes > 0);

    reset(1, 2);
    text_out_to_screen(1, "test\nhi");
    CHECK(invalid == 0 && writes == 0);

    reset(12, 4);
    width_override = 1;
    text_out_to_screen(1, "invalid cursor");
    CHECK(invalid == 0 && writes == 0);

    reset(12, 4);
    Term = NULL;
    text_out_to_screen(1, "rotation");
    CHECK(invalid == 0 && writes == 0);

    reset(12, 4);
    text_out_to_screen(1, NULL);
    CHECK(invalid == 0 && writes == 0);

    printf("PASS: %lu production FA text_out_to_screen bounds checks\n",
           checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_text_out_screen_bounds_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + match.group(0) + "\n" + tests, encoding="utf-8")
