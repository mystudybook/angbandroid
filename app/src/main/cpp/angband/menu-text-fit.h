#ifndef ANGDROID_MENU_TEXT_FIT_H
#define ANGDROID_MENU_TEXT_FIT_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* Copy complete UTF-8 codepoints into a bounded display-cell region.
 * Never rely on Term_putstr(n) for width: n counts codepoints, not columns.
 * Malformed byte sequences become '?' rather than leaking partial UTF-8. */
static inline size_t angdroid_menu_copy_columns(
    char *dest, size_t capacity, const char *source, size_t max_columns)
{
    size_t bytes = 0;
    size_t columns = 0;

    if (!dest || capacity == 0) return 0;
    dest[0] = '\0';
    if (!source) return 0;

    while (*source) {
        uint32_t codepoint = 0;
        size_t length = angdroid_utf8_next_scalar(source, &codepoint);
        size_t width;
        int invalid;

        if (length == 0) break;
        width = (size_t)angdroid_terminal_columns(codepoint);
        if (width > max_columns - columns) break;
        invalid = length == 1 && (unsigned char)*source >= 0x80;
        if (invalid) {
            if (bytes + 1 >= capacity) break;
            dest[bytes++] = '?';
        } else {
            if (length >= capacity - bytes) break;
            memcpy(dest + bytes, source, length);
            bytes += length;
        }
        columns += width;
        dest[bytes] = '\0';
        source += length;
    }
    return columns;
}

/* Label prefix must not consume more columns than the row owns. */
static inline int angdroid_menu_tag_columns(int available)
{
    return available <= 0 ? 0 : available < 3 ? available : 3;
}

#endif
