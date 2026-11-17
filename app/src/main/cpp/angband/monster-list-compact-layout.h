/* Screen-only geometry and field budgets for the monster list. */
#ifndef ANGDROID_MONSTER_LIST_COMPACT_LAYOUT_H
#define ANGDROID_MONSTER_LIST_COMPACT_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>

struct angdroid_monster_list_popup {
    int col;
    int row;
    int width;
    int height;
};

/* Interactive textblocks reserve one header and two footer rows.
 * A one-row body requires four region rows, positioned below row zero.
 * Keep a 40-column map area at regular widths, but allow nearly the
 * whole display on narrow mobile terminals. */
static inline bool angdroid_monster_list_popup_fit(int screen_width,
    int screen_height, size_t preferred_width, size_t preferred_height,
    struct angdroid_monster_list_popup *out)
{
    int available_width, available_height, popup_width;
    size_t body_rows, max_body_rows;

    if (!out) return false;
    out->col = out->row = out->width = out->height = 0;
    if (screen_width < 12 || screen_height < 7) return false;

    available_width = screen_width > 42 ? screen_width - 40 : screen_width - 2;
    available_height = screen_height - 3;
    popup_width = preferred_width > 0 &&
        preferred_width < (size_t)available_width ?
        (int)preferred_width : available_width;
    if (popup_width < 4) popup_width = 4;

    /* The subtraction is safe after the minimum-height check above. */
    max_body_rows = (size_t)(available_height - 3);
    body_rows = preferred_height > 0 ? preferred_height : 1;
    if (body_rows > max_body_rows) body_rows = max_body_rows;

    out->col = screen_width - popup_width;
    out->row = 1;
    out->width = popup_width;
    out->height = (int)body_rows + 3;
    return true;
}

struct angdroid_monster_row_budget {
    size_t name_columns;
    bool show_location;
};

/* The symbol/space/safety margin takes three columns. Only preserve
 * the optional distance when at least four name columns remain. */
static inline struct angdroid_monster_row_budget
angdroid_monster_row_fit(int width, size_t location_columns)
{
    struct angdroid_monster_row_budget result = { 0, false };
    size_t content;
    if (width <= 3) return result;
    content = (size_t)(width - 3);
    if (location_columns > 0 && content >= location_columns &&
        content - location_columns >= 4) {
        result.name_columns = content - location_columns;
        result.show_location = true;
    } else {
        result.name_columns = content;
    }
    return result;
}

/* Sleep status is decorative; when space is scarce, show the name. */
static inline bool angdroid_monster_sleep_fits(size_t name_columns,
    size_t sleep_columns)
{
    return sleep_columns > 0 && name_columns >= sleep_columns &&
        name_columns - sleep_columns >= 4;
}

#endif /* ANGDROID_MONSTER_LIST_COMPACT_LAYOUT_H */
