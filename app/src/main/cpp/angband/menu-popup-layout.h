#ifndef ANGDROID_MENU_POPUP_LAYOUT_H
#define ANGDROID_MENU_POPUP_LAYOUT_H

#include <stddef.h>

/* A popup needs at least one actual screen cell. Preserve a one-cell
 * border only when the terminal is large enough to afford it. */
typedef struct {
    int col;
    int row;
    int width;
    int page_rows;
} angdroid_menu_popup_box;

static inline int angdroid_menu_popup_bounds(
    int term_width, int term_height, int click_x, int click_y,
    size_t longest_label_columns, int item_count,
    angdroid_menu_popup_box *out)
{
    int margin_x, margin_y;
    int max_width, max_rows;
    size_t wanted_width;
    int col, row, last_col, last_row;

    if (!out) return 0;
    out->col = 0;
    out->row = 0;
    out->width = 0;
    out->page_rows = 0;
    if (term_width <= 0 || term_height <= 0 || item_count <= 0)
        return 0;

    margin_x = term_width >= 3 ? 1 : 0;
    margin_y = term_height >= 3 ? 1 : 0;
    max_width = term_width - 2 * margin_x;
    max_rows = term_height - 2 * margin_y;

    /* Label + three tag cells + two padding cells, without overflowing
     * size_t for unreasonably long imported labels. */
    if (longest_label_columns >= (size_t)max_width)
        wanted_width = (size_t)max_width;
    else {
        size_t free_columns = (size_t)max_width - longest_label_columns;
        wanted_width = longest_label_columns +
            (free_columns < 5 ? free_columns : 5);
    }
    out->width = wanted_width ? (int)wanted_width : 1;
    out->page_rows = item_count < max_rows ? item_count : max_rows;

    last_col = term_width - margin_x - out->width;
    last_row = term_height - margin_y - out->page_rows;
    /* Compare before adding one to avoid INT_MAX anchor overflow. */
    col = click_x < margin_x - 1 ? margin_x :
        click_x >= last_col - 1 ? last_col : click_x + 1;
    row = click_y < margin_y - 1 ? margin_y :
        click_y >= last_row - 1 ? last_row : click_y + 1;

    out->col = col;
    out->row = row;
    return 1;
}

#endif
