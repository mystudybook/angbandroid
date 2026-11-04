#ifndef ANGDROID_OBJECT_LIST_TEXT_H
#define ANGDROID_OBJECT_LIST_TEXT_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "menu-text-fit.h"

/* Space available for an item name after fixed extra-field columns.
 * An int below zero must never become a huge unsigned size_t. */
static inline int angdroid_object_list_name_offset(
    int terminal_width, int start_col, int reserved_cols, size_t desired)
{
    int64_t available = (int64_t)terminal_width - 1 - start_col - reserved_cols;
    if (available <= 0 || desired == 0) return 0;
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

#endif
