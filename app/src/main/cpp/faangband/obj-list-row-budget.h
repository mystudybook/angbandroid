/* FAangband object list: allocate visible rows without writing beyond a
 * short textblock. The UI may omit distant items before hiding a near item.
 * This is display-only; object counts, grouping and game state are unchanged.
 */
#ifndef ANGDROID_FAANGBAND_OBJ_LIST_ROW_BUDGET_H
#define ANGDROID_FAANGBAND_OBJ_LIST_ROW_BUDGET_H

#include <stdbool.h>

struct angdroid_fa_object_rows {
    int near_rows;
    int distant_rows;
    bool show_distant;
    bool near_overflow;
    bool distant_overflow;
};

/* When a group is truncated, reserve an extra line for "... others" only
 * when at least one actual object row can also be shown. */
static inline int angdroid_fa_list_take_rows(
    int count, int capacity, bool *overflow)
{
    int rows;
    if (overflow) *overflow = false;
    if (count <= 0 || capacity <= 0) return 0;
    rows = count < capacity ? count : capacity;
    if (count > rows && rows >= 2) {
        --rows;
        if (overflow) *overflow = true;
    }
    return rows;
}

static inline struct angdroid_fa_object_rows
angdroid_fa_list_row_budget(int max_lines, int near_count, int distant_count)
{
    struct angdroid_fa_object_rows out = {0, 0, false, false, false};
    int available, near_capacity, near_used;

    if (max_lines <= 0) return out;
    if (near_count < 0) near_count = 0;
    if (distant_count < 0) distant_count = 0;

    /* One near-section heading always exists. A distant section adds a
     * separating blank line, a heading, and at least one object row.
     * Do not print the distant heading without room for an actual item. */
    out.show_distant = distant_count > 0 &&
        max_lines >= 1 + (near_count > 0 ? 1 : 0) + 3;
    available = max_lines - 1 - (out.show_distant ? 2 : 0);
    if (available <= 0) return out;

    near_capacity = available - (out.show_distant ? 1 : 0);
    out.near_rows = angdroid_fa_list_take_rows(
        near_count, near_capacity, &out.near_overflow);
    near_used = out.near_rows + (out.near_overflow ? 1 : 0);

    if (out.show_distant)
        out.distant_rows = angdroid_fa_list_take_rows(
            distant_count, available - near_used,
            &out.distant_overflow);
    return out;
}

/* Lists can contain the same distinct entry in both LOS and non-LOS.
 * Never count the other section's rows in its "... others" summary. */
static inline int angdroid_fa_list_undisplayed(int section_count, int displayed)
{
    if (section_count <= 0 || displayed >= section_count) return 0;
    return displayed <= 0 ? section_count : section_count - displayed;
}

#endif /* ANGDROID_FAANGBAND_OBJ_LIST_ROW_BUDGET_H */
