#ifndef ANGDROID_UNICODE_LAYOUT_H
#define ANGDROID_UNICODE_LAYOUT_H

#include <stddef.h>
#include <stdint.h>
#include <wchar.h>
#include "unicode_width.h"

/*
 * Private marker in the native Angband text grid for the second column of
 * a two-column glyph. Never expose it as a real glyph to Java. The value
 * U+FDD0 is a Unicode noncharacter, not a game tile or keyboard command.
 */
#define ANGDROID_TEXT_CONTINUATION ((wchar_t)0xFDD0u)

typedef struct {
    size_t codepoints;
    size_t columns;
    int clipped;
} angdroid_text_fit;

static int angdroid_is_wide_text(wchar_t ch, int attr)
{
    if ((attr & 0x80) || ch == ANGDROID_TEXT_CONTINUATION) return 0;
    return angdroid_terminal_columns((uint32_t)ch) == 2;
}

/*
 * Find a prefix of a NUL-terminated wide string that fits wholly inside
 * a native row. The length argument is in wchar_t units, not UTF-8 bytes.
 * Does not wrap or split an East Asian full-width character at the edge.
 */
static angdroid_text_fit angdroid_fit_terminal_text(
    const wchar_t *text, size_t limit, size_t available_columns)
{
    angdroid_text_fit fit = {0u, 0u, 0};
    size_t i;

    if (!text) {
        fit.clipped = limit != 0;
        return fit;
    }
    for (i = 0; i < limit && text[i]; ++i) {
        size_t span = (size_t)(angdroid_terminal_columns((uint32_t)text[i]));
        if (span > available_columns - fit.columns) {
            fit.clipped = 1;
            return fit;
        }
        ++fit.codepoints;
        fit.columns += span;
    }
    return fit;
}


/*
 * Enumerate one visible text run in a native row, omitting the internal
 * continuation column while retaining physical x offsets. A zero return
 * means no more visible code points remain in this source range.
 */
static size_t angdroid_next_visible_text_run(
    const wchar_t *cells, size_t length, size_t *cursor, size_t *start)
{
    size_t i;
    if (!cells || !cursor || !start || *cursor >= length) return 0;
    i = *cursor;
    while (i < length && cells[i] == ANGDROID_TEXT_CONTINUATION) ++i;
    *start = i;
    while (i < length && cells[i] != ANGDROID_TEXT_CONTINUATION) ++i;
    *cursor = i;
    return i - *start;
}

#endif
