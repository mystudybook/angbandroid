/* FAangband menu labels and terminal geometry. Source strings remain
 * untouched: screen text is copied only as complete UTF-8 scalars, with
 * positions and widths measured in terminal display cells, not bytes.
 */
#ifndef ANGDROID_FA_MENU_CELL_TEXT_H
#define ANGDROID_FA_MENU_CELL_TEXT_H

#include <stddef.h>
#include "../angband/menu-text-fit.h"

static inline int angdroid_fa_menu_visible_columns(
    int terminal_width, int terminal_height,
    int row, int col, int requested)
{
    int remaining;
    if (terminal_width <= 0 || terminal_height <= 0 ||
        row < 0 || row >= terminal_height ||
        col < 0 || col >= terminal_width || requested <= 0)
        return 0;
    remaining = terminal_width - col;
    return requested < remaining ? requested : remaining;
}

/* Return visible display columns; destination is always NUL terminated
 * when writable. A glyph that cannot fit is omitted as a whole. */
static inline size_t angdroid_fa_menu_copy_visible(
    char *out, size_t capacity, const char *source,
    int terminal_width, int terminal_height,
    int row, int col, int requested)
{
    int cells;
    if (!out || capacity == 0) return 0;
    out[0] = '\0';
    cells = angdroid_fa_menu_visible_columns(
        terminal_width, terminal_height, row, col, requested);
    if (!cells) return 0;
    return angdroid_menu_copy_columns(out, capacity, source, (size_t)cells);
}

#endif /* ANGDROID_FA_MENU_CELL_TEXT_H */
