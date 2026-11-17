#ifndef ANGDROID_MENU_HIT_TEST_H
#define ANGDROID_MENU_HIT_TEST_H

#include <stdint.h>

/* Return -1 for empty rows instead of silently selecting the final item.
 * The coordinates are in native terminal cells, not Android pixels. */
static inline int angdroid_menu_scrolling_hit(
    int row, int origin_row, int page_rows, int top, int count)
{
    int64_t offset = (int64_t)row - origin_row;

    if (count <= 0 || page_rows <= 0 || top < 0 || top >= count ||
        offset < 0 || offset >= page_rows || offset >= (int64_t)count - top)
        return -1;
    return top + (int)offset;
}

/* Match display_columns(): first items flow down rows then across columns.
 * Reject unused cells in the last column and the remaining empty space. */
static inline int angdroid_menu_columns_hit(
    int row, int col, int origin_row, int origin_col,
    int page_rows, int region_width, int column_width, int count)
{
    int64_t dx = (int64_t)col - origin_col;
    int64_t dy = (int64_t)row - origin_row;
    int64_t index;

    if (count <= 0 || page_rows <= 0 || region_width <= 0 ||
        column_width <= 0 || dx < 0 || dx >= region_width ||
        dy < 0 || dy >= page_rows)
        return -1;

    index = (dx / column_width) * page_rows + dy;
    return index < count ? (int)index : -1;
}

/* "Back" is a tap in the left margin ALONGSIDE the active rows, not
 * anywhere above them. In a shop, the owner/header occupies those top
 * rows: tapping it and dismissing its context menu must not exit the shop.
 * Reject negative native coordinates and use widened row subtraction. */
static inline int angdroid_menu_back_margin_hit(
    int row, int col, int origin_row, int origin_col, int page_rows)
{
    return row >= 0 && col >= 0 && origin_row >= 0 &&
        origin_col > 0 && page_rows > 0 &&
        col < origin_col && row >= origin_row &&
        (int64_t)row - origin_row < page_rows;
}

#endif
