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

#endif
