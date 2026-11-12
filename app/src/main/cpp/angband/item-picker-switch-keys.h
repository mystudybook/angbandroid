/* Only offer Android inventory/equipment/quiver/floor switch keys that
 * can actually change the current item picker tab. The caller has already
 * checked the command's allowed sources and the current tab.
 *
 * This is strictly a UI shortcut list: it does not change command rules.
 */
#ifndef ANGDROID_ITEM_PICKER_SWITCH_KEYS_H
#define ANGDROID_ITEM_PICKER_SWITCH_KEYS_H

static inline void angdroid_item_picker_switch_keys(
    char keys[4], int can_toggle_inventory_equipment,
    int can_open_quiver, int can_open_floor)
{
    int count = 0;

    if (!keys)
        return;
    if (can_toggle_inventory_equipment)
        keys[count++] = '/';
    if (can_open_quiver)
        keys[count++] = '|';
    if (can_open_floor)
        keys[count++] = '-';
    keys[count] = '\0';
}

/* SHOW_EMPTY equipment may render multiple placeholder rows while none
 * contains an object. Those rows are not selectable, but /, | and - must
 * still be passed to the picker so it can open another allowed source. */
static inline int angdroid_item_picker_allow_blank_switch(
    int selectable_objects, const char *switch_keys)
{
    return selectable_objects <= 0 && switch_keys && switch_keys[0] != '\0';
}

/* A SHOW_EMPTY list may have several painted placeholder rows and
 * zero selectable objects. Let left/right arrows switch item sources in
 * that case, but never treat up/down as an item selection. Do not enable
 * this fallback for shops or other non-item menus. */
static inline int angdroid_item_picker_blank_arrow_switch(
    int flags, int empty_input_flag, int is_item_skin,
    int has_switch_keys, int dx, int dy)
{
    return (flags & empty_input_flag) != 0 &&
        is_item_skin && has_switch_keys && dx != 0 && dy == 0;
}

#endif /* ANGDROID_ITEM_PICKER_SWITCH_KEYS_H */
