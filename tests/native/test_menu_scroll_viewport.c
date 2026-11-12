/* Angband scroll/item menu bounds after rotation or filtered-list shrink. */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-scroll-viewport.h"

static unsigned long checks;
#define CHECK(expression) do { assert(expression); ++checks; } while (0)

static void exhaustive_small_viewports(void)
{
    int count, rows, cursor, top;
    for (count = -2; count <= 64; ++count)
        for (rows = -2; rows <= 34; ++rows)
            for (cursor = -2; cursor <= 70; ++cursor)
                for (top = -2; top <= 70; ++top) {
                    int actual = angdroid_menu_scroll_clamp_top(
                        top, cursor, count, rows);
                    if (count <= 0 || rows <= 0) {
                        CHECK(actual == 0);
                        continue;
                    }
                    {
                        int maximum = count > rows ? count - rows : 0;
                        CHECK(actual >= 0 && actual <= maximum);
                        if (cursor >= 0 && cursor < count) {
                            CHECK(cursor >= actual);
                            CHECK(cursor - actual < rows);
                        }
                    }
                }
}

static void boundary_cases(void)
{
    CHECK(angdroid_menu_scroll_clamp_top(40, 4, 6, 12) == 0);
    CHECK(angdroid_menu_scroll_clamp_top(90, 9, 18, 4) == 9);
    CHECK(angdroid_menu_scroll_clamp_top(0, 17, 18, 4) == 14);
    CHECK(angdroid_menu_scroll_clamp_top(0, -1, 18, 4) == 0);
    CHECK(angdroid_menu_scroll_clamp_top(99, -1, 18, 4) == 14);
    CHECK(angdroid_menu_scroll_clamp_top(-3, -1, 18, 4) == 0);
    CHECK(angdroid_menu_scroll_clamp_top(INT_MAX, INT_MAX - 1,
        INT_MAX, 2) == INT_MAX - 2);
    CHECK(angdroid_menu_scroll_clamp_top(INT_MIN, INT_MIN,
        INT_MAX, 2) == 0);
    CHECK(angdroid_menu_scroll_clamp_top(INT_MAX, INT_MAX,
        INT_MAX, 2) == INT_MAX - 2);
    CHECK(angdroid_menu_scroll_clamp_top(INT_MIN, INT_MAX,
        INT_MAX, INT_MAX) == 0);
    CHECK(angdroid_menu_scroll_clamp_top(INT_MAX, -1, 0, 5) == 0);
}

int main(void)
{
    exhaustive_small_viewports();
    boundary_cases();
    printf("PASS: menu scroll & item viewports after rotation (%lu checks)\n",
        checks);
    return 0;
}
