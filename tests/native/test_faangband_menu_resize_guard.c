/* FAangband: strict C99 coverage for invalid terminal/menu geometry after
 * Android rotation. The actual helper is included from production Native.
 */
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/faangband/ui-menu-resize-guard.h"

static unsigned long checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static bool reference_visible(int w, int h, int col, int row,
                              int width, int rows)
{
    int64_t right = (int64_t)col + width;
    int64_t bottom = (int64_t)row + rows;
    return w > 0 && h > 0 && col >= 0 && row >= 0 &&
        width > 0 && rows > 0 && right <= w && bottom <= h;
}

static void exhaustive_tiny_terminals(void)
{
    int w, h, c, r, dw, dh;
    for (w = -1; w <= 12; ++w)
        for (h = -1; h <= 10; ++h)
            for (c = -1; c <= 12; ++c)
                for (r = -1; r <= 10; ++r)
                    for (dw = -1; dw <= 12; ++dw)
                        for (dh = -1; dh <= 10; ++dh) {
                            bool actual = angdroid_fa_menu_region_visible(
                                w, h, c, r, dw, dh);
                            CHECK(actual == reference_visible(
                                w, h, c, r, dw, dh));
                        }
}

static void rotation_extremes(void)
{
    static const int sizes[] = {
        INT_MIN, -1, 0, 1, 2, 7, 12, 24, 80, 120, INT_MAX
    };
    size_t i, j;
    int w, h;
    CHECK(angdroid_fa_menu_region_visible(80, 24, 1, 4, 78, 18));
    CHECK(!angdroid_fa_menu_region_visible(80, 24, 1, 4, 80, 18));
    CHECK(!angdroid_fa_menu_region_visible(80, 24, 1, 4, 78, 21));
    CHECK(!angdroid_fa_menu_region_visible(0, 0, 0, 0, 1, 1));
    for (i = 0; i < sizeof(sizes)/sizeof(sizes[0]); ++i)
        for (j = 0; j < sizeof(sizes)/sizeof(sizes[0]); ++j) {
            w = sizes[i];
            h = sizes[j];
            CHECK(angdroid_fa_menu_region_visible(
                w, h, 0, 0, 1, 1) ==
                reference_visible(w, h, 0, 0, 1, 1));
            CHECK(angdroid_fa_menu_region_visible(
                w, h, INT_MAX, INT_MAX, 1, 1) ==
                reference_visible(w, h, INT_MAX, INT_MAX, 1, 1));
            CHECK(angdroid_fa_menu_region_visible(
                w, h, INT_MIN, INT_MIN, INT_MAX, INT_MAX) ==
                reference_visible(w, h, INT_MIN, INT_MIN, INT_MAX, INT_MAX));
            CHECK(angdroid_fa_menu_region_visible(
                w, h, 1, 1, INT_MAX, INT_MAX) ==
                reference_visible(w, h, 1, 1, INT_MAX, INT_MAX));
        }
}

int main(void)
{
    exhaustive_tiny_terminals();
    rotation_extremes();
    printf("PASS: FA menu resize region safety (%lu checks)\n", checks);
    return 0;
}
