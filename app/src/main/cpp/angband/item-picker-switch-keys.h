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

#endif /* ANGDROID_ITEM_PICKER_SWITCH_KEYS_H */
