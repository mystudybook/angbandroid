#ifndef ANGDROID_MENU_VALID_CURSOR_H
#define ANGDROID_MENU_VALID_CURSOR_H

/* Find the next selectable row in the requested direction. In a column
 * menu, horizontal navigation may land on an invalid row: dy is zero,
 * so repeatedly adding dy would hang the event loop. */
typedef int (*angdroid_menu_row_valid_fn)(void *context, int row);

static inline int angdroid_menu_find_valid_cursor(
    int cursor, int count, int direction, void *context,
    angdroid_menu_row_valid_fn is_valid)
{
    int checked;
    int step;

    if (count <= 0 || !is_valid) return -1;
    cursor %= count;
    if (cursor < 0) cursor += count;
    step = direction < 0 ? -1 : 1;

    for (checked = 0; checked < count; ++checked) {
        if (is_valid(context, cursor)) return cursor;
        if (step > 0)
            cursor = cursor == count - 1 ? 0 : cursor + 1;
        else
            cursor = cursor == 0 ? count - 1 : cursor - 1;
    }
    return -1;
}

/* Spacebar page navigation returns a valid target INDEX, never count.
 * The last visible item may itself be the next page's first item.
 * Return -1 when no pages exist. This helper never selects an object. */
static inline int angdroid_menu_page_next_cursor(
    int cursor, int count, int page_rows)
{
    if (count <= 0 || page_rows <= 0 || page_rows >= count)
        return -1;
    if (cursor < 0 || cursor >= count)
        return 0;
    if (cursor >= count - page_rows)
        return 0;
    return cursor + page_rows;
}

#endif
