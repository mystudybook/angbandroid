/* Replace, rather than accumulate, the modal menu's Android key hints.
 * The caller provides a distinct source buffer. Keys are single-byte
 * shortcut labels; drop duplicate bytes and always terminate the output.
 */
#ifndef ANGDROID_SOFT_KBD_REPLACE_H
#define ANGDROID_SOFT_KBD_REPLACE_H

#include <stddef.h>

static inline size_t angdroid_soft_kbd_replace_buffer(
    char *output, size_t capacity, const char *keys)
{
    size_t used = 0;

    if (!output || capacity == 0) return 0;
    output[0] = '\0';
    if (!keys) return 0;

    while (*keys) {
        size_t i;
        for (i = 0; i < used; ++i) {
            if (output[i] == *keys) break;
        }
        if (i == used) {
            if (used >= capacity - 1) break;
            output[used++] = *keys;
            output[used] = '\0';
        }
        ++keys;
    }
    return used;
}

#endif /* ANGDROID_SOFT_KBD_REPLACE_H */
