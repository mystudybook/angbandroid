/* Preserve complete UTF-8 effect names and their surrounding punctuation. */
#ifndef ANGDROID_OBJ_DESC_EFFECT_UTF8_H
#define ANGDROID_OBJ_DESC_EFFECT_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* These are display-only suffixes: the caller has already checked whether
 * the object's effect is known.  Keep the full original source label (even
 * when no translated name exists), or append nothing if it will not fit.
 * Never produce a dangling 'of', apostrophe, or Chinese parenthesis. */
static inline bool angdroid_obj_desc_append_effect_wrapped(char *out,
    size_t capacity, size_t *end, const char *prefix, const char *effect,
    const char *suffix)
{
    size_t start, a, b, c, remaining;
    const char *segments[3];
    size_t i;

    if (!out || !end || !capacity || *end >= capacity ||
        !prefix || !effect || !*effect || !suffix) return false;
    start = *end;
    a = strlen(prefix);
    b = strlen(effect);
    c = strlen(suffix);
    remaining = capacity - start;
    if (a >= remaining) return false;
    remaining -= a;
    if (b >= remaining) return false;
    remaining -= b;
    if (c >= remaining) return false;

    /* Inspect every UTF-8 scalar before touching the output, so a failure
     * does not leave half a wide character or an unmatched delimiter. */
    segments[0] = prefix;
    segments[1] = effect;
    segments[2] = suffix;
    for (i = 0; i < 3; ++i) {
        const char *p = segments[i];
        while (*p) {
            uint32_t cp;
            size_t n = angdroid_utf8_next_scalar(p, &cp);
            if (!n || (n == 1 && cp == 0xFFFDu &&
                (unsigned char)*p >= 0x80u)) return false;
            p += n;
        }
    }

    memcpy(out + start, prefix, a);
    memcpy(out + start + a, effect, b);
    memcpy(out + start + a + b, suffix, c);
    out[start + a + b + c] = '\0';
    *end = start + a + b + c;
    return true;
}

static inline bool angdroid_obj_desc_append_scroll_effect(char *out,
    size_t capacity, size_t *end, const char *effect)
{
    return angdroid_obj_desc_append_effect_wrapped(out, capacity, end,
        "（", effect, "）");
}

static inline bool angdroid_obj_desc_append_flavor_effect(char *out,
    size_t capacity, size_t *end, const char *effect, bool terse)
{
    return angdroid_obj_desc_append_effect_wrapped(out, capacity, end,
        terse ? " '" : " of ", effect, terse ? "'" : "");
}

#endif /* ANGDROID_OBJ_DESC_EFFECT_UTF8_H */
