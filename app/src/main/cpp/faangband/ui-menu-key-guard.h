/* Safe view indices and soft-key labels for FAangband menu lists.
 * A filtered view may map each screen row to a different source item.
 * Keep key-label buffers bounded even when a menu has thousands of items.
 */
#ifndef ANGDROID_FA_MENU_KEY_GUARD_H
#define ANGDROID_FA_MENU_KEY_GUARD_H

#include <stddef.h>

static inline int angdroid_fa_menu_source_index(
    const int *filter, int position, int visible_count, int source_count)
{
    int source;
    if (visible_count <= 0 || source_count <= 0 ||
        position < 0 || position >= visible_count)
        return -1;
    source = filter ? filter[position] : position;
    return source >= 0 && source < source_count ? source : -1;
}

static inline char angdroid_fa_menu_selection_key(
    const char *selections, int position)
{
    size_t i;
    if (!selections || position < 0) return 0;
    for (i = 0; i < (size_t)position && selections[i]; ++i) {}
    return i == (size_t)position ? selections[i] : 0;
}

/* Append only complete one-byte shortcut keys. Caller owns 'used' and
 * initializes it to strlen(output); no partial write at capacity. */
static inline int angdroid_fa_menu_add_key(
    char *output, size_t capacity, size_t *used, char key)
{
    if (!output || !used || !capacity || !key ||
        *used >= capacity || capacity - *used <= 1)
        return 0;
    output[*used] = key;
    ++*used;
    output[*used] = '\0';
    return 1;
}

/* Relative tags refer only to currently visible page rows. */
static inline int angdroid_fa_menu_page_rows(
    int count, int top, int page_rows)
{
    if (count <= 0 || top < 0 || top >= count || page_rows <= 0)
        return 0;
    return page_rows < count - top ? page_rows : count - top;
}

#endif /* ANGDROID_FA_MENU_KEY_GUARD_H */
