/* Android/desktop menu scroll bounds after filtering and viewport resize.
 * Pure geometry: do not change the item identity, only the visible origin.
 */
#ifndef ANGDROID_MENU_SCROLL_VIEWPORT_H
#define ANGDROID_MENU_SCROLL_VIEWPORT_H

static inline int angdroid_menu_scroll_clamp_top(
    int top, int cursor, int count, int page_rows)
{
    int max_top;

    if (count <= 0 || page_rows <= 0)
        return 0;

    max_top = count > page_rows ? count - page_rows : 0;
    if (top < 0) top = 0;
    if (top > max_top) top = max_top;

    /* When no row is selectable (cursor == -1), keep the legal page
     * origin but do not move the cursor or select an arbitrary row. */
    if (cursor >= 0 && cursor < count) {
        if (cursor < top)
            top = cursor;
        else if (cursor - top >= page_rows)
            top = cursor - page_rows + 1;
    }
    return top;
}

#endif /* ANGDROID_MENU_SCROLL_VIEWPORT_H */
