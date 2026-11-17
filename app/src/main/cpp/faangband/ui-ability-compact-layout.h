/* FAangband ability and specialty browser: terminal-cell layout only.
 * No player or game-state mutation. All coordinates fit on the terminal.
 */
#ifndef ANGDROID_FA_ABILITY_COMPACT_LAYOUT_H
#define ANGDROID_FA_ABILITY_COMPACT_LAYOUT_H

#include <stddef.h>
#include <stdint.h>

struct angdroid_fa_ability_layout {
    int width;
    int menu_rows;
    int detail_row;
    int detail_rows; /* Includes one footer/status line. */
};

/* Reserve at least three text lines plus one paging line in portrait.
 * A titled specialty menu consumes two menu rows; a header consumes one.
 * On wide terminals the panel stays at a readable 80 columns. */
static inline int angdroid_fa_ability_plan(int width, int height,
    int titled, struct angdroid_fa_ability_layout *out)
{
    int detail_rows, menu_rows;
    if (!out) return 0;
    out->width = 0;
    out->menu_rows = 0;
    out->detail_row = 0;
    out->detail_rows = 0;
    if (width < 16 || height < 12) return 0;

    detail_rows = height / 3;
    if (detail_rows < 4) detail_rows = 4;
    if (detail_rows > 10) detail_rows = 10;
    menu_rows = height - detail_rows - 1;
    if (menu_rows < (titled ? 5 : 4)) return 0;

    out->width = width > 80 ? 80 : width;
    out->menu_rows = menu_rows;
    out->detail_row = menu_rows + 1;
    out->detail_rows = height - out->detail_row;
    return out->detail_rows >= 4;
}

/* A UTF-8 string of N bytes can contain at most N+1 wrapped lines:
 * one line per byte in the worst case (embedded newlines). Reserve all
 * indices dynamically, never silently clip descriptions at 256 rows.
 * Return 0 instead of overflowing the allocation multiplication. */
static inline size_t angdroid_fa_ability_line_capacity(size_t bytes)
{
    if (bytes >= SIZE_MAX / sizeof(size_t)) return 0;
    return bytes + 1;
}

/* A page number is never negative or past the final description page.
 * Use (line_count - 1) / rows to avoid an overflowing count+rows-1. */
static inline int angdroid_fa_ability_page_clamp(
    int requested, size_t line_count, int content_rows)
{
    size_t last;
    if (requested <= 0 || content_rows <= 0 || line_count == 0) return 0;
    last = (line_count - 1) / (size_t)content_rows;
    if ((size_t)requested > last) return last > 2147483647u ?
        requested : (int)last;
    return requested;
}

#endif /* ANGDROID_FA_ABILITY_COMPACT_LAYOUT_H */
