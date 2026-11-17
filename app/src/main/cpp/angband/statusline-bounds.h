/* Screen-only bounds for Angband and FAangband statusline placement.
 * Avoid advancing past Term->wid when a translated label is clipped.
 * Status values, colors and the order of handlers are unchanged.
 */
#ifndef ANGDROID_STATUSLINE_BOUNDS_H
#define ANGDROID_STATUSLINE_BOUNDS_H
#include <stddef.h>

static inline int angdroid_statusline_has_room(int col, size_t used,
    int term_width)
{
    return term_width > 0 && col >= 0 && col < term_width &&
        used < (size_t)(term_width - col);
}

static inline int angdroid_statusline_next_col(int col, size_t advance,
    int term_width)
{
    if (term_width <= 0) return 0;
    if (col <= 0) col = 0;
    if (col >= term_width || advance >= (size_t)(term_width - col))
        return term_width;
    return col + (int)advance;
}
#endif
