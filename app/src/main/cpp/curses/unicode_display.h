#ifndef ANGDROID_UNICODE_DISPLAY_H
#define ANGDROID_UNICODE_DISPLAY_H

#include <stddef.h>
#include <stdint.h>
#include "unicode_width.h"

/*
 * Measure UI text in terminal columns, NOT Unicode scalar count or UTF-8
 * bytes. The terminal's own Unicode width classifier is the source of truth.
 * These routines are display helpers only; game records stay untouched.
 *
 * Incomplete/invalid UTF-8 consumes one byte as U+FFFD (one column).
 * Like unicode_width.h, this is not full grapheme-cluster handling.
 */
static inline size_t angdroid_utf8_next_scalar(
    const char *text, uint32_t *scalar)
{
    const unsigned char *p = (const unsigned char *)text;
    uint32_t cp = 0xFFFDu;
    size_t count = 1;
    size_t i;

    if (!text || !p[0] || !scalar) return 0;

    if (p[0] < 0x80u) {
        *scalar = p[0];
        return 1;
    }
    if (p[0] >= 0xC2u && p[0] <= 0xDFu) {
        cp = p[0] & 0x1Fu;
        count = 2;
    } else if (p[0] >= 0xE0u && p[0] <= 0xEFu) {
        cp = p[0] & 0x0Fu;
        count = 3;
    } else if (p[0] >= 0xF0u && p[0] <= 0xF4u) {
        cp = p[0] & 0x07u;
        count = 4;
    } else {
        *scalar = 0xFFFDu;
        return 1;
    }

    for (i = 1; i < count; ++i) {
        /* Stop at the NUL before inspecting any following bytes. */
        if (!p[i] || (p[i] & 0xC0u) != 0x80u) {
            *scalar = 0xFFFDu;
            return 1;
        }
        cp = (cp << 6) | (p[i] & 0x3Fu);
    }
    if ((count == 2 && cp < 0x80u) ||
        (count == 3 && cp < 0x800u) ||
        (count == 4 && cp < 0x10000u) ||
        cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu)) {
        *scalar = 0xFFFDu;
        return 1;
    }
    *scalar = cp;
    return count;
}

static inline size_t angdroid_utf8_display_columns(const char *text)
{
    size_t columns = 0;
    size_t offset = 0;
    uint32_t scalar;
    size_t count;

    if (!text) return 0;
    while ((count = angdroid_utf8_next_scalar(text + offset, &scalar)) != 0) {
        columns += (size_t)angdroid_terminal_columns(scalar);
        offset += count;
    }
    return columns;
}

/* Copy complete UTF-8 scalars into a byte-limited display buffer.
 * Caller may subsequently clip to terminal columns.  Unlike my_strcpy(),
 * this never writes a partial Han glyph at the byte boundary.
 * A malformed input byte is replaced with ASCII '?' (one byte) so even an
 * invalid translated name cannot inject broken UTF-8 into the terminal.
 */
static inline size_t angdroid_utf8_copy_complete(char *dst, size_t capacity,
    const char *text)
{
    size_t used = 0, offset = 0;
    uint32_t scalar;
    size_t count, j;
    if (!dst || capacity == 0) return 0;
    dst[0] = '\0';
    if (!text) return 0;
    while (text[offset]) {
        count = angdroid_utf8_next_scalar(text + offset, &scalar);
        if (!count || count > capacity - used - 1) break;
        if (count == 1 && scalar == 0xFFFDu &&
            (unsigned char)text[offset] >= 0x80u) {
            dst[used++] = '?';
            ++offset;
            continue;
        }
        for (j = 0; j < count; ++j) dst[used++] = text[offset + j];
        offset += count;
    }
    dst[used] = '\0';
    return used;
}

/* Append only whole UTF-8 scalars to an already NUL-terminated display
 * buffer.  The caller owns the byte offset, so no repeated strlen() scan is
 * needed for spell-effect summaries or other incremental UI renderers.
 * A full buffer is left unchanged.  Never modifies game-state strings. */
static inline size_t angdroid_utf8_append_complete(char *dst, size_t capacity,
    size_t offset, const char *text)
{
    if (!dst || capacity == 0 || offset >= capacity) return offset;
    return offset + angdroid_utf8_copy_complete(
        dst + offset, capacity - offset, text);
}

/* In-place clip to whole UTF-8 scalars that fit inside max_columns.
 * This never slices the bytes belonging to a Han/Hangul/wide character. */
static inline void angdroid_utf8_clip_columns(char *text, size_t max_columns)
{
    size_t offset = 0;
    size_t columns = 0;
    size_t count;
    uint32_t scalar;

    if (!text) return;
    while ((count = angdroid_utf8_next_scalar(text + offset, &scalar)) != 0) {
        size_t width = (size_t)angdroid_terminal_columns(scalar);
        if (width > max_columns - columns) break;
        columns += width;
        offset += count;
    }
    text[offset] = '\0';
}

/* Skip display columns without ever returning a pointer into the middle
 * of a UTF-8 scalar. If the offset falls inside a wide scalar, skip the
 * entire scalar so the terminal never starts on a continuation byte.
 * The caller's horizontal offset is in *columns*, not encoded bytes. */
static inline const char *angdroid_utf8_skip_columns(
    const char *text, size_t columns)
{
    const char *p = text;
    size_t used = 0;
    uint32_t cp;
    size_t count;

    if (!p) return NULL;
    while (*p && used < columns) {
        count = angdroid_utf8_next_scalar(p, &cp);
        if (!count) break;
        used += (size_t)angdroid_terminal_columns(cp);
        p += count;
    }
    return p;
}

/* Width of the prefix [text, end) where end is known to refer to the
 * same string. Intended for rendering highlighted message matches. */
static inline size_t angdroid_utf8_prefix_columns(
    const char *text, const char *end)
{
    const char *p = text;
    size_t columns = 0;
    uint32_t cp;
    size_t count;

    if (!p || !end) return 0;
    while (*p && p < end) {
        count = angdroid_utf8_next_scalar(p, &cp);
        if (!count || p + count > end) break;
        columns += (size_t)angdroid_terminal_columns(cp);
        p += count;
    }
    return columns;
}

/* printf("%-*s") pads in bytes. Add the difference between actual UTF-8
 * bytes and terminal columns to obtain a field of 'columns' display cells.
 * The caller must first clip text to fit the requested number of columns. */
static inline size_t angdroid_utf8_printf_field_bytes(
    const char *text, size_t columns)
{
    size_t bytes = 0;
    size_t display = 0;

    if (text) {
        const char *p = text;
        while (*p++) ++bytes;
        display = angdroid_utf8_display_columns(text);
    }
    /* Defensive fallback for accidentally un-clipped content. */
    if (display > columns) return bytes;
    return columns + bytes - display;
}

#endif
