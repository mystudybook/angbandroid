/* Render-only monster spell tag matching and UTF-8-safe appending.
 * No spell damage, hit/miss decision, monster race ID, RNG or sound changes.
 */
#ifndef ANGDROID_ZH_TW_MON_SPELL_RENDER_H
#define ANGDROID_ZH_TW_MON_SPELL_RENDER_H
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* The parser gives [tag, tag+bytes): never match just a leading prefix. */
static inline int angdroid_mon_spell_tag_matches(const char *tag,
    size_t bytes, const char *known)
{
    return tag && known && strlen(known) == bytes &&
        memcmp(tag, known, bytes) == 0;
}

/* Copy a bounded slice of a translated template, name or pronoun without
 * splitting UTF-8 scalars.  Invalid source bytes are shown as '?' instead
 * of putting broken multi-byte sequences into the message terminal. */
static inline size_t angdroid_mon_spell_append_utf8(
    char *dst, size_t capacity, size_t used, const char *source, size_t bytes)
{
    size_t offset = 0;
    uint32_t scalar;
    size_t step, i;
    if (!dst || capacity == 0 || used >= capacity || !source) return used;
    dst[used] = '\0';
    while (offset < bytes && source[offset]) {
        step = angdroid_utf8_next_scalar(source + offset, &scalar);
        if (!step || step > bytes - offset ||
            step > capacity - used - 1) break;
        if (step == 1 && scalar == 0xFFFDu &&
            (unsigned char)source[offset] >= 0x80u) {
            dst[used++] = '?';
            ++offset;
            continue;
        }
        for (i = 0; i < step; ++i)
            dst[used++] = source[offset + i];
        offset += step;
    }
    dst[used] = '\0';
    return used;
}

#endif
