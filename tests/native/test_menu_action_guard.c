#include <assert.h>
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

    puts("PASS: selected action state and empty filtered menu callback safety");
    return 0;
}
