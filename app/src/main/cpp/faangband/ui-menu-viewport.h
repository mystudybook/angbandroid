/* FAangband terminal menu positions after selection/filter/rotation.
 * No game state is changed: only visible row and cursor indices are clamped.
 */
#ifndef ANGDROID_FA_MENU_VIEWPORT_H
#define ANGDROID_FA_MENU_VIEWPORT_H

static inline int angdroid_fa_menu_clamp_cursor(int cursor, int count)
{
    if (count <= 0) return -1;
    if (cursor < 0) return 0;
    if (cursor >= count) return count - 1;
    return cursor;
}

static inline int angdroid_fa_menu_clamp_top(
    int top, int cursor, int count, int page_rows)
{
    int max_top;
    if (count <= 0 || page_rows <= 0) return 0;
    max_top = count > page_rows ? count - page_rows : 0;
    if (top < 0) top = 0;
    if (top > max_top) top = max_top;
    if (cursor >= 0 && cursor < count) {
        if (cursor < top) top = cursor;
        else if (cursor - top >= page_rows)
            top = cursor - page_rows + 1;
    }
    return top < max_top ? top : max_top;
}

#endif /* ANGDROID_FA_MENU_VIEWPORT_H */
