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

/* Native menu-key routing: a SHOW_EMPTY picker may have rows but
 * zero *selectable* objects. Horizontal arrows are source changes only
 * when the item skin explicitly opted in to blank keyboard input. */
static void blank_picker_direction_keys(void)
{
    const int empty_flag = 0x80;
    const int directions[] = {-1, 0, 1};
    int flags, is_item_skin, has_switch_keys, x, y;

    for (flags = 0; flags < 256; ++flags)
        for (is_item_skin = 0; is_item_skin <= 1; ++is_item_skin)
            for (has_switch_keys = 0; has_switch_keys <= 1; ++has_switch_keys)
                for (x = 0; x < 3; ++x)
                    for (y = 0; y < 3; ++y) {
                        int dx = directions[x];
                        int dy = directions[y];
                        int expected = (flags & empty_flag) &&
                            is_item_skin && has_switch_keys &&
                            dx != 0 && dy == 0;
                        int actual = angdroid_item_picker_blank_arrow_switch(
                            flags, empty_flag, is_item_skin,
                            has_switch_keys, dx, dy);
                        assert(!!actual == !!expected);
                    }
    /* Never let vertical/diagonal arrows select a placeholder slot. */
    assert(!angdroid_item_picker_blank_arrow_switch(
        empty_flag, empty_flag, 1, 1, 0, 1));
    assert(!angdroid_item_picker_blank_arrow_switch(
        empty_flag, empty_flag, 1, 1, 1, 1));
    /* This fallback must not activate for an empty shop. */
    assert(!angdroid_item_picker_blank_arrow_switch(
        empty_flag, empty_flag, 0, 1, 1, 0));
    assert(angdroid_item_picker_blank_arrow_switch(
        empty_flag, empty_flag, 1, 1, -1, 0));
    assert(angdroid_item_picker_blank_arrow_switch(
        empty_flag, empty_flag, 1, 1, 1, 0));
}

int main(void)
{
    available_tab_combinations();
    no_unusable_tab_is_advertised();
    placeholder_only_equipment_switches();
    blank_picker_direction_keys();
    puts("PASS: only actionable inventory/equipment/quiver/floor tab shortcuts");
    return 0;
}
