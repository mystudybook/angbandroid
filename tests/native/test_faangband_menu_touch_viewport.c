/* Strict C99 regression for actual FAangband mouse/menu viewport helpers.
 * The production headers are shared by scrolling, object and column skins.
 */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/faangband/ui-menu-viewport.h"
#include "../../app/src/main/cpp/faangband/ui-menu-key-guard.h"
#include "../../app/src/main/cpp/angband/menu-hit-test.h"
#include "../../app/src/main/cpp/angband/menu-column-width.h"
#include "../../app/src/main/cpp/angband/menu-compact-layout.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void scrolling_blank_rows(void)
{
    int count, page, top, offset;
    for (count = 0; count <= 36; ++count)
        for (page = 0; page <= 16; ++page)
            for (top = -1; top <= 36; ++top)
                for (offset = -2; offset <= 18; ++offset) {
                    int expected = -1;
                    int at = angdroid_menu_scrolling_hit(
                        12 + offset, 12, page, top, count);
                    if (count > 0 && page > 0 && top >= 0 &&
                        top < count && offset >= 0 &&
                        offset < page && offset < count - top)
                        expected = top + offset;
                    CHECK(at == expected);
                }

    CHECK(angdroid_menu_scrolling_hit(16, 12, 9, 0, 3) == -1);
    CHECK(angdroid_menu_scrolling_hit(14, 12, 9, 0, 3) == 2);
    CHECK(angdroid_menu_scrolling_hit(15, 12, 4, 5, 8) == -1);
    CHECK(angdroid_menu_scrolling_hit(13, 12, 4, 5, 8) == 6);
    CHECK(angdroid_menu_scrolling_hit(INT_MAX, INT_MIN, 4, 0, 5) == -1);
}

static void columns_match_drawn_cells(void)
{
    int count, rows, width, y, x;
    for (count = 0; count <= 24; ++count)
        for (rows = 0; rows <= 8; ++rows)
            for (width = 1; width <= 36; ++width) {
                int columns = count > 0 && rows > 0 ?
                    1 + (count - 1) / rows : 0;
                int compact = angdroid_menu_use_compact_columns(
                    count, rows, width, 8);
                int colw = angdroid_menu_fit_column_width(
                    23, width, 0, columns);
                CHECK(colw >= 1);
                for (y = -1; y <= rows + 1; ++y)
                    for (x = -1; x <= width + 1; ++x) {
                        int hit;
                        if (count <= 0 || rows <= 0) {
                            hit = -1;
                        } else if (compact) {
                            hit = angdroid_menu_scrolling_hit(
                                3 + y, 3, rows, 0, count);
                        } else {
                            hit = angdroid_menu_columns_hit(
                                3 + y, 5 + x, 3, 5,
                                rows, width, colw, count);
                        }
                        if (hit >= 0) {
                            CHECK(hit < count);
                            if (!compact) {
                                CHECK(y >= 0 && y < rows);
                                CHECK(x >= 0 && x < width);
                                CHECK((x / colw) * rows + y == hit);
                            } else {
                                CHECK(y == hit);
                            }
                        }
                    }
            }
    CHECK(angdroid_menu_use_compact_columns(20, 2, 10, 8));
    CHECK(!angdroid_menu_use_compact_columns(4, 4, 10, 8));
    CHECK(angdroid_menu_columns_hit(3, 35, 3, 5, 4, 40, 10, 10) == -1);
    CHECK(angdroid_menu_columns_hit(5, 25, 3, 5, 4, 40, 10, 10) == -1);
    CHECK(angdroid_menu_columns_hit(4, 25, 3, 5, 4, 40, 10, 12) == 9);
    CHECK(angdroid_menu_columns_hit(0, 0, 0, 0, 0, 40, 1, 20) == -1);
    CHECK(angdroid_menu_columns_hit(0, 0, 0, 0, 4, 40, 0, 20) == -1);
}

static void scroll_cursor_after_rotation(void)
{
    int count, page, cursor, top;
    for (count = -1; count <= 42; ++count)
        for (page = -1; page <= 18; ++page)
            for (cursor = -3; cursor <= 45; ++cursor)
                for (top = -3; top <= 45; top += 3) {
                    int selected = angdroid_fa_menu_clamp_cursor(
                        cursor, count);
                    int next = angdroid_fa_menu_clamp_top(
                        top, selected, count, page);
                    CHECK(selected >= -1);
                    CHECK(selected < count || selected == -1);
                    CHECK(next >= 0);
                    if (page > 0 && count > 0) {
                        int max_top = count > page ? count - page : 0;
                        CHECK(next <= max_top);
                        CHECK(selected >= next);
                        CHECK(selected - next < page);
                    } else CHECK(next == 0);
                }
    CHECK(angdroid_fa_menu_clamp_cursor(INT_MAX, INT_MAX) == INT_MAX - 1);
    CHECK(angdroid_fa_menu_clamp_cursor(INT_MIN, INT_MAX) == 0);
    CHECK(angdroid_fa_menu_clamp_top(INT_MAX, INT_MAX - 1,
        INT_MAX, 1) == INT_MAX - 1);
    CHECK(angdroid_fa_menu_clamp_top(INT_MIN, 0, INT_MAX, 1) == 0);
    CHECK(angdroid_fa_menu_clamp_top(3, -1, 0, 20) == 0);
}

static void relative_shortcuts_after_scrolling(void)
{
    const char *shortcuts = "abcde";
    int top, i;
    for (top = 0; top < 40; ++top)
        for (i = 0; i < 5; ++i) {
            int absolute_row = top + i;
            CHECK(angdroid_fa_menu_selection_key(
                shortcuts, absolute_row - top) == shortcuts[i]);
            CHECK(angdroid_fa_menu_page_rows(
                40, top, 5) == (top <= 35 ? 5 : 40 - top));
        }
    CHECK(angdroid_fa_menu_selection_key(shortcuts, -1) == 0);
}

int main(void)
{
    scrolling_blank_rows();
    columns_match_drawn_cells();
    scroll_cursor_after_rotation();
    relative_shortcuts_after_scrolling();
    printf("PASS: FA filtered touch hit-test, compact columns and rotation viewport (%lu checks)\n",
        checks);
    return 0;
}
