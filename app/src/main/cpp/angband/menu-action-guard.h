#ifndef ANGDROID_MENU_ACTION_GUARD_H
#define ANGDROID_MENU_ACTION_GUARD_H

/* Menu cursor positions refer to visible rows; filters refer to original
 * action indices. Verify both before calling a row handler. */
static inline int angdroid_menu_action_item_index(
    int cursor, int visible_count, int original_count, const int *filter)
{
    int oid;

    if (cursor < 0 || visible_count <= 0 ||
        cursor >= visible_count || original_count <= 0)
        return -1;
    oid = filter ? filter[cursor] : cursor;
    return oid >= 0 && oid < original_count ? oid : -1;
}

/* The gray flag belongs to the SELECTED item, not acts[0]. */
static inline int angdroid_menu_action_can_run(
    int selected_flags, int disabled_flag, int has_callback)
{
    return has_callback && !(selected_flags & disabled_flag);
}

#endif
