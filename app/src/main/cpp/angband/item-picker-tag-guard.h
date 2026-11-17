/* Item picker safety for @0 .. @9 quiver shortcuts.
 * Quiver tags index a runtime-sized array; keep the original inscription
 * search as the fallback for non-digit or out-of-range tags.
 */
#ifndef ANGDROID_ITEM_PICKER_TAG_GUARD_H
#define ANGDROID_ITEM_PICKER_TAG_GUARD_H

static inline int angdroid_item_picker_quiver_index(int tag, int slot_count)
{
    int index;
    if (slot_count <= 0 || tag < '0' || tag > '9') return -1;
    index = tag - '0';
    return index < slot_count ? index : -1;
}

/* A SHOW_EMPTY row is drawn but has no selectable object. Index
 * validation must happen before reading the row's object or key; the
 * callback can also receive oid=-1 for a blank picker shortcut. */
static inline int angdroid_item_picker_row_in_range(int row, int count)
{
    return row >= 0 && row < count;
}

#endif
