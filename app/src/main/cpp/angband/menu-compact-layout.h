#ifndef ANGDROID_MENU_COMPACT_LAYOUT_H
#define ANGDROID_MENU_COMPACT_LAYOUT_H

/* UI decorations are optional on small terminals; selection rows are not.
 * All coordinates and sizes are terminal cells, not Android pixels. */
typedef struct {
    int col;
    int row;
    int width;
    int page_rows;
    int title_row;
    int header_row;
    int prompt_row;
} angdroid_menu_compact_layout;

static inline int angdroid_menu_compute_layout(
    int terminal_width, int terminal_height,
    int outer_col, int outer_row, int outer_width, int outer_rows,
    int title, int header, int prompt,
    angdroid_menu_compact_layout *out)
{
    int width;
    int rows;
    if (!out || terminal_width <= 0 || terminal_height <= 0 ||
        outer_col < 0 || outer_col >= terminal_width ||
        outer_row < 0 || outer_row >= terminal_height ||
        outer_width <= 0 || outer_rows <= 0)
        return 0;

    width = outer_width;
    rows = outer_rows;
    if (width > terminal_width - outer_col)
        width = terminal_width - outer_col;
    if (rows > terminal_height - outer_row)
        rows = terminal_height - outer_row;

    out->col = outer_col;
    out->row = outer_row;
    out->width = width;
    out->page_rows = rows;
    out->title_row = -1;
    out->header_row = -1;
    out->prompt_row = -1;

    if (title && out->page_rows >= 3) {
        out->title_row = out->row;
        out->row += 2;
        out->page_rows -= 2;
        /* Avoid the historical x+4 offset without subtracting width. */
        if (out->width > 4) {
            out->col += 4;
            out->width -= 4;
        }
    }
    if (header && out->page_rows >= 2) {
        out->header_row = out->row;
        ++out->row;
        --out->page_rows;
    }
    if (prompt && out->page_rows >= 2) {
        --out->page_rows;
        out->prompt_row = out->row + out->page_rows;
    }
    return 1;
}

/* If all columns cannot fit readable labels, a scrolling list is safer
 * than drawing unreachable columns offscreen on a portrait terminal. */
static inline int angdroid_menu_use_compact_columns(
    int count, int page_rows, int available_width, int min_column_width)
{
    int columns;

    if (count <= 0 || page_rows <= 0 || available_width <= 0 ||
        min_column_width <= 0)
        return 0;
    columns = 1 + (count - 1) / page_rows;
    return columns > 1 && columns > available_width / min_column_width;
}

#endif
