#ifndef ANGDROID_MENU_DYNAMIC_CHOICE_H
#define ANGDROID_MENU_DYNAMIC_CHOICE_H

/* A menu entry may only be selected by an actual SELECT event, or by a
 * caller-declared shortcut that resolves to an existing valid menu row.
 * Unhandled key events, cancellation and empty menus must return -1. */
static inline int angdroid_menu_dynamic_choice_index(
    int is_select, int is_alias, int cursor, int alias_row, int count)
{
    int index = is_select ? cursor : (is_alias ? alias_row : -1);

    return index >= 0 && index < count ? index : -1;
}

/* A filtered popup displays visible positions, but linked-list traversal
 * needs the original entry index. Reject stale or invalid filter entries. */
static inline int angdroid_menu_dynamic_original_index(
    int visible_index, int visible_count, int original_count,
    const int *filter_list)
{
    int index;

    if (visible_index < 0 || visible_count <= 0 ||
        original_count <= 0 || visible_index >= visible_count)
        return -1;
    index = filter_list ? filter_list[visible_index] : visible_index;
    return index >= 0 && index < original_count ? index : -1;
}

#endif
