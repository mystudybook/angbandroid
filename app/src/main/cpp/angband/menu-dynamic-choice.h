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

#endif
