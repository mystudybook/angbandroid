/* Atomic numeric and ASCII punctuation fragments in object descriptions.
 * The caller still decides which stats and charges the player can see.
 */
#ifndef ANGDROID_OBJ_DESC_NUMERIC_UTF8_H
#define ANGDROID_OBJ_DESC_NUMERIC_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Format a complete numeric/status fragment before touching the destination.
 * All production format strings are literals, and their UTF-8 punctuation
 * (if any) is never sliced by the destination buffer's byte limit.
 */
static inline bool angdroid_obj_desc_append_formatted_detail(char *out,
    size_t capacity, size_t *end, const char *fmt, ...)
{
    char text[128];
    va_list args;
    int n;
    if (!out || !capacity || !end || *end >= capacity || !fmt)
        return false;
    va_start(args, fmt);
    n = vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);
    if (n <= 0 || (size_t)n >= sizeof(text) ||
        (size_t)n >= capacity - *end) return false;
    memcpy(out + *end, text, (size_t)n + 1);
    *end += (size_t)n;
    return true;
}

/* Preserve original distinct known modifier values and their order.
 * '<' and '>' must fit together with every number, or append nothing.
 */
static inline bool angdroid_obj_desc_append_modifiers_ascii(char *out,
    size_t capacity, size_t *end, const int *mods, size_t count)
{
    char text[512];
    size_t used = 0, i;
    if (!out || !capacity || !end || *end >= capacity ||
        !mods || !count) return false;
    for (i = 0; i < count; ++i) {
        int n = snprintf(text + used, sizeof(text) - used,
            "%s%+d", i ? ", " : " <", mods[i]);
        if (n < 0 || (size_t)n >= sizeof(text) - used) return false;
        used += (size_t)n;
    }
    if (used >= sizeof(text) - 1) return false;
    text[used++] = '>';
    text[used] = '\0';
    if (used >= capacity - *end) return false;
    memcpy(out + *end, text, used + 1);
    *end += used;
    return true;
}

#endif /* ANGDROID_OBJ_DESC_NUMERIC_UTF8_H */
