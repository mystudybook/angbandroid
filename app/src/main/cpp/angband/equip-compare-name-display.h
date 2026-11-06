/* A display-cell-bounded equipment-compare ROW label.
 * The canonical short_name remains the source for sorting and file export.
 * Preserve complete UTF-8 scalars; avoid Term_putstr(n) byte/column confusion.
 */
#ifndef ANGDROID_EQUIP_COMPARE_NAME_DISPLAY_H
#define ANGDROID_EQUIP_COMPARE_NAME_DISPLAY_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "menu-text-fit.h"

static inline bool angdroid_equip_cmp_format_short_name(
    char *out, size_t capacity, const char *name, size_t columns,
    bool keep_tail)
{
    const char *source = name;
    size_t total, used, length;

    if (!out || capacity == 0) return false;
    out[0] = '\0';
    if (!name || columns == 0) return false;

    total = angdroid_utf8_display_columns(name);
    if (keep_tail && total > columns) {
        /* Skipping part of a wide glyph must skip that whole glyph. */
        source = angdroid_utf8_skip_columns(name, total - columns);
    }

    used = angdroid_menu_copy_columns(out, capacity, source, columns);
    length = strlen(out);

    /* Fill any unoccupied display cells, so a previous longer name
     * cannot leave stale glyphs behind during in-place refresh. */
    while (used < columns) {
        if (length >= capacity - 1) {
            out[0] = '\0';
            return false;
        }
        out[length++] = ' ';
        out[length] = '\0';
        ++used;
    }
    return true;
}
#endif
