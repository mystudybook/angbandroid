/* Object list geometry must survive compact terminals and huge size_t data. */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/object-list-compact-layout.h"

static unsigned long assertions;
#define CHECK(expr) do { assert(expr); ++assertions; } while (0)

static void test_popup_bounds(void)
{
    const int widths[] = { INT_MIN, -1, 0, 1, 11, 12, 16, 27, 40, 47,
                           48, 49, 79, 80, 100, 400, INT_MAX };
    const int heights[] = { INT_MIN, -1, 0, 1, 5, 6, 7, 8, 24, 40, INT_MAX };
    const size_t prefs[] = { 0, 1, 2, 4, 17, 40, 200, SIZE_MAX };
    size_t i, j, k, m;
    struct angdroid_object_list_popup p;

    for (i = 0; i < sizeof(widths)/sizeof(widths[0]); ++i)
        for (j = 0; j < sizeof(heights)/sizeof(heights[0]); ++j)
            for (k = 0; k < sizeof(prefs)/sizeof(prefs[0]); ++k)
                for (m = 0; m < sizeof(prefs)/sizeof(prefs[0]); ++m) {
                    bool shown = angdroid_object_list_popup_fit(widths[i],
                        heights[j], prefs[k], prefs[m], &p);
                    CHECK(shown == (widths[i] >= 12 && heights[j] >= 7));
                    if (!shown) {
                        CHECK(p.col == 0 && p.width == 0 && p.height == 0);
                        continue;
                    }
                    CHECK(p.row == 1 && p.width >= 7 && p.height >= 4);
                    CHECK(p.col >= 1 && p.col < widths[i]);
                    CHECK(p.width <= widths[i] - p.col);
                    CHECK(p.height <= heights[j] - p.row - 2);
                    CHECK(p.row + p.height + 1 < heights[j]);
                    if (widths[i] == 80 && prefs[k] >= 67)
                        CHECK(p.width == 67);
                }
    CHECK(!angdroid_object_list_popup_fit(80, 24, 40, 20, NULL));
}

static void test_row_budget(void)
{
    int width;
    for (width = -2; width < 180; ++width) {
        size_t distance;
        for (distance = 0; distance < 40; ++distance) {
            struct angdroid_object_row_budget b =
                angdroid_object_row_fit(width, distance);
            CHECK(b.name_columns <= (width > 3 ? (size_t)(width - 3) : 0));
            if (b.show_location) {
                CHECK(distance != 0 && b.name_columns >= 8);
                CHECK(b.name_columns + distance + 3 == (size_t)width);
            } else if (width > 3) {
                CHECK(b.name_columns + 3 == (size_t)width);
            }
        }
    }
    CHECK(angdroid_object_row_fit(25, 10).show_location);
    CHECK(angdroid_object_row_fit(25, 10).name_columns == 12);
    CHECK(!angdroid_object_row_fit(20, 10).show_location);
    CHECK(!angdroid_object_row_fit(14, 10).show_location);
    CHECK(angdroid_object_row_fit(14, 10).name_columns == 11);
    CHECK(!angdroid_object_row_fit(0, SIZE_MAX).show_location);
}

int main(void)
{
    test_popup_bounds();
    test_row_budget();
    printf("PASS: object-list compact layout (%lu assertions)\n", assertions);
    return 0;
}
