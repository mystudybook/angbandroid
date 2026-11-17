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

/* Append one visible key at most once. A menu may expose the same letter
 * as an inscription, item tag, command or screen switch. Repeated Android
 * buttons give no additional action and can hide useful controls. */
static inline size_t angdroid_menu_hint_append_unique(
    char *text, size_t used, size_t capacity, char key)
{
    size_t i;

    if (!text || !key || used >= capacity ||
        capacity - used <= 1)
        return used;
    for (i = 0; i < used; ++i)
        if (text[i] == key)
            return used;
    text[used++] = key;
    text[used] = '\0';
    return used;
}

/* Command and switch keys need guaranteed space after all visible item
 * tags; their original order is maintained while duplicates are omitted. */
static inline size_t angdroid_menu_hint_append_keys_unique(
    char *text, size_t used, size_t capacity, const char *keys)
{
    if (!keys)
        return used;
    for (; *keys; ++keys)
        used = angdroid_menu_hint_append_unique(
            text, used, capacity, *keys);
    return used;
}

/* Reserve a safe upper bound for fixed keys. Duplicate fixed keys might
 * consume fewer bytes, but never more. This prevents a long inventory
 * from exhausting the keyboard hint buffer before switch/command keys. */
static inline size_t angdroid_menu_hint_tag_capacity(
    size_t capacity, size_t used, const char *commands,
    const char *switches)
{
    size_t reserved = 0;
    size_t remaining, i;
    const char *keys[2] = {commands, switches};

    if (used >= capacity)
        return 0;
    remaining = capacity - used - 1;
    for (i = 0; i < 2; ++i) {
        const char *p = keys[i];
        if (!p) continue;
        while (*p && remaining) {
            ++reserved;
            --remaining;
            ++p;
        }
    }
    return capacity - reserved;
}

#endif
