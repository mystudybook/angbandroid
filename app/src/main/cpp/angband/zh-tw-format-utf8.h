/* A byte-limited printf precision must not end inside a UTF-8 scalar.
 * This helper changes only the number of bytes retained; printf retains
 * responsibility for field padding, left alignment, and integer formatting.
 * Strings are NUL-terminated. Malformed bytes can be replaced with '?'
 * using the sanitizing copy routine below.
 */
#ifndef ANGDROID_ZH_TW_FORMAT_UTF8_H
#define ANGDROID_ZH_TW_FORMAT_UTF8_H

#include <stddef.h>
#include "../curses/unicode_display.h"

/* Repair an incomplete scalar at the byte boundary of a fixed-size
 * format buffer. Keep the byte count unchanged, so vstrnfmt() can still
 * return max - 1 to tell its dynamically resizing callers to retry.
 * Whole UTF-8 scalars and ASCII output are never modified. */
static inline void angdroid_utf8_repair_truncated_tail(char *text, size_t used)
{
    size_t position, start, expected = 0, i;
    unsigned char lead;
    if (!text || !used) return;
    position = used;
    while (position > 0 &&
           ((unsigned char)text[position - 1] & 0xC0u) == 0x80u)
        --position;
    if (position == 0) {
        for (i = 0; i < used; ++i) text[i] = '?';
        return;
    }
    start = position - 1;
    lead = (unsigned char)text[start];
    if (lead >= 0xC2u && lead <= 0xDFu) expected = 2;
    else if (lead >= 0xE0u && lead <= 0xEFu) expected = 3;
    else if (lead >= 0xF0u && lead <= 0xF4u) expected = 4;
    if (expected && used - start < expected) {
        for (i = start; i < used; ++i) text[i] = '?';
    } else if (!expected && position < used) {
        for (i = position; i < used; ++i) text[i] = '?';
    }
}

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

/* Identify malformed UTF-8 bytes within a previously measured prefix. A
 * valid U+FFFD is three encoded bytes; only one-byte U+FFFD is malformed. */
static inline int angdroid_utf8_prefix_contains_invalid(
    const char *text, size_t prefix_bytes)
{
    size_t offset = 0;
    while (text && offset < prefix_bytes) {
        uint32_t scalar;
        size_t step = angdroid_utf8_next_scalar(text + offset, &scalar);
        if (!step || step > prefix_bytes - offset) return 1;
        if (step == 1 && scalar == 0xFFFDu &&
            (unsigned char)text[offset] >= 0x80u) return 1;
        offset += step;
    }
    return 0;
}

/* Copy exactly a complete-scalar prefix and replace each malformed input
 * byte with ASCII '?'. This cannot increase the byte length; capacity must
 * be at least prefix_bytes + 1. Return the number of bytes written. */
static inline size_t angdroid_utf8_copy_sanitized_prefix(
    char *destination, const char *text, size_t prefix_bytes)
{
    size_t offset = 0;
    if (!destination) return 0;
    if (!text) {
        destination[0] = '\0';
        return 0;
    }
    while (offset < prefix_bytes) {
        uint32_t scalar;
        size_t j;
        size_t step = angdroid_utf8_next_scalar(text + offset, &scalar);
        if (!step || step > prefix_bytes - offset) break;
        if (step == 1 && scalar == 0xFFFDu &&
            (unsigned char)text[offset] >= 0x80u) {
            destination[offset] = '?';
        } else {
            for (j = 0; j < step; ++j)
                destination[offset + j] = text[offset + j];
        }
        offset += step;
    }
    destination[offset] = '\0';
    return offset;
}
#endif
