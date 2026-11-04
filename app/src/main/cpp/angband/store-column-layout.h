#ifndef ANGDROID_STORE_COLUMN_LAYOUT_H
#define ANGDROID_STORE_COLUMN_LAYOUT_H

#include <stdint.h>

/* Terminal cell coordinates for a single store row. Optional columns are
 * -1 when they cannot fit without swallowing the item name.
 * Price takes priority over weight in non-home stores. */
typedef struct {
    int width;
    int name_col;
    int name_cols;
    int price_col;
    int weight_col;
    int gold_col;
    int owner_end;
} angdroid_store_column_layout;

static inline angdroid_store_column_layout angdroid_store_columns(
    int terminal_width, int name_col, int is_home)
{
    angdroid_store_column_layout out = {0};
    int left;

    out.price_col = -1;
    out.weight_col = -1;
    if (terminal_width <= 0)
        return out;
    out.width = terminal_width < 104 ? terminal_width : 104;
    out.name_col = name_col >= 0 ? name_col : 0;
    out.gold_col = out.width > 26 ? out.width - 26 : 0;
    out.owner_end = out.width > 2 ? out.width - 2 : out.width;

    if (out.name_col >= out.width)
        return out;
    left = out.width - out.name_col;
    out.name_cols = left;

    if (is_home) {
        if (left >= 27) {
            out.weight_col = out.width - 14;
            out.name_cols = out.weight_col - out.name_col - 1;
        }
    } else {
        if (left >= 27) {
            out.price_col = out.width - 14;
            out.name_cols = out.price_col - out.name_col - 1;
        }
        if (left >= 37) {
            out.weight_col = out.width - 24;
            out.name_cols = out.weight_col - out.name_col - 1;
        }
    }
    return out;
}

/* A row may belong to a smaller embedded menu region than the full screen.
 * Limit it to the first active numeric column and its own cell width. */
static inline int angdroid_store_name_budget(
    angdroid_store_column_layout layout, int row_col, int row_width)
{
    int64_t end = (int64_t)row_col + row_width;
    int first_detail = layout.weight_col >= 0 ? layout.weight_col :
        layout.price_col;

    if (row_col < 0 || row_width <= 0 || row_col >= layout.width)
        return 0;
    if (end > layout.width)
        end = layout.width;
    if (first_detail >= 0 && end > first_detail - 1)
        end = first_detail - 1;
    return end > row_col ? (int)(end - row_col) : 0;
}

/* Footer/header coordinates must also stay nonnegative on tiny surfaces.
 * A zero-height screen cannot draw; callers check current term height. */
static inline int angdroid_store_safe_row(int row, int height)
{
    if (height <= 0 || row < 0) return 0;
    return row >= height ? height - 1 : row;
}

#endif
