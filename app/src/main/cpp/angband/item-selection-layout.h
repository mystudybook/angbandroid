/* Safe native selection coordinates after Android terminal rotation.
 * This code never changes item IDs, cursor selection, or source descriptions.
 */
#ifndef ANGDROID_ITEM_SELECTION_LAYOUT_H
#define ANGDROID_ITEM_SELECTION_LAYOUT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

typedef struct {
    int col, row, width, page_rows;
} angdroid_item_selection_layout;

/* Number of quiver summary rows, without a potentially overflowing
 * (item_count + slot_size - 1) ceil-division expression. */
static inline int angdroid_item_quiver_footer_slots(
    int item_count, int slot_size)
{
    if (item_count <= 0 || slot_size <= 0)
        return 0;
    return item_count / slot_size + (item_count % slot_size != 0);
}

/* Keep an empty picker visible only when it has another source to switch
 * to. This reserves a blank viewport row for keyboard hints but never
 * changes menu->count or invents a selectable object. */
static inline int angdroid_item_selection_viewport_count(
    int actual_count, bool has_switch_target)
{
    if (actual_count > 0) return actual_count;
    return has_switch_target ? 1 : 0;
}

static inline bool angdroid_item_selection_region(
    int terminal_width, int terminal_height, int prompt_columns,
    size_t name_columns, size_t header_columns, int extra_columns,
    int count, bool reserve_footer, angdroid_item_selection_layout *out)
{
    int64_t desired, col, ceiling, available;
    int rows;
    if (!out) return false;
    out->col = out->row = out->width = out->page_rows = 0;
    if (terminal_width <= 0 || terminal_height <= 1 ||
        count <= 0 || extra_columns < 0) return false;

    if (name_columns > (size_t)INT_MAX ||
        (int64_t)name_columns > INT_MAX - extra_columns)
        desired = INT_MAX;
    else
        desired = (int64_t)name_columns + extra_columns;
    if (header_columns > (size_t)desired)
        desired = header_columns > INT_MAX ? INT_MAX : (int64_t)header_columns;
    if (desired <= 0) desired = 1;
    ceiling = prompt_columns > 2 ? prompt_columns - 2 : 0;
    col = (int64_t)terminal_width - 1 - desired;
    if (col > ceiling) col = ceiling;
    if (col <= 3) col = 0;
    available = terminal_width - col;
    if (available <= 0) return false;

    rows = terminal_height - 1;
    if (reserve_footer && rows > 1) --rows;
    if (rows <= 0) return false;
    out->col = (int)col;
    out->row = 1;
    out->width = desired < available ? (int)desired : (int)available;
    out->page_rows = count < rows ? count : rows;
    return out->width > 0 && out->page_rows > 0;
}

/* Footer appears AFTER the selectable rows, never on an item or offscreen.
 * Return -1 when no terminal cell is available. */
static inline int angdroid_item_selection_footer_row(
    int first_row, int page_rows, int terminal_height, int index)
{
    int64_t row;
    if (first_row < 0 || page_rows < 1 || terminal_height <= 0 || index < 0)
        return -1;
    row = (int64_t)first_row + page_rows + index;
    return row < terminal_height ? (int)row : -1;
}
#endif
