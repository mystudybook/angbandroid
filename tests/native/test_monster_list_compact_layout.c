/* Test tiny/normal screen regions and optional monster fields. */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/monster-list-compact-layout.h"

static int count;
#define CHECK(condition) do { assert(condition); ++count; } while (0)

static void check_all_popup_bounds(void)
{
    const int widths[] = { -1, 0, 1, 3, 10, 11, 12, 18, 39, 40, 41,
                           42, 43, 44, 79, 80, 120, 400, INT_MAX };
    const int heights[] = { -1, 0, 1, 2, 5, 6, 7, 8, 12, 24, 40, INT_MAX };
    const size_t pref[] = { 0, 1, 2, 4, 20, 40, 200, SIZE_MAX };
    size_t i, j, k, z;
    struct angdroid_monster_list_popup area;
    for (i = 0; i < sizeof(widths)/sizeof(widths[0]); ++i) {
        for (j = 0; j < sizeof(heights)/sizeof(heights[0]); ++j) {
            for (k = 0; k < sizeof(pref)/sizeof(pref[0]); ++k) {
                for (z = 0; z < sizeof(pref)/sizeof(pref[0]); ++z) {
                    bool shown = angdroid_monster_list_popup_fit(widths[i],
                        heights[j], pref[k], pref[z], &area);
                    CHECK(shown == (widths[i] >= 12 && heights[j] >= 7));
                    if (!shown) {
                        CHECK(!area.width && !area.height && !area.col);
                        continue;
                    }
                    CHECK(area.col >= 1 && area.row == 1);
                    CHECK(area.width >= 4 && area.height >= 4);
                    CHECK(area.width <= widths[i] - area.col);
                    CHECK(area.height <= heights[j] - area.row - 2);
                    CHECK(area.col - 1 >= 0);
                    CHECK(area.col + area.width <= widths[i]);
                    CHECK(area.row + area.height + 1 < heights[j]);
                    if (widths[i] == 80 && pref[k] >= 40)
                        CHECK(area.width == 40);
                }
            }
        }
    }
    CHECK(!angdroid_monster_list_popup_fit(80, 24, 30, 10, NULL));
}

static void check_rows(void)
{
    struct angdroid_monster_row_budget b;
    int width;
    for (width = -1; width <= 200; ++width) {
        size_t distance;
        for (distance = 0; distance <= 35; ++distance) {
            b = angdroid_monster_row_fit(width, distance);
            CHECK(b.name_columns <= (width > 3 ? (size_t)(width - 3) : 0));
            if (b.show_location) {
                CHECK(distance != 0 && b.name_columns >= 4);
                CHECK(b.name_columns + distance + 3 == (size_t)width);
            } else if (width > 3) {
                CHECK(b.name_columns + 3 == (size_t)width);
            }
        }
    }
    b = angdroid_monster_row_fit(24, 11);
    CHECK(b.show_location && b.name_columns == 10);
    b = angdroid_monster_row_fit(14, 11);
    CHECK(!b.show_location && b.name_columns == 11);
    b = angdroid_monster_row_fit(0, SIZE_MAX);
    CHECK(!b.show_location && !b.name_columns);
    CHECK(angdroid_monster_sleep_fits(15, 10));
    CHECK(!angdroid_monster_sleep_fits(10, 10));
    CHECK(!angdroid_monster_sleep_fits(12, 0));
    CHECK(!angdroid_monster_sleep_fits(0, SIZE_MAX));
}

int main(void)
{
    check_all_popup_bounds();
    check_rows();
    printf("PASS: monster-list compact geometry (%d assertions)\n", count);
    return 0;
}
