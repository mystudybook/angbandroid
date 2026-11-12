/* Host C99 tests for the FAangband ability browser layout (no Android SDK). */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/faangband/ui-ability-compact-layout.h"

static unsigned long checks;
#define CHECK(expression) do { assert(expression); ++checks; } while (0)

static void test_terminal_geometry(void)
{
    struct angdroid_fa_ability_layout plan;
    int w, h, titled;
    CHECK(!angdroid_fa_ability_plan(80, 24, 0, NULL));
    for (w = -2; w <= 160; ++w) {
        for (h = -2; h <= 100; ++h) {
            for (titled = 0; titled < 2; ++titled) {
                int works = angdroid_fa_ability_plan(w, h, titled, &plan);
                CHECK(works == (w >= 16 && h >= 12));
                if (!works) {
                    CHECK(plan.width == 0 && plan.menu_rows == 0 &&
                          plan.detail_row == 0 && plan.detail_rows == 0);
                    continue;
                }
                CHECK(plan.width > 0 && plan.width <= 80 &&
                      plan.width <= w);
                CHECK(plan.menu_rows >= (titled ? 5 : 4));
                CHECK(plan.detail_row == plan.menu_rows + 1);
                CHECK(plan.detail_rows == h - plan.detail_row);
                CHECK(plan.detail_rows >= 4 && plan.detail_rows <= 10);
                CHECK(plan.menu_rows - (titled ? 2 : 1) >= 3);
                CHECK(plan.detail_row + plan.detail_rows == h);
                CHECK(plan.menu_rows > 0 && plan.menu_rows < h);
            }
        }
    }
}

static void test_description_paging(void)
{
    size_t count;
    int rows, requested;
    for (count = 0; count <= 500; ++count) {
        for (rows = -1; rows <= 20; ++rows) {
            for (requested = -1; requested <= 30; ++requested) {
                int got = angdroid_fa_ability_page_clamp(
                    requested, count, rows);
                size_t last = rows > 0 && count > 0 ?
                    (count - 1) / (size_t)rows : 0;
                int expected = requested < 0 ? 0 : requested;
                if (rows <= 0 || !count) expected = 0;
                else if ((size_t)expected > last)
                    expected = (int)last;
                CHECK(got == expected);
            }
        }
    }
    CHECK(angdroid_fa_ability_page_clamp(INT_MAX, SIZE_MAX, 1)
        == INT_MAX);
    CHECK(angdroid_fa_ability_page_clamp(INT_MAX, 3, 1) == 2);
    CHECK(angdroid_fa_ability_page_clamp(INT_MAX, 1, 1) == 0);
}

int main(void)
{
    test_terminal_geometry();
    test_description_paging();
    printf("PASS: %lu FA ability layout and paging checks\n", checks);
    return 0;
}
