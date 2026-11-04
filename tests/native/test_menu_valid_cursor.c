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

    puts("PASS: horizontal menu navigation always terminates on invalid rows");
    return 0;
}
