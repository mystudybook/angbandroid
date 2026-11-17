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

/* Explicit opt-in for non-selection commands in an empty store menu.
 * Selection events must always require a valid cursor/item. */
static inline int angdroid_menu_allow_empty_input(
    int flags, int allow_flag, int keyboard_or_mouse_event)
{
    return (flags & allow_flag) != 0 && keyboard_or_mouse_event;
}

/* An empty shop can still sell from inventory, show help, inspect player
 * inventory, open settings and leave.  Stock purchase/examine shortcuts are
 * disabled until a real stock row exists. This filters only Android visual
 * hints; command handling retains its own selection checks. */
static inline int angdroid_menu_empty_shop_key_visible(
    int flags, int empty_shop_flag, int visible_count, char key)
{
    if (!(flags & empty_shop_flag) || visible_count > 0)
        return 1;

    return key != 'p' && key != 'g' && key != 'l' && key != 'x';
}

#endif
