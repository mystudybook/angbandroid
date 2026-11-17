/* Pure C99 checks for cursor redraw arithmetic and two-cell CJK partners. */
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <wchar.h>
#include "../../app/src/main/cpp/curses/term-soft-cursor-bounds.h"
#include "../../app/src/main/cpp/curses/unicode_layout.h"

static unsigned long assertions;
#define CHECK(expr) do { assert(expr); ++assertions; } while (0)

static void test_cursor_spans(void)
{
    static const int limits[] = {INT_MIN, -2, 0, 1, 2, 3, 80, 1024, INT_MAX};
    static const int origins[] = {INT_MIN, -2, -1, 0, 1, 2, 79, 80, 1024, INT_MAX};
    static const int spans[] = {INT_MIN, -1, 0, 1, 2, 3, 80, INT_MAX};
    size_t l, x, s;
    for (l = 0; l < sizeof(limits)/sizeof(limits[0]); ++l)
    for (x = 0; x < sizeof(origins)/sizeof(origins[0]); ++x)
    for (s = 0; s < sizeof(spans)/sizeof(spans[0]); ++s) {
        int limit = limits[l], origin = origins[x], span = spans[s];
        int actual = angdroid_term_cursor_last_cell(origin, span, limit);
        if (limit <= 0 || origin < 0 || origin >= limit) {
            CHECK(actual == -1);
        } else {
            long long effective = span < 1 ? 1 : (long long)span;
            long long result = (long long)origin + effective - 1;
            if (result >= limit) result = limit - 1;
            CHECK(actual == (int)result);
            CHECK(actual >= origin && actual < limit);
        }
    }
}

static void test_big_cursor(void)
{
    static const int dims[] = {INT_MIN, -1, 0, 1, 2, 3, 80, INT_MAX};
    static const int positions[] = {INT_MIN, -1, 0, 1, 2, 79, INT_MAX};
    size_t w, h, x, y, tw, th;
    for (w = 0; w < sizeof(dims)/sizeof(dims[0]); ++w)
    for (h = 0; h < sizeof(dims)/sizeof(dims[0]); ++h)
    for (x = 0; x < sizeof(positions)/sizeof(positions[0]); ++x)
    for (y = 0; y < sizeof(positions)/sizeof(positions[0]); ++y)
    for (tw = 0; tw < sizeof(dims)/sizeof(dims[0]); ++tw)
    for (th = 0; th < sizeof(dims)/sizeof(dims[0]); ++th) {
        long long remw = (long long)dims[w] - positions[x];
        long long remh = (long long)dims[h] - positions[y];
        bool expected = dims[w] > 0 && dims[h] > 0 &&
            positions[x] >= 0 && positions[y] >= 0 &&
            positions[x] < dims[w] && positions[y] < dims[h] &&
            dims[tw] > 0 && dims[th] > 0 &&
            dims[tw] <= remw && dims[th] <= remh;
        bool actual = angdroid_term_big_cursor_fits(
            positions[x], positions[y], dims[w], dims[h], dims[tw], dims[th]);
        CHECK(actual == expected);
    }
}

static void test_wide_cursor_partner(void)
{
    wchar_t c[5] = {L'A', L'甲', ANGDROID_TEXT_CONTINUATION, L'B', L' '};
    int a[5] = {1, 1, 1, 1, 1};
    CHECK(angdroid_wide_partner_column(5, 1, c, a) == 2);
    CHECK(angdroid_wide_partner_column(5, 2, c, a) == 1);
    CHECK(angdroid_wide_partner_column(5, 0, c, a) == -1);
    CHECK(angdroid_wide_partner_column(5, 3, c, a) == -1);
    CHECK(angdroid_wide_partner_column(5, 4, c, a) == -1);
    CHECK(angdroid_wide_partner_column(1, 0, c, a) == -1);
    a[1] = 0x80;  /* Graphics tile: never treat it as CJK text. */
    CHECK(angdroid_wide_partner_column(5, 1, c, a) == -1);
    CHECK(angdroid_wide_partner_column(5, 2, c, a) == -1);
    a[1] = 1;
    c[2] = L' ';
    CHECK(angdroid_wide_partner_column(5, 1, c, a) == -1);
    CHECK(angdroid_wide_partner_column(5, 2, c, a) == -1);
}

int main(void)
{
    test_cursor_spans();
    test_big_cursor();
    test_wide_cursor_partner();
    printf("PASS: terminal soft cursor span, large cursor and CJK cells (%lu assertions)\n",
        assertions);
    return 0;
}
