/* Safe textblock pager dimensions for very small mobile terminals.
 * With fewer than three rows after the optional header, keep all rows for
 * text and omit the two-row prompt/footer. No signed-to-size_t underflow.
 */
#ifndef ANGDROID_TEXTBLOCK_PAGER_LAYOUT_H
#define ANGDROID_TEXTBLOCK_PAGER_LAYOUT_H
#include <stddef.h>

static inline int angdroid_textblock_pager_content_rows(
    int region_rows, int has_header, int *has_footer)
{
    int available;
    if (has_footer) *has_footer = 0;
    if (region_rows <= 0) return 0;
    available = region_rows - (has_header ? 1 : 0);
    if (available <= 0) return 0;
    if (available >= 3) {
        if (has_footer) *has_footer = 1;
        available -= 2;
    }
    return available;
}

/* Keep all scroll commands inside the actual line range. This accepts
 * size_t values so huge textblocks cannot overflow a signed line index. */
static inline size_t angdroid_textblock_pager_clamp_start(
    size_t requested, size_t total_lines, int content_rows)
{
    size_t limit;
    if (content_rows <= 0 || total_lines <= (size_t)content_rows) return 0;
    limit = total_lines - (size_t)content_rows;
    return requested < limit ? requested : limit;
}
#endif
