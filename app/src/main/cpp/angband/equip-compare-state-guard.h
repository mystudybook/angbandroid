/* Tiny-terminal safety for the original Angband equipment comparison.
 * The UI may enter its BAIL state before summary counting/filtering has run.
 * Keep failed-layout messages separate from uninitialized item state.
 */
#ifndef ANGDROID_EQUIP_COMPARE_STATE_GUARD_H
#define ANGDROID_EQUIP_COMPARE_STATE_GUARD_H
#include <stdbool.h>

static inline bool angdroid_equip_cmp_summary_readable(
    bool layout_failed, bool summary_exists)
{
    return !layout_failed && summary_exists;
}

static inline bool angdroid_equip_cmp_label_region_valid(
    int width, int height, int row, int col)
{
    return width > 0 && height > 0 &&
        row >= 0 && row < height && col >= 0 && col < width;
}
#endif
