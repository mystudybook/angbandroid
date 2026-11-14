/* Append a visible artifact/ego suffix as one complete UTF-8 text unit. */
#ifndef ANGDROID_OBJ_DESC_SUFFIX_UTF8_H
#define ANGDROID_OBJ_DESC_SUFFIX_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* The caller has already established that the suffix is known/visible.
 * Keep its spelling unchanged in both English and Traditional Chinese.
 * Reject a suffix as a whole if it would leave a dangling leading space,
 * unmatched decoration, truncated character, or invalid UTF-8. */
static inline bool angdroid_obj_desc_append_suffix_atomic(char *out,
    size_t capacity, size_t *end, const char *suffix, bool leading_space)
{
    size_t used, length, pos = 0, padding = leading_space ? 1u : 0u;

    if (!out || !end || !suffix || !suffix[0] || !capacity ||
        *end >= capacity) return false;
    used = *end;
    length = strlen(suffix);
    if (capacity - used <= padding ||
        length >= capacity - used - padding) return false;

    /* Validate before changing the output, so failures are atomic. */
    while (pos < length) {
        uint32_t scalar;
        size_t bytes = angdroid_utf8_next_scalar(suffix + pos, &scalar);
        if (!bytes || bytes > length - pos ||
            (bytes == 1 && scalar == 0xFFFDu &&
             (unsigned char)suffix[pos] >= 0x80u)) return false;
        pos += bytes;
    }

    if (leading_space) out[used++] = ' ';
    memcpy(out + used, suffix, length);
    used += length;
    out[used] = '\0';
    *end = used;
    return true;
}

#endif /* ANGDROID_OBJ_DESC_SUFFIX_UTF8_H */
