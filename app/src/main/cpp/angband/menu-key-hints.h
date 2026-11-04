#ifndef ANGDROID_MENU_KEY_HINTS_H
#define ANGDROID_MENU_KEY_HINTS_H

#include <stddef.h>

/* Menu filters store underlying item indexes, not visible row numbers.
 * Keep soft-keyboard hints consistent with the items actually displayed. */
static inline int angdroid_menu_hint_item_index(
    const int *filter_list, int visible_index)
{
    return filter_list ? filter_list[visible_index] : visible_index;
}

/* Append a single shortcut, reserving the final byte for NUL.
 * Large menus must not overflow the fixed-size keyboard hint buffer. */
static inline size_t angdroid_menu_hint_append(
    char *text, size_t used, size_t capacity, char shortcut)
{
    if (text && shortcut && used < capacity && capacity - used > 1) {
        text[used++] = shortcut;
        text[used] = '\0';
    }
    return used;
}

#endif
