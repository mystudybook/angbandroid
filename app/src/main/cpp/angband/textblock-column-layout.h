/* Terminal-cell-aware line boundaries for textblock-based prose.
 * Inputs and returned lengths are wchar_t indexes; width is in cells.
 * Keep this independent of Angband memory allocators for host C99 tests.
 */
#ifndef ANGDROID_TEXTBLOCK_COLUMN_LAYOUT_H
#define ANGDROID_TEXTBLOCK_COLUMN_LAYOUT_H
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>
#include "../curses/unicode_layout.h"

typedef struct {
    size_t length;
    size_t next;
} angdroid_textblock_line;

static inline angdroid_textblock_line angdroid_textblock_next_line(
    const wchar_t *text, const uint8_t *attrs,
    size_t total, size_t start, size_t width)
{
    angdroid_textblock_line result = {0, total};
    size_t i, columns = 0, last_space = start;
    if (!text || !width || start >= total) return result;
    for (i = start; i < total; ++i) {
        size_t span;
        if (text[i] == L'\n') {
            result.length = i - start;
            result.next = i + 1;
            return result;
        }
        span = (size_t)(angdroid_is_wide_text(text[i], attrs ? attrs[i] : 0) ? 2 : 1);
        if (span > width - columns) {
            /* Prefer breaking at a previous space, preserving English
             * words while allowing Chinese text to wrap without spaces. */
            if (last_space > start) {
                result.length = last_space - start;
                result.next = last_space + 1;
            } else if (i > start) {
                result.length = i - start;
                result.next = i;
            } else {
                /* One wide glyph on a one-cell row: consume it. The
                 * renderer substitutes '?' because it cannot fit there. */
                result.length = 1;
                result.next = i + 1;
            }
            return result;
        }
        columns += span;
        if (text[i] == L' ') last_space = i;
        if (columns == width) {
            if (last_space > start) {
                result.length = last_space - start;
                result.next = last_space + 1;
            } else {
                result.length = i - start + 1;
                result.next = i + 1;
            }
            return result;
        }
    }
    result.length = total - start;
    result.next = total;
    return result;
}
#endif
