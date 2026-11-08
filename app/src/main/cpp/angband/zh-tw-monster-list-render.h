/* Presentation-only monster-list labels.  Never alter race data or
 * the English count/pluralization grammar used by game logic and exports. */
#ifndef ANGDROID_ZH_TW_MONSTER_LIST_RENDER_H
#define ANGDROID_ZH_TW_MONSTER_LIST_RENDER_H

#include <stddef.h>
#include <stdio.h>
#include "../curses/unicode_display.h"

/* The caller has already resolved a translated unique/non-unique name.
 * Reserve the original [U]/numeric prefix and append only whole UTF-8
 * scalars.  Invalid non-ASCII bytes are replaced by '?' by the helper. */
static inline size_t angdroid_zh_tw_monster_list_copy(
    char *out, size_t capacity, const char *translated,
    int is_unique, int number)
{
    int written;
    if (!out || capacity == 0) return 0;
    out[0] = '\0';
    if (!translated) return 0;

    if (is_unique)
        written = snprintf(out, capacity, "[U] ");
    else
        written = snprintf(out, capacity, "%3d ", number);
    if (written < 0 || (size_t)written >= capacity) {
        out[0] = '\0';
        return 0;
    }

    return angdroid_utf8_append_complete(
        out, capacity, (size_t)written, translated);
}
#endif /* ANGDROID_ZH_TW_MONSTER_LIST_RENDER_H */
