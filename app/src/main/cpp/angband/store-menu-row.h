#ifndef ANGDROID_STORE_MENU_ROW_H
#define ANGDROID_STORE_MENU_ROW_H

#include <stdint.h>
#include <limits.h>

/* Stock item indices are absolute. The native menu displays only the
 * rows starting at top, so a popup must use the visible screen row. */
static inline int angdroid_store_menu_screen_row(
    int item_index, int top, int first_row, int page_rows)
{
    int64_t visible_row;
    int64_t result;

    if (item_index < 0 || top < 0 || first_row < 0 || page_rows <= 0)
        return -1;
    visible_row = (int64_t)item_index - top;
    if (visible_row < 0 || visible_row >= page_rows)
        return -1;
    result = (int64_t)first_row + visible_row;
    if (result > INT_MAX)
        return -1;
    return (int)result;
}

#endif
