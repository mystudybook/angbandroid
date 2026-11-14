/* FAangband popup placement and filtered source-index contract.
 * Uses production helpers; no Android runtime or game simulation.
 */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-popup-layout.h"
#include "../../app/src/main/cpp/faangband/ui-menu-key-guard.h"

static unsigned long checks;
#define CHECK(condition) do { assert(condition); ++checks; } while (0)

static void popup_viewports(void)
{
    int w, h, count;
    for (w = 0; w <= 36; ++w)
        for (h = 0; h <= 24; ++h)
            for (count = 0; count <= 35; ++count) {
                angdroid_menu_popup_box b;
                int ok = angdroid_menu_popup_bounds(
                    w, h, INT_MAX, INT_MIN, SIZE_MAX, count, &b);
                if (!w || !h || !count) {
                    CHECK(!ok);
                    CHECK(b.width == 0 && b.page_rows == 0);
                } else {
                    CHECK(ok);
                    CHECK(b.col >= 0 && b.row >= 0);
                    CHECK(b.width > 0 && b.page_rows > 0);
                    CHECK(b.width <= w - b.col);
                    CHECK(b.page_rows <= h - b.row);
                    CHECK(b.page_rows <= count);
                }
            }
    {
        angdroid_menu_popup_box b;
        CHECK(angdroid_menu_popup_bounds(
            30, 12, 4, 3, 6, 30, &b));
        CHECK(b.width == 11);
        CHECK(b.page_rows == 10);
        CHECK(b.col >= 0 && b.col + b.width < 30);
        CHECK(b.row >= 0 && b.row + b.page_rows < 12);
        CHECK(!angdroid_menu_popup_bounds(0, 12, 0, 0, 0, 1, &b));
        CHECK(!angdroid_menu_popup_bounds(12, 12, 0, 0, 0, 0, &b));
        CHECK(angdroid_menu_popup_bounds(
            1, 1, INT_MIN, INT_MAX, SIZE_MAX, INT_MAX, &b));
        CHECK(b.col == 0 && b.row == 0);
        CHECK(b.width == 1 && b.page_rows == 1);
    }
}

static void filtered_choice_mapping(void)
{
    const int filter[] = {5, 2, 0, 3};
    const int invalid[] = {2, -1, 19};
    int values[] = {11, 22, 33, 44, 55, 66};
    int index, source;
    for (index = 0; index < 4; ++index) {
        source = angdroid_fa_menu_source_index(filter, index, 4, 6);
        CHECK(source >= 0 && source < 6);
        CHECK(values[source] == values[filter[index]]);
    }
    CHECK(angdroid_fa_menu_source_index(filter, 0, 4, 6) == 5);
    CHECK(angdroid_fa_menu_source_index(filter, 1, 4, 6) == 2);
    CHECK(angdroid_fa_menu_source_index(filter, -1, 4, 6) == -1);
    CHECK(angdroid_fa_menu_source_index(filter, 4, 4, 6) == -1);
    CHECK(angdroid_fa_menu_source_index(filter, 0, 0, 6) == -1);
    CHECK(angdroid_fa_menu_source_index(NULL, 3, 6, 6) == 3);
    CHECK(angdroid_fa_menu_source_index(invalid, 1, 3, 6) == -1);
    CHECK(angdroid_fa_menu_source_index(invalid, 2, 3, 6) == -1);
    CHECK(angdroid_fa_menu_source_index(filter, INT_MAX, 4, 6) == -1);
}

int main(void)
{
    popup_viewports();
    filtered_choice_mapping();
    printf("PASS: FA popup geometry and filtered dynamic selection (%lu checks)\n",
        checks);
    return 0;
}
