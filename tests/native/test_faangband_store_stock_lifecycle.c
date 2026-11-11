/* FAangband shop lifecycle: obsolete stock IDs must never survive a
 * reorder, while keyboard and shopkeeper commands remain usable in an
 * empty shop. Exercise the same native guards as the game.
 */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-action-guard.h"
#include "../../app/src/main/cpp/angband/store-stock-guard.h"
#include "../../app/src/main/cpp/angband/store-menu-row.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void empty_shop_commands(void)
{
    const int flag = 0x80; /* FAangband MN_ALLOW_EMPTY_INPUT */
    int flags, is_command;
    for (flags = 0; flags < 256; ++flags)
        for (is_command = 0; is_command <= 1; ++is_command) {
            int expected = !!(flags & flag) && is_command;
            CHECK(angdroid_menu_allow_empty_input(
                flags, flag, is_command) == expected);
        }
    CHECK(angdroid_menu_allow_empty_input(flag, flag, 1));
    CHECK(angdroid_menu_allow_empty_input(flag | 0x20, flag, 1));
    CHECK(!angdroid_menu_allow_empty_input(flag, flag, 0));
    CHECK(!angdroid_menu_allow_empty_input(0, flag, 1));
}

static void mutated_stock_requires_new_selection(void)
{
    int count, cursor;
    for (count = 0; count <= 40; ++count)
        for (cursor = -2; cursor <= 42; ++cursor) {
            int previous = cursor;
            int top = 7;
            angdroid_store_clear_stale_selection(true, &cursor, &top);
            CHECK(cursor == -1);
            CHECK(top == 0);
            CHECK(angdroid_store_selected_index(
                1, cursor, count, 40) == -1);
            cursor = previous;
            angdroid_store_clear_stale_selection(false, &cursor, &top);
            CHECK(cursor == previous);
        }
    CHECK(angdroid_store_selected_index(0, 1, 5, 5) == -1);
    CHECK(angdroid_store_selected_index(1, 1, 5, 5) == 1);
    CHECK(angdroid_store_selected_index(1, 1, 0, 5) == -1);
}

static void bounds_and_popup_after_reorder(void)
{
    int count, capacity, item;
    for (count = 0; count <= 20; ++count)
        for (capacity = 0; capacity <= 20; ++capacity)
            for (item = -1; item <= 21; ++item) {
                int valid = item >= 0 && item < count && item < capacity;
                CHECK(!!angdroid_store_stock_index_valid(
                    item, count, capacity) == valid);
                if (valid) {
                    int row = angdroid_store_menu_screen_row(
                        item, item, 4, 7);
                    CHECK(row == 4);
                }
            }
    CHECK(!angdroid_store_stock_index_valid(
        INT_MAX, INT_MAX, 20));
    CHECK(!angdroid_store_stock_index_valid(0, 0, 20));
    CHECK(angdroid_menu_action_item_index(-1, 0, 0, NULL) == -1);
}

int main(void)
{
    empty_shop_commands();
    mutated_stock_requires_new_selection();
    bounds_and_popup_after_reorder();
    printf("PASS: FA empty-shop commands, stock-generation invalidation and bounds (%lu checks)\n",
        checks);
    return 0;
}
