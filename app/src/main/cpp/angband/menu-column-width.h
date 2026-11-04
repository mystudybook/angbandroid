#ifndef ANGDROID_MENU_COLUMN_WIDTH_H
#define ANGDROID_MENU_COLUMN_WIDTH_H

/* Keep menu hit-testing and drawing on the same nonzero column width.
 * A very narrow terminal can contain fewer character cells than menu
 * columns. This avoids dividing by zero; it does not reflow all items. */
static inline int angdroid_menu_fit_column_width(
    int preferred, int terminal_width, int start, int column_count)
{
    int available;
    int fitted;

    if (preferred < 1 || column_count <= 0 || terminal_width <= start)
        return 1;
    available = terminal_width - start;
    fitted = available / column_count;
    if (fitted < 1) return 1;
    return fitted < preferred ? fitted : preferred;
}

#endif
