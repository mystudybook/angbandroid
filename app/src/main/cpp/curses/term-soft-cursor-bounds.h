/* Bounds used by soft-cursor redraw and by the native big-cursor hook.
 * This header depends only on C99 types so host tests exercise the same
 * arithmetic as both Angband and FAangband. */
#ifndef ANGDROID_TERM_SOFT_CURSOR_BOUNDS_H
#define ANGDROID_TERM_SOFT_CURSOR_BOUNDS_H

#include <stdbool.h>

/* A cursor span may be corrupt, very large, or extend past the terminal.
 * Return -1 when its origin is outside the screen; otherwise return the
 * inclusive final visible cell without overflowing signed int. */
static inline int angdroid_term_cursor_last_cell(int origin, int span, int limit)
{
    int remaining;
    if (limit <= 0 || origin < 0 || origin >= limit) return -1;
    remaining = limit - origin;
    if (span < 1) span = 1;
    if (span > remaining) span = remaining;
    return origin + span - 1;
}

/* A native large cursor must fit entirely inside the current terminal.
 * Compare lengths against the remaining room instead of adding coordinates:
 * the latter could overflow for a pathological tile dimension. */
static inline bool angdroid_term_big_cursor_fits(
    int x, int y, int width, int height, int tile_width, int tile_height)
{
    if (width <= 0 || height <= 0 || x < 0 || y < 0 ||
            x >= width || y >= height ||
            tile_width <= 0 || tile_height <= 0) return false;
    return tile_width <= width - x && tile_height <= height - y;
}

#endif
