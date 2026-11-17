/* Screen-only geometry and name/distance budgets for visible object lists. */
#ifndef ANGDROID_OBJECT_LIST_COMPACT_LAYOUT_H
#define ANGDROID_OBJECT_LIST_COMPACT_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>

struct angdroid_object_list_popup {
    int col;
    int row;
    int width;
    int height;
};

/* The interactive textblock needs one body row plus two footer rows,
 * positioned below the main game's top status line. Preserve thirteen
 * map columns on roomy terminals but use almost full width on phones. */
static inline bool angdroid_object_list_popup_fit(int screen_width,
    int screen_height, size_t preferred_width, size_t preferred_height,
    struct angdroid_object_list_popup *out)
{
    int available_width, available_height, popup_width;
    size_t body_rows, max_body_rows;

    if (!out) return false;
    out->col = out->row = out->width = out->height = 0;
    if (screen_width < 12 || screen_height < 7) return false;

    available_width = screen_width >= 48 ? screen_width - 13 : screen_width - 2;
    available_height = screen_height - 3;
    popup_width = preferred_width > 0 &&
        preferred_width < (size_t)available_width ?
        (int)preferred_width : available_width;

    if (popup_width < 7) popup_width = 7;

    max_body_rows = (size_t)(available_height - 3);
    body_rows = preferred_height > 0 ? preferred_height : 1;
    if (body_rows > max_body_rows) body_rows = max_body_rows;

    out->col = screen_width - popup_width;
    out->row = 1;
    out->width = popup_width;
    out->height = (int)body_rows + 3;
    return true;
}

struct angdroid_object_row_budget {
    size_t name_columns;
    bool show_location;
};

/* Prioritize an item's name. Its distance is only displayed when it
 * leaves space for the count prefix and at least two Han glyphs. */
static inline struct angdroid_object_row_budget
angdroid_object_row_fit(int width, size_t location_columns)
{
    struct angdroid_object_row_budget result = { 0, false };
    size_t available;
    if (width <= 3) return result;
    available = (size_t)(width - 3);
    if (location_columns != 0 && location_columns <= available &&
        available - location_columns >= 8) {
        result.name_columns = available - location_columns;
        result.show_location = true;
    } else {
        result.name_columns = available;
    }
    return result;
}

#endif /* ANGDROID_OBJECT_LIST_COMPACT_LAYOUT_H */
