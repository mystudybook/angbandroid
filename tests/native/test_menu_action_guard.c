#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/menu-action-guard.h"

int main(void)
{
    enum { GRAY = 0x0001 };
    const int items[] = { GRAY, 0, GRAY, 0 };
    const int filtered[] = { 3, 2, 1 };
    int oid;

    /* A disabled FIRST entry cannot disable a later enabled action. */
    assert(!angdroid_menu_action_can_run(items[0], GRAY, 1));
    assert(angdroid_menu_action_can_run(items[1], GRAY, 1));

    /* A disabled SELECTED entry cannot execute merely because the
     * first item is enabled, including after a filter reorders rows. */
    oid = angdroid_menu_action_item_index(1, 3, 4, filtered);
    assert(oid == 2);
    assert(!angdroid_menu_action_can_run(items[oid], GRAY, 1));
    oid = angdroid_menu_action_item_index(0, 3, 4, filtered);
    assert(oid == 3);
    assert(angdroid_menu_action_can_run(items[oid], GRAY, 1));
    assert(!angdroid_menu_action_can_run(items[3], GRAY, 0));

    /* Empty, stale or malformed filtered indices never reach callbacks. */
    assert(angdroid_menu_action_item_index(-1, 3, 4, filtered) == -1);
    assert(angdroid_menu_action_item_index(3, 3, 4, filtered) == -1);
    assert(angdroid_menu_action_item_index(0, 0, 4, filtered) == -1);
    assert(angdroid_menu_action_item_index(0, 3, 0, filtered) == -1);
    assert(angdroid_menu_action_item_index(2, 4, 4, NULL) == 2);
    {
        const int bad[] = { -1, 50 };
        assert(angdroid_menu_action_item_index(0, 2, 4, bad) == -1);
        assert(angdroid_menu_action_item_index(1, 2, 4, bad) == -1);
    }

    /* Generic render, browse, valid_row and soft-keyboard paths use the
     * same mapping; these ids must never reach user-provided callbacks. */
    {
        const int stale[] = { INT_MIN, INT_MAX, 2, 0 };
        assert(angdroid_menu_action_item_index(0, 4, 4, stale) == -1);
        assert(angdroid_menu_action_item_index(1, 4, 4, stale) == -1);
        assert(angdroid_menu_action_item_index(2, 4, 4, stale) == 2);
        assert(angdroid_menu_action_item_index(3, 4, 4, stale) == 0);
        assert(angdroid_menu_action_item_index(INT_MAX, 4, 4, stale) == -1);
        assert(angdroid_menu_action_item_index(INT_MIN, 4, 4, stale) == -1);
        assert(angdroid_menu_action_item_index(0, INT_MIN, 4, stale) == -1);
        assert(angdroid_menu_action_item_index(0, 4, INT_MIN, stale) == -1);
    }

    /* Empty shops must stop advertising stock-only shortcuts, while all
     * other menu modes and a restocked shop preserve the existing keys. */
    {
        const char *stock_only = "pglx";
        const char *usable = "s?=Iie";
        const char *key;
        int flags, count;
        for (flags = 0; flags <= 255; ++flags) {
            for (count = 0; count <= 3; ++count) {
                int masked = (flags & 0x80) && count == 0;
                for (key = stock_only; *key; ++key)
                    assert(angdroid_menu_empty_shop_key_visible(
                        flags, 0x80, count, *key) == !masked);
                for (key = usable; *key; ++key)
                    assert(angdroid_menu_empty_shop_key_visible(
                        flags, 0x80, count, *key));
            }
        }
        assert(angdroid_menu_empty_shop_key_visible(
            0x80, 0x80, -1, 's'));
        assert(!angdroid_menu_empty_shop_key_visible(
            0x80, 0x80, -1, 'p'));
        assert(angdroid_menu_empty_shop_key_visible(
            0, 0x80, 0, 'p'));
    }

    puts("PASS: selected action state and filtered menu callback index bounds");
    return 0;
}
