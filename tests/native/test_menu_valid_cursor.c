#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/menu-valid-cursor.h"

struct menu_data {
    int count;
    const char *valid;
};

static int valid_row(void *context, int row)
{
    struct menu_data *menu = (struct menu_data *)context;
    assert(row >= 0 && row < menu->count);
    return menu->valid[row] != 0;
}

int main(void)
{
    const char flags[] = { 1, 0, 1, 0, 0, 1 };
    const char disabled[] = { 0, 0, 0, 0 };
    struct menu_data menu = { 6, flags };
    struct menu_data none = { 4, disabled };

    /* A column move with dy=0 and invalid target must find another row. */
    assert(angdroid_menu_find_valid_cursor(1, 6, 1, &menu, valid_row) == 2);
    assert(angdroid_menu_find_valid_cursor(1, 6, -1, &menu, valid_row) == 0);
    assert(angdroid_menu_find_valid_cursor(3, 6, 1, &menu, valid_row) == 5);
    assert(angdroid_menu_find_valid_cursor(3, 6, -1, &menu, valid_row) == 2);

    /* Valid rows stay put; negative, oversized and extreme cursors wrap. */
    assert(angdroid_menu_find_valid_cursor(5, 6, 1, &menu, valid_row) == 5);
    assert(angdroid_menu_find_valid_cursor(6, 6, 1, &menu, valid_row) == 0);
    assert(angdroid_menu_find_valid_cursor(-1, 6, 1, &menu, valid_row) == 5);
    assert(angdroid_menu_find_valid_cursor(INT_MAX, 6, 1, &menu, valid_row) >= 0);
    assert(angdroid_menu_find_valid_cursor(INT_MIN, 6, -1, &menu, valid_row) >= 0);

    /* No valid item and empty menu return safely without hanging. */
    assert(angdroid_menu_find_valid_cursor(0, 4, 1, &none, valid_row) == -1);
    assert(angdroid_menu_find_valid_cursor(0, 0, 1, &none, valid_row) == -1);
    assert(angdroid_menu_find_valid_cursor(0, 6, 1, &menu, NULL) == -1);

    /* Spacebar paging must reach the last row, not wrap one row early. */
    assert(angdroid_menu_page_next_cursor(0, 9, 4) == 4);
    assert(angdroid_menu_page_next_cursor(4, 9, 4) == 8);
    assert(angdroid_menu_page_next_cursor(8, 9, 4) == 0);
    assert(angdroid_menu_page_next_cursor(4, 5, 1) == 0);
    assert(angdroid_menu_page_next_cursor(-1, 9, 4) == 0);
    assert(angdroid_menu_page_next_cursor(99, 9, 4) == 0);
    assert(angdroid_menu_page_next_cursor(0, 4, 4) == -1);
    assert(angdroid_menu_page_next_cursor(0, 4, 0) == -1);
    assert(angdroid_menu_page_next_cursor(INT_MAX, INT_MAX, 1) == 0);
    assert(angdroid_menu_page_next_cursor(INT_MAX - 2, INT_MAX, 1)
        == INT_MAX - 1);
    {
        struct menu_data only_last = { 6, flags };
        int candidate = angdroid_menu_page_next_cursor(0, 6, 4);
        assert(candidate == 4);
        assert(angdroid_menu_find_valid_cursor(
            candidate, 6, 1, &only_last, valid_row) == 5);
    }

    puts("PASS: horizontal menu navigation always terminates on invalid rows");
    return 0;
}
