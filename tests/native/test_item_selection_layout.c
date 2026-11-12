/* Rotation and scroll-safe mobile item-selection geometry (C99). */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "../../app/src/main/cpp/angband/item-selection-layout.h"
#include "../../app/src/main/cpp/angband/menu-hit-test.h"

int main(void)
{
    angdroid_item_selection_layout l;
    int w, h, count, footer;
    assert(!angdroid_item_selection_region(0, 24, 15, 32, 10, 19,
        20, false, &l));
    assert(!angdroid_item_selection_region(25, 1, 15, 32, 10, 19,
        20, false, &l));
    assert(!angdroid_item_selection_region(25, 24, 15, 32, 10, 19,
        0, false, &l));
    assert(!angdroid_item_selection_region(25, 24, 15, 32, 10, 19,
        20, false, NULL));

    assert(angdroid_item_selection_region(100, 40, 16, 42, 25, 9,
        30, false, &l));
    assert(l.col >= 0 && l.width > 0 && l.col + l.width <= 100);
    assert(l.row == 1 && l.page_rows == 30);
    assert(angdroid_item_selection_region(22, 12, 16, 42, 25, 9,
        30, true, &l));
    assert(l.col == 0 && l.width == 22);
    assert(l.row == 1 && l.page_rows == 10);
    assert(angdroid_item_selection_footer_row(
        l.row, l.page_rows, 12, 0) == 11);
    assert(angdroid_item_selection_footer_row(
        l.row, l.page_rows, 12, 1) == -1);
    assert(angdroid_menu_scrolling_hit(
        3, l.row, l.page_rows, 5, 30) == 7);
    assert(angdroid_menu_scrolling_hit(
        11, l.row, l.page_rows, 5, 30) == -1);

    assert(angdroid_item_selection_region(
        22, 12, 16, SIZE_MAX, SIZE_MAX, INT_MAX, 30, false, &l));
    assert(l.col == 0 && l.width <= 22);
    assert(angdroid_item_selection_footer_row(
        INT_MAX, INT_MAX, INT_MAX, 1) == -1);
    assert(angdroid_item_selection_footer_row(1, 0, 22, 0) == -1);
    assert(angdroid_item_selection_footer_row(1, 10, 22, -1) == -1);

    /* A source with zero selectable objects is still navigable when
     * another inventory/equipment/quiver/floor tab can be opened.
     * The placeholder viewport row is not a real item: the actual menu
     * count remains zero, so touch and Enter cannot select an object. */
    assert(angdroid_item_selection_viewport_count(0, false) == 0);
    assert(angdroid_item_selection_viewport_count(0, true) == 1);
    assert(angdroid_item_selection_viewport_count(-1, false) == 0);
    assert(angdroid_item_selection_viewport_count(-1, true) == 1);
    assert(angdroid_item_selection_viewport_count(1, false) == 1);
    assert(angdroid_item_selection_viewport_count(23, true) == 23);
    assert(angdroid_item_selection_viewport_count(INT_MAX, true) == INT_MAX);
    assert(angdroid_item_selection_region(22, 12, 16, 42, 25, 9,
        angdroid_item_selection_viewport_count(0, true), false, &l));
    assert(l.page_rows == 1);
    assert(angdroid_menu_scrolling_hit(
        l.row, l.row, l.page_rows, 0, 0) == -1);
    assert(!angdroid_item_selection_region(22, 12, 16, 42, 25, 9,
        angdroid_item_selection_viewport_count(0, false), false, &l));

    /* An empty bag can reserve a quiver footer after the one virtual
     * viewport row; the row remains outside every selectable touch hit. */
    assert(angdroid_item_selection_region(22, 12, 16, 42, 25, 9,
        angdroid_item_selection_viewport_count(0, true), true, &l));
    assert(l.page_rows == 1);
    assert(angdroid_item_selection_footer_row(l.row, l.page_rows, 12, 0) == 2);
    assert(angdroid_menu_scrolling_hit(
        2, l.row, l.page_rows, 0, 0) == -1);

    /* Compare the production pure-C helper against an independent
     * int64_t ceil division (which cannot overflow on any int input). */
    for (count = -2; count <= 256; ++count) {
        int slot;
        for (slot = -2; slot <= 64; ++slot) {
            int expected = (count <= 0 || slot <= 0) ? 0 :
                (int)(((int64_t)count + slot - 1) / slot);
            assert(angdroid_item_quiver_footer_slots(count, slot) ==
                expected);
        }
    }
    assert(angdroid_item_quiver_footer_slots(INT_MAX, 1) == INT_MAX);
    assert(angdroid_item_quiver_footer_slots(INT_MAX, 2) ==
        (INT_MAX / 2 + 1));
    assert(angdroid_item_quiver_footer_slots(INT_MAX, INT_MAX) == 1);
    assert(angdroid_item_quiver_footer_slots(1, INT_MAX) == 1);
    assert(angdroid_item_quiver_footer_slots(1, 0) == 0);
    assert(angdroid_item_quiver_footer_slots(-1, 1) == 0);

    for (w = 0; w <= 160; ++w) {
        for (h = 0; h <= 40; ++h) {
            for (count = 0; count <= 55; count += 5) {
                for (footer = 0; footer <= 1; ++footer) {
                    bool ok = angdroid_item_selection_region(
                        w, h, 20, 65, 35, 19, count, footer, &l);
                    if (!ok) {
                        assert(l.width == 0 && l.page_rows == 0);
                        continue;
                    }
                    assert(w > 0 && h > 1 && count > 0);
                    assert(l.row >= 0 && l.row < h);
                    assert(l.col >= 0 && l.col < w);
                    assert(l.width > 0 && l.width <= w - l.col);
                    assert(l.page_rows >= 1 && l.page_rows <= count);
                    assert(l.row + l.page_rows <= h);
                    if (footer && h >= 3 && count >= h - 1) {
                        int y = angdroid_item_selection_footer_row(
                            l.row, l.page_rows, h, 0);
                        assert(y < h && y >= l.row + l.page_rows);
                    }
                    assert(angdroid_menu_scrolling_hit(
                        l.row - 1, l.row, l.page_rows, 0, count) == -1);
                    assert(angdroid_menu_scrolling_hit(
                        l.row + l.page_rows, l.row, l.page_rows,
                        0, count) == -1);
                }
            }
        }
    }
    puts("PASS: rotation geometry, footer bounds and stock touch rows");
    return 0;
}
