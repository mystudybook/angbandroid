/* Offline C99 regression for active item-picker tab switch hints. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/item-picker-switch-keys.h"

static void available_tab_combinations(void)
{
    int mask;
    for (mask = 0; mask < 8; ++mask) {
        char storage[6] = {'#', 0, 0, 0, 0, '#'};
        char expected[4] = "";
        char *keys = storage + 1;
        int n = 0;

        if (mask & 1) expected[n++] = '/';
        if (mask & 2) expected[n++] = '|';
        if (mask & 4) expected[n++] = '-';

        angdroid_item_picker_switch_keys(
            keys, mask & 1, mask & 2, mask & 4);
        assert(strcmp(keys, expected) == 0);
        assert(strlen(keys) == (size_t)n);
        assert(storage[0] == '#');
        assert(storage[5] == '#');
    }
}

static void no_unusable_tab_is_advertised(void)
{
    char keys[4] = "xyz";
    angdroid_item_picker_switch_keys(keys, 0, 0, 0);
    assert(strcmp(keys, "") == 0);
    angdroid_item_picker_switch_keys(keys, 1, 0, 0);
    assert(strcmp(keys, "/") == 0);
    angdroid_item_picker_switch_keys(keys, 0, 1, 0);
    assert(strcmp(keys, "|") == 0);
    angdroid_item_picker_switch_keys(keys, 0, 0, 1);
    assert(strcmp(keys, "-") == 0);
    angdroid_item_picker_switch_keys(keys, 1, 1, 1);
    assert(strcmp(keys, "/|-") == 0);
    angdroid_item_picker_switch_keys(NULL, 1, 1, 1);
}

/* Visible SHOW_EMPTY equipment slots are not selectable objects.
 * The switch strip must stay usable even if the list has 12 blank slots. */
static void placeholder_only_equipment_switches(void)
{
    const char *alternates[] = { NULL, "", "/", "|", "-", "/|-" };
    int count, k;

    for (count = -2; count <= 12; ++count) {
        for (k = 0; k < 6; ++k) {
            const int expected = count <= 0 && k >= 2;
            assert(!!angdroid_item_picker_allow_blank_switch(
                count, alternates[k]) == expected);
        }
    }
    assert(angdroid_item_picker_allow_blank_switch(0, "/"));
    assert(angdroid_item_picker_allow_blank_switch(0, "/|-"));
    assert(!angdroid_item_picker_allow_blank_switch(1, "/|-"));
    assert(!angdroid_item_picker_allow_blank_switch(12, "/|-"));
}

int main(void)
{
    available_tab_combinations();
    no_unusable_tab_is_advertised();
    placeholder_only_equipment_switches();
    puts("PASS: only actionable inventory/equipment/quiver/floor tab shortcuts");
    return 0;
}
