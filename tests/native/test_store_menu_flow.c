#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/store-menu-row.h"
#include "../../app/src/main/cpp/angband/menu-action-guard.h"

int main(void)
{
    enum { ALLOW_EMPTY = 0x80 };
    const int filtered[] = { 9, 3 };

    /* A scrolled store still opens its popup over the tapped stock row. */
    assert(angdroid_store_menu_screen_row(6, 6, 4, 8) == 4);
    assert(angdroid_store_menu_screen_row(10, 6, 4, 8) == 8);
    assert(angdroid_store_menu_screen_row(13, 6, 4, 8) == 11);
    assert(angdroid_store_menu_screen_row(5, 6, 4, 8) == -1);
    assert(angdroid_store_menu_screen_row(14, 6, 4, 8) == -1);
    assert(angdroid_store_menu_screen_row(-1, 0, 4, 8) == -1);
    assert(angdroid_store_menu_screen_row(1, -1, 4, 8) == -1);
    assert(angdroid_store_menu_screen_row(1, 0, 4, 0) == -1);

    /* Large offsets never overflow native signed-int terminal positions. */
    assert(angdroid_store_menu_screen_row(INT_MAX, 0, 4, INT_MAX) == -1);
    assert(angdroid_store_menu_screen_row(INT_MAX, INT_MAX, 3, 8) == 3);
    assert(angdroid_store_menu_screen_row(0, 0, -1, 8) == -1);

    /* Sell/help/header commands remain available without shop stock. */
    assert(angdroid_menu_allow_empty_input(ALLOW_EMPTY, ALLOW_EMPTY, 1));
    assert(angdroid_menu_allow_empty_input(ALLOW_EMPTY | 0x20, ALLOW_EMPTY, 1));
    assert(!angdroid_menu_allow_empty_input(0, ALLOW_EMPTY, 1));
    assert(!angdroid_menu_allow_empty_input(ALLOW_EMPTY, ALLOW_EMPTY, 0));

    /* Empty event allowance must not make invalid stock selectable. */
    assert(angdroid_menu_action_item_index(-1, 0, 0, NULL) == -1);
    assert(angdroid_menu_action_item_index(1, 2, 10, filtered) == 3);
    assert(angdroid_menu_action_item_index(2, 2, 10, filtered) == -1);

    puts("PASS: scrolled store popup coordinates and empty-store commands");
    return 0;
}
