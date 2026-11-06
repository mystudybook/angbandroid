/* Equipment-compare touch selection and resize safety.
 * Rows refer to filtered/sorted view indices, never object array IDs.
 * Keep this display-only: callers still use sorted_indices[] to accept.
 */
#ifndef ANGDROID_EQUIP_COMPARE_TOUCH_GUARD_H
#define ANGDROID_EQUIP_COMPARE_TOUCH_GUARD_H
#include <stdbool.h>
#include <stdint.h>

static inline bool angdroid_equip_cmp_touch_index(
    int x, int y, int width, int height, int first_row,
    int visible_count, int first_index, int direction,
    int filtered_count, int *selected)
{
    int offset;
    int64_t index;

    if (!selected) return false;
    *selected = -1;
    if (width <= 0 || height <= 0 || x < 0 || x >= width ||
            y < 0 || y >= height || first_row < 0 ||
            visible_count <= 0 || filtered_count <= 0 ||
            visible_count > filtered_count ||
            first_index < 0 || first_index >= filtered_count ||
            (direction != 1 && direction != -1)) return false;
    if (y < first_row) return false;
    offset = y - first_row;
    if (offset >= visible_count) return false;
    index = (int64_t)first_index + (int64_t)offset * direction;
    if (index < 0 || index >= filtered_count) return false;
    *selected = (int)index;
    return true;
}

static inline bool angdroid_equip_cmp_selection_valid(
    int selected, int filtered_count)
{
    return filtered_count > 0 && selected >= 0 &&
        selected < filtered_count;
}

/* Retain the same filtered item when still visible; otherwise use the
 * nearest visible row. Invalid/empty pages cancel selection safely. */
static inline int angdroid_equip_cmp_clamp_visible_selection(
    int selected, int first_index, int visible_count,
    int direction, int filtered_count)
{
    int64_t last, offset;

    if (selected == -1) return -1;
    if (filtered_count <= 0 || visible_count <= 0 ||
            visible_count > filtered_count ||
            first_index < 0 || first_index >= filtered_count ||
            (direction != 1 && direction != -1)) return -1;

    last = (int64_t)first_index +
        (int64_t)(visible_count - 1) * direction;
    if (last < 0 || last >= filtered_count) return -1;
    offset = ((int64_t)selected - first_index) * direction;
    if (offset < 0) return first_index;
    if (offset >= visible_count) return (int)last;
    return selected;
}
#endif
