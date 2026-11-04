#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/menu-hit-test.h"

int main(void)
{
    /* Short scroll menu: tapping a blank row must not select the last item. */
    assert(angdroid_menu_scrolling_hit(1, 1, 8, 0, 3) == 0);
    assert(angdroid_menu_scrolling_hit(3, 1, 8, 0, 3) == 2);
    assert(angdroid_menu_scrolling_hit(4, 1, 8, 0, 3) == -1);
    assert(angdroid_menu_scrolling_hit(0, 1, 8, 0, 3) == -1);
    /* After a page scroll, only drawn items can be tapped. */
    assert(angdroid_menu_scrolling_hit(12, 10, 4, 5, 9) == 7);
    assert(angdroid_menu_scrolling_hit(13, 10, 4, 5, 8) == -1);
    assert(angdroid_menu_scrolling_hit(10, 10, 4, 5, 8) == 5);
    /* Rotation changes the terminal row count; old blank rows stay invalid. */
    assert(angdroid_menu_scrolling_hit(13, 10, 2, 5, 8) == -1);

    /* Two full columns plus a partial third column of length two. */
    assert(angdroid_menu_columns_hit(2, 5, 2, 5, 4, 40, 10, 10) == 0);
    assert(angdroid_menu_columns_hit(3, 15, 2, 5, 4, 40, 10, 10) == 5);
    assert(angdroid_menu_columns_hit(3, 25, 2, 5, 4, 40, 10, 10) == 9);
    assert(angdroid_menu_columns_hit(4, 25, 2, 5, 4, 40, 10, 10) == -1);
    assert(angdroid_menu_columns_hit(2, 35, 2, 5, 4, 40, 10, 10) == -1);
    assert(angdroid_menu_columns_hit(6, 15, 2, 5, 4, 40, 10, 10) == -1);
    assert(angdroid_menu_columns_hit(1, 15, 2, 5, 4, 40, 10, 10) == -1);
    assert(angdroid_menu_columns_hit(3, 4, 2, 5, 4, 40, 10, 10) == -1);
    /* Narrow terminals and rapidly changing row/column geometry. */
    assert(angdroid_menu_columns_hit(1, 0, 0, 0, 2, 2, 1, 3) == 1);
    assert(angdroid_menu_columns_hit(1, 1, 0, 0, 2, 2, 1, 3) == -1);
    assert(angdroid_menu_columns_hit(0, 0, 0, 0, 0, 10, 1, 3) == -1);
    assert(angdroid_menu_columns_hit(0, 0, 0, 0, 2, 10, 0, 3) == -1);
    assert(angdroid_menu_columns_hit(INT_MAX, INT_MAX,
        INT_MIN, INT_MIN, 4, 40, 10, 3) == -1);
    assert(angdroid_menu_scrolling_hit(INT_MAX, INT_MIN, 4, 0, 3) == -1);
    assert(angdroid_menu_scrolling_hit(0, 0, 4, INT_MAX, 3) == -1);
    assert(angdroid_menu_scrolling_hit(0, 0, 4, 0, 0) == -1);

    puts("PASS: Angband menu hit-testing never selects blank terminal cells");
    return 0;
}
