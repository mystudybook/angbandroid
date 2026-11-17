/* Display-only item list prefix layout. Never rewrite stored object names. */
#ifndef ANGDROID_OBJECT_LIST_NAME_DISPLAY_H
#define ANGDROID_OBJECT_LIST_NAME_DISPLAY_H

#include <stddef.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* Preserve Angband's three-column ASCII count/article when one exists.
 * Non-ASCII or longer first words are part of the name, not a numeric
 * prefix: retain the whole string after the four-column indent. */
static inline size_t angdroid_object_list_name_copy(char *out,
    size_t capacity, const char *name, int split_prefix)
{
    const char *rest = name;
    const char *space;
    size_t i, prefix_bytes = 0, used = 0;
    int ascii_prefix = 0;

    if (!out || capacity == 0) return 0;
    out[0] = '\0';
    if (!name) return 0;

    /* For very small buffers, show text rather than only an indent. */
    if (capacity <= 5)
        return angdroid_utf8_copy_complete(out, capacity, name);

    space = split_prefix ? strchr(name, ' ') : NULL;
    if (space && space > name && (size_t)(space - name) <= 3) {
        ascii_prefix = 1;
        prefix_bytes = (size_t)(space - name);
        for (i = 0; i < prefix_bytes; ++i) {
            unsigned char ch = (unsigned char)name[i];
            if (ch < 0x21u || ch > 0x7Eu) {
                ascii_prefix = 0;
                break;
            }
        }
    }

    if (ascii_prefix) {
        for (i = prefix_bytes; i < 3; ++i) out[used++] = ' ';
        for (i = 0; i < prefix_bytes; ++i) out[used++] = name[i];
        out[used++] = ' ';
        rest = space + 1;
        while (*rest == ' ') ++rest;
    } else {
        for (i = 0; i < 4; ++i) out[used++] = ' ';
    }
    out[used] = '\0';
    return angdroid_utf8_append_complete(out, capacity, used, rest);
}

#endif /* ANGDROID_OBJECT_LIST_NAME_DISPLAY_H */
