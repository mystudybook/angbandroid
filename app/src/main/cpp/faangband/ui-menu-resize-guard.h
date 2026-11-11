/* FAangband terminal menu bounds, shared by the regular menu loop.
 * Android rotation can transiently produce a 0x0 or very small terminal:
 * do not ask a menu skin to render invalid or negative rectangles.
 * Keep this independent of Native globals so C99 can test the geometry.
 */
#ifndef ANGDROID_FA_MENU_RESIZE_GUARD_H
#define ANGDROID_FA_MENU_RESIZE_GUARD_H

#include <stdbool.h>

static inline bool angdroid_fa_menu_region_visible(
    int screen_width, int screen_height,
    int col, int row, int width, int rows)
{
    if (screen_width <= 0 || screen_height <= 0 ||
        col < 0 || row < 0 || width <= 0 || rows <= 0)
        return false;
    /* Check the origin before subtracting: INT_MAX and INT_MIN inputs
     * must not cause signed overflow or an unsigned width wraparound. */
    if (col >= screen_width || row >= screen_height)
        return false;
    return width <= screen_width - col && rows <= screen_height - row;
}

#endif /* ANGDROID_FA_MENU_RESIZE_GUARD_H */
