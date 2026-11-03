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

static inline int angdroid_is_wide_text(wchar_t ch, int attr)
{
    if ((attr & 0x80) || ch == ANGDROID_TEXT_CONTINUATION) return 0;
    return angdroid_terminal_columns((uint32_t)ch) == 2;
}

/*
 * Find a prefix of a NUL-terminated wide string that fits wholly inside
 * a native row. The length argument is in wchar_t units, not UTF-8 bytes.
 * Does not wrap or split an East Asian full-width character at the edge.
 */
static inline angdroid_text_fit angdroid_fit_terminal_text(
    const wchar_t *text, size_t limit, size_t available_columns, int attr)
{
    angdroid_text_fit fit = {0u, 0u, 0};
    size_t i;

    if (!text) {
        fit.clipped = limit != 0;
        return fit;
    }
    for (i = 0; i < limit && text[i]; ++i) {
        size_t span = (size_t)(angdroid_is_wide_text(text[i], attr) ? 2 : 1);
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
static inline size_t angdroid_next_visible_text_run(
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


/*
 * Expand a requested redraw rectangle to full native display-cell spans.
 * This is row-local: the caller must not carry adjusted boundaries from one
 * row to another. Read both requested and previously drawn cells because
 * either version might contain a wide glyph or an old big-tile pad.
 */
static inline void angdroid_expand_redraw_columns(
    int width, int *first, int *last,
    const wchar_t *requested_chars, const int *requested_attrs,
    const wchar_t *previous_chars, const int *previous_attrs)
{
    if (width <= 0 || !first || !last ||
        !requested_chars || !requested_attrs ||
        !previous_chars || !previous_attrs) return;
    if (*first < 0 || *last >= width || *first > *last) return;

    if (*first > 0 &&
        (requested_chars[*first] == ANGDROID_TEXT_CONTINUATION ||
         previous_chars[*first] == ANGDROID_TEXT_CONTINUATION ||
         requested_attrs[*first] == 255 ||
         previous_attrs[*first] == 255)) {
        --*first;
    }
    if (*last + 1 < width &&
        (angdroid_is_wide_text(requested_chars[*last],
                               requested_attrs[*last]) ||
         angdroid_is_wide_text(previous_chars[*last],
                               previous_attrs[*last]))) {
        ++*last;
    }
}


/*
 * If cell x is one half of a wide glyph, identify the other half.
 * Return -1 for ordinary cells, tile bytes, invalid positions, or an
 * orphaned continuation marker. This is independent of curses/term and
 * can be checked with the host C compiler.
 */
static inline int angdroid_wide_partner_column(
    int width, int x, const wchar_t *cells, const int *attrs)
{
    if (!cells || !attrs || width <= 0 || x < 0 || x >= width)
        return -1;

    if (cells[x] == ANGDROID_TEXT_CONTINUATION &&
        x > 0 && angdroid_is_wide_text(cells[x - 1], attrs[x - 1]))
        return x - 1;

    if (x + 1 < width && angdroid_is_wide_text(cells[x], attrs[x]) &&
        cells[x + 1] == ANGDROID_TEXT_CONTINUATION)
        return x + 1;

    return -1;
}


/*
 * Merge a requested native redraw range with changes already pending in
 * the same row or global row bounds. Existing markers may use first > last
 * to indicate that no cells were dirty before the new request.
 */
static inline void angdroid_union_dirty_range(
    int first, int last, int *dirty_first, int *dirty_last)
{
    if (!dirty_first || !dirty_last || first > last) return;
    if (first < *dirty_first) *dirty_first = first;
    if (last > *dirty_last) *dirty_last = last;
}


/*
 * Direct cell writes (Term_draw and Term_queue_chars) address fixed map
 * columns. A wide glyph must not consume the adjacent grid cell: that cell
 * may contain an unrelated map feature, a tile or another UI control.
 * Keep text flowing through Term_addstr/Term_addch for two-column CJK.
 */
static inline wchar_t angdroid_fixed_cell_glyph(wchar_t ch, int attr)
{
    if (ch == ANGDROID_TEXT_CONTINUATION ||
        angdroid_is_wide_text(ch, attr))
        return L'?';
    return ch;
}

#endif
