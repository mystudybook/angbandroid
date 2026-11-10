/* A byte-limited printf precision must not end inside a UTF-8 scalar.
 * This helper changes only the number of bytes retained; printf retains
 * responsibility for field padding, left alignment, and integer formatting.
 * Strings are NUL-terminated. Invalid input bytes remain the caller's concern.
 */
#ifndef ANGDROID_ZH_TW_FORMAT_UTF8_H
#define ANGDROID_ZH_TW_FORMAT_UTF8_H

#include <stddef.h>
#include "../curses/unicode_display.h"

static inline size_t angdroid_utf8_precision_prefix_bytes(
    const char *text, size_t byte_limit)
{
    size_t used = 0;

    if (!text) return 0;
    while (used < byte_limit && text[used]) {
        uint32_t scalar;
        size_t bytes = angdroid_utf8_next_scalar(text + used, &scalar);
        if (!bytes || bytes > byte_limit - used) break;
        used += bytes;
    }
    return used;
}
#endif
