#ifndef ANGDROID_OBJECT_LIST_TEXT_H
#define ANGDROID_OBJECT_LIST_TEXT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "menu-text-fit.h"

/* Space available for an item name after fixed extra-field columns.
 * An int below zero must never become a huge unsigned size_t. */
static inline int angdroid_object_list_name_offset(
    int terminal_width, int start_col, int reserved_cols, size_t desired)
{
    int64_t available = (int64_t)terminal_width - 1 - start_col - reserved_cols;
    if (terminal_width <= 0 || start_col < 0 || reserved_cols < 0 ||
        available <= 0 || desired == 0) return 0;
    if (available > INT_MAX) available = INT_MAX;
    return desired < (size_t)available ? (int)desired : (int)available;
}

/* The menu's active cell region and extra-field offset both bound the
 * label/name text. Small screens may have no room for a label. */
static inline size_t angdroid_object_list_cell_budget(
    int name_offset, int row_width)
{
    if (name_offset <= 0 || row_width <= 0) return 0;
    return (size_t)(name_offset < row_width ? name_offset : row_width);
}


/* Selection, inventory and equipment share the same preference order for
 * optional numeric fields. Leave at least min_name terminal cells for the
 * item shortcut/name before reserving 9/10-column detail fields.
 * Leave one more cell for the renderer's right-edge margin.
 */
typedef struct {
    bool price;
    bool fail;
    bool recharge;
    bool weight;
    int reserved;
} angdroid_object_list_extras;

static inline angdroid_object_list_extras angdroid_object_list_fit_extras(
    int row_width, int min_name, bool price, bool fail,
    bool recharge, bool weight)
{
    angdroid_object_list_extras out = {false, false, false, false, 0};
    int remaining;
    if (row_width <= 0) return out;
    if (min_name < 0) min_name = 0;
    if (min_name >= row_width) return out;
    remaining = row_width - min_name - 1;

    /* Price/failure information precedes optional weight on narrow phones. */
    if (price && remaining >= 9) {
        out.price = true;
        out.reserved += 9;
        remaining -= 9;
    }
    if (fail && remaining >= 10) {
        out.fail = true;
        out.reserved += 10;
        remaining -= 10;
    }
    if (recharge && remaining >= 10) {
        out.recharge = true;
        out.reserved += 10;
        remaining -= 10;
    }
    if (weight && remaining >= 9) {
        out.weight = true;
        out.reserved += 9;
    }
    return out;
}

/* Never print part of a price or a percent, or overwrite the next column.
 * row_width is the active menu row, not necessarily Term->wid. */
static inline bool angdroid_object_list_detail_fits(
    int offset, int row_width, int reserved, size_t text_columns)
{
    if (offset < 0 || row_width <= 0 || reserved <= 0 ||
        offset > row_width || reserved > row_width - offset)
        return false;
    return text_columns <= (size_t)reserved;
}

#endif
