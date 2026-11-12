/* Keyboard movement in column-major menus, shared by Angband/FAangband.
 * Rows displayed in the final column may be fewer than page_rows. Never
 * form a cursor beyond count, and never visit hidden/disabled view rows.
 */
#ifndef ANGDROID_MENU_COLUMN_NAVIGATION_H
#define ANGDROID_MENU_COLUMN_NAVIGATION_H

#include "menu-valid-cursor.h"

static inline int angdroid_menu_column_next_valid(
    int cursor, int count, int page_rows, int dx, int dy,
    void *context, angdroid_menu_row_valid_fn is_valid)
{
    int column, row, columns, attempt;

    if (count <= 0 || page_rows <= 0 || !is_valid ||
        (dx != 0 && dy != 0) || (dx == 0 && dy == 0))
        return -1;

    /* Stock changes may clear the cursor. A direction key can still
     * recover selection, without referencing a stale item. */
    if (cursor < 0 || cursor >= count) {
        if (dx < 0 || dy < 0) {
            for (attempt = count - 1; attempt >= 0; --attempt)
                if (is_valid(context, attempt)) return attempt;
        } else {
            for (attempt = 0; attempt < count; ++attempt)
                if (is_valid(context, attempt)) return attempt;
        }
        return -1;
    }

    column = cursor / page_rows;
    row = cursor % page_rows;
    columns = 1 + (count - 1) / page_rows;

    if (dx) {
        int next_column = column;
        /* Keep the physical row across columns. If the last column is
         * shorter, use its final actual row rather than wrapping early.
         * All columns are tried once; disabled/hidden rows are skipped. */
        for (attempt = 0; attempt < columns - 1; ++attempt) {
            int base, candidate, available;
            if (dx > 0)
                next_column = next_column == columns - 1 ?
                    0 : next_column + 1;
            else
                next_column = next_column == 0 ?
                    columns - 1 : next_column - 1;
            base = next_column * page_rows;
            available = count - base;
            candidate = base + (row < available ? row : available - 1);
            if (candidate != cursor && is_valid(context, candidate))
                return candidate;
        }
        return -1;
    }

    /* Vertical movement stays inside its current column. Wrapping at a
     * short final column must not move into a different column. */
    {
        int base = column * page_rows;
        int length = count - base;
        int next_row = row;
        if (length > page_rows) length = page_rows;
        for (attempt = 0; attempt < length - 1; ++attempt) {
            if (dy > 0)
                next_row = next_row == length - 1 ? 0 : next_row + 1;
            else
                next_row = next_row == 0 ? length - 1 : next_row - 1;
            if (is_valid(context, base + next_row))
                return base + next_row;
        }
    }

    return -1;
}

#endif /* ANGDROID_MENU_COLUMN_NAVIGATION_H */
