/* C99: FAangband's two visibility sections must fit extremely short
 * terminal displays and must not include the other section in "...others".
 */
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include "../../app/src/main/cpp/faangband/obj-list-row-budget.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void exhaustive_small_layouts(void)
{
    int h, near, far;
    for (h = -2; h <= 24; ++h) {
        for (near = 0; near <= 36; ++near) {
            for (far = 0; far <= 36; ++far) {
                struct angdroid_fa_object_rows plan =
                    angdroid_fa_list_row_budget(h, near, far);
                int lines = h > 0 ? 1 : 0;
                CHECK(plan.near_rows >= 0 && plan.near_rows <= near);
                CHECK(plan.distant_rows >= 0 && plan.distant_rows <= far);
                CHECK(plan.show_distant == (plan.distant_rows > 0));
                CHECK(!plan.near_overflow || (plan.near_rows > 0 &&
                    plan.near_rows < near));
                CHECK(!plan.distant_overflow || (plan.distant_rows > 0 &&
                    plan.distant_rows < far));
                if (!plan.show_distant) {
                    CHECK(plan.distant_rows == 0 && !plan.distant_overflow);
                } else {
                    CHECK(h >= 4);
                    lines += 2; /* blank separator and distant heading */
                }
                lines += plan.near_rows + (plan.near_overflow ? 1 : 0);
                lines += plan.distant_rows + (plan.distant_overflow ? 1 : 0);
                CHECK(lines <= (h > 0 ? h : 0));
                if (h <= 0) CHECK(lines == 0);
            }
        }
    }
}

static void known_tight_screens(void)
{
    struct angdroid_fa_object_rows p;
    p = angdroid_fa_list_row_budget(1, 30, 30);
    CHECK(p.near_rows == 0 && p.distant_rows == 0 && !p.show_distant);
    p = angdroid_fa_list_row_budget(2, 30, 30);
    CHECK(p.near_rows == 1 && !p.near_overflow && !p.show_distant);
    p = angdroid_fa_list_row_budget(3, 30, 30);
    CHECK(p.near_rows == 1 && p.near_overflow && !p.show_distant);
    p = angdroid_fa_list_row_budget(4, 30, 30);
    CHECK(p.near_rows == 2 && p.near_overflow && !p.show_distant);
    p = angdroid_fa_list_row_budget(5, 30, 30);
    CHECK(p.near_rows == 1 && p.distant_rows == 1 && p.show_distant);
    CHECK(!p.near_overflow && !p.distant_overflow);
    p = angdroid_fa_list_row_budget(4, 0, 30);
    CHECK(p.near_rows == 0 && p.distant_rows == 1 && p.show_distant);
    p = angdroid_fa_list_row_budget(40, 3, 9);
    CHECK(p.near_rows == 3 && p.distant_rows == 9 && p.show_distant);
    CHECK(!p.near_overflow && !p.distant_overflow);
    p = angdroid_fa_list_row_budget(7, 3, 9);
    CHECK(p.near_rows >= 1 && p.distant_rows >= 1 && p.show_distant);
}

static void exact_section_counts(void)
{
    CHECK(angdroid_fa_list_undisplayed(3, 2) == 1);
    CHECK(angdroid_fa_list_undisplayed(9, 1) == 8);
    CHECK(angdroid_fa_list_undisplayed(9, 9) == 0);
    CHECK(angdroid_fa_list_undisplayed(9, 11) == 0);
    CHECK(angdroid_fa_list_undisplayed(0, 0) == 0);
    CHECK(angdroid_fa_list_undisplayed(-10, 0) == 0);
    CHECK(angdroid_fa_list_undisplayed(INT_MAX, 1) == INT_MAX - 1);
}

static void large_values(void)
{
    struct angdroid_fa_object_rows p;
    int used;
    p = angdroid_fa_list_row_budget(INT_MAX, INT_MAX, INT_MAX);
    CHECK(p.show_distant && p.near_rows > 0 && p.distant_rows > 0);
    used = 3 + p.near_rows + (p.near_overflow ? 1 : 0) +
        p.distant_rows + (p.distant_overflow ? 1 : 0);
    CHECK(used <= INT_MAX);
    p = angdroid_fa_list_row_budget(INT_MIN, INT_MAX, INT_MAX);
    CHECK(!p.show_distant && !p.near_rows && !p.distant_rows);
    p = angdroid_fa_list_row_budget(40, -1, 9);
    CHECK(!p.near_rows && p.distant_rows == 9);
    p = angdroid_fa_list_row_budget(40, 9, -1);
    CHECK(!p.distant_rows && p.near_rows == 9);
}

int main(void)
{
    exhaustive_small_layouts();
    known_tight_screens();
    exact_section_counts();
    large_values();
    printf("PASS: FA object-list short row budget and exact hidden totals (%lu checks)\n",
        checks);
    return 0;
}
