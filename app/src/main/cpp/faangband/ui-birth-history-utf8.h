/* FAangband character history: wrap Unicode scalars by actual terminal cells.
 * Source history is never modified and all offsets remain Unicode scalar
 * indices used by the existing native input editor (not byte indices).
 */
#ifndef ANGDROID_FA_BIRTH_HISTORY_UTF8_H
#define ANGDROID_FA_BIRTH_HISTORY_UTF8_H

#include <stddef.h>
#include <stdint.h>
#include "../curses/unicode_display.h"

/* Return at least one line for an empty history. Lines are represented as
 * [scalar_start, scalar_length], so the caller can use its original UTF-8
 * byte string without splitting a Chinese glyph.
 *
 * A new line is forced on an embedded newline or when the next glyph would
 * exceed the terminal width. The UTF-8 decoder substitutes one scalar for
 * malformed bytes, matching the terminal's fallback behavior.
 */
static inline size_t angdroid_fa_history_wrap(
    const char *text, size_t width, size_t *starts, size_t *lengths,
    size_t capacity)
{
    size_t offset = 0, scalar_pos = 0, col = 0, count = 1;
    uint32_t cp;
    size_t bytes, glyph_cols;

    if (!text || !starts || !lengths || !capacity) return 0;
    if (!width) width = 1;
    starts[0] = 0;
    lengths[0] = 0;

    while ((bytes = angdroid_utf8_next_scalar(text + offset, &cp)) != 0) {
        glyph_cols = (size_t)angdroid_terminal_columns(cp);
        if (cp == '\n') {
            lengths[count - 1] = scalar_pos - starts[count - 1];
            ++scalar_pos;
            offset += bytes;
            if (count == capacity) break;
            starts[count] = scalar_pos;
            lengths[count] = 0;
            ++count;
            col = 0;
            continue;
        }

        if (glyph_cols > width - col &&
                scalar_pos > starts[count - 1]) {
            lengths[count - 1] = scalar_pos - starts[count - 1];
            if (count == capacity) break;
            starts[count] = scalar_pos;
            lengths[count] = 0;
            ++count;
            col = 0;
            continue;
        }

        col += glyph_cols;
        offset += bytes;
        ++scalar_pos;
        lengths[count - 1] = scalar_pos - starts[count - 1];
    }
    return count;
}

/* Draw a wrapped line from the original UTF-8 input without ever slicing
 * in the middle of a multibyte scalar or touching the user-owned history. */
static inline size_t angdroid_fa_history_copy_line(
    char *out, size_t capacity, const char *text,
    size_t start, size_t count)
{
    size_t offset = 0, index = 0, used = 0;
    uint32_t cp;
    size_t bytes, j;
    if (!out || !capacity) return 0;
    out[0] = '\0';
    if (!text) return 0;
    while (index < start && (bytes =
            angdroid_utf8_next_scalar(text + offset, &cp)) != 0) {
        offset += bytes;
        ++index;
    }
    while (count && (bytes =
            angdroid_utf8_next_scalar(text + offset, &cp)) != 0) {
        if (used + bytes >= capacity) break;
        if (bytes == 1 && cp == 0xFFFDu &&
                (unsigned char)text[offset] >= 0x80u) {
            out[used++] = '?';
        } else {
            for (j = 0; j < bytes; ++j)
                out[used++] = text[offset + j];
        }
        offset += bytes;
        --count;
    }
    out[used] = '\0';
    return used;
}
#endif
