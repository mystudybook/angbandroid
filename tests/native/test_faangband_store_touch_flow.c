/* FAangband stock and item touches: a screen cell is first mapped to the
 * currently rendered view row, then to the underlying object or stock index.
 * Production helpers are used directly; no Android device is required.
 */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-hit-test.h"
#include "../../app/src/main/cpp/angband/store-menu-row.h"
#include "../../app/src/main/cpp/angband/store-stock-guard.h"
#include "../../app/src/main/cpp/faangband/ui-menu-key-guard.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void stock_touch_after_scrolling(void)
{
    int count, top, rows, y;
    const int first_row = 4;

    for (count = 0; count <= 45; ++count)
        for (top = 0; top <= count; ++top)
            for (rows = 1; rows <= 12; ++rows)
                for (y = first_row - 2; y <= first_row + rows + 2; ++y) {
                    int view = angdroid_menu_scrolling_hit(
                        y, first_row, rows, top, count);
                    int popup = angdroid_store_menu_screen_row(
                        view, top, first_row, rows);
                    if (view >= 0) {
                        CHECK(view >= top && view < count);
                        CHECK(popup == y);
                        CHECK(angdroid_store_stock_index_valid(
                            view, count, 45));
                    } else {
                        CHECK(popup == -1);
                    }
                }
    CHECK(angdroid_menu_scrolling_hit(4, 4, 6, 8, 25) == 8);
    CHECK(angdroid_menu_scrolling_hit(9, 4, 6, 8, 25) == 13);
    CHECK(angdroid_store_menu_screen_row(13, 8, 4, 6) == 9);
    CHECK(angdroid_store_menu_screen_row(7, 8, 4, 6) == -1);
}

static void filtered_inventory_view(void)
{
    const int filter[] = {8, 3, 11, 2};
    int i;
    for (i = 0; i < 4; ++i) {
        int view = angdroid_menu_scrolling_hit(10 + i, 10, 4, 0, 4);
        int source = angdroid_fa_menu_source_index(filter, view, 4, 12);
        CHECK(view == i);
        CHECK(source == filter[i]);
    }
    CHECK(angdroid_menu_scrolling_hit(14, 10, 4, 0, 4) == -1);
    CHECK(angdroid_fa_menu_source_index(filter, -1, 4, 12) == -1);
    CHECK(angdroid_fa_menu_source_index(filter, 4, 4, 12) == -1);
}

static void obsolete_selection_cannot_purchase(void)
{
    int selected, count, is_select;
    for (count = 0; count <= 30; ++count)
        for (selected = -2; selected <= 32; ++selected)
            for (is_select = 0; is_select <= 1; ++is_select) {
                int actual = angdroid_store_selected_index(
                    is_select, selected, count, 30);
                int expected = is_select && selected >= 0 &&
                    selected < count && selected < 30 ? selected : -1;
                CHECK(actual == expected);
            }
    CHECK(angdroid_store_selected_index(0, 3, 10, 30) == -1);
    CHECK(angdroid_store_selected_index(1, 3, 2, 30) == -1);
    CHECK(angdroid_store_selected_index(1, 3, 10, 3) == -1);
    CHECK(angdroid_store_menu_screen_row(
        INT_MAX, INT_MAX, INT_MAX, INT_MAX) == INT_MAX);
    CHECK(angdroid_store_menu_screen_row(
        INT_MAX, 0, INT_MAX, INT_MAX) == -1);
}

int main(void)
{
    stock_touch_after_scrolling();
    filtered_inventory_view();
    obsolete_selection_cannot_purchase();
    printf("PASS: FA stock touch before callback, scroll offsets, filtered items (%lu checks)\n",
        checks);
    return 0;
}
