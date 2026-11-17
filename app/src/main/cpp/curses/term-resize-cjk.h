/* Keep two-column CJK text intact when resizing the native terminal.
 * Only replace a wide lead when the old source contains its continuation
 * just beyond the copied right edge.  This avoids modifying FAangband
 * single-cell map glyphs, which are not stream-text pairs.
 */
#ifndef ANGDROID_TERM_RESIZE_CJK_H
#define ANGDROID_TERM_RESIZE_CJK_H

#include "unicode_layout.h"

static inline void angdroid_term_repair_resized_cjk_row(
    wchar_t *dest, const int *dest_attrs,
    const wchar_t *src, const int *src_attrs,
    int copied_width, int source_width)
{
    int x;

    if (!dest || !dest_attrs || !src || !src_attrs ||
            copied_width < 1 || source_width < copied_width)
        return;

    /* A private continuation without a visible lead is not a character. */
    for (x = 0; x < copied_width; ++x) {
        if (dest[x] == ANGDROID_TEXT_CONTINUATION &&
                (x == 0 ||
                 !angdroid_is_wide_text(dest[x - 1], dest_attrs[x - 1])))
            dest[x] = L' ';
    }

    /* Copying only the left part of a wider screen may cut a text pair.
     * Verify the source marker before replacing the last visible lead:
     * a solitary wide map glyph must be left unchanged. */
    if (copied_width < source_width &&
            src[copied_width] == ANGDROID_TEXT_CONTINUATION &&
            angdroid_is_wide_text(src[copied_width - 1],
                                  src_attrs[copied_width - 1])) {
        dest[copied_width - 1] = L'?';
    }
}

#endif
