#include <assert.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-column-width.h"

int main(void)
{
    assert(angdroid_menu_fit_column_width(23, 120, 0, 2) == 23);
    assert(angdroid_menu_fit_column_width(23, 40, 5, 2) == 17);
    assert(angdroid_menu_fit_column_width(23, 12, 2, 10) == 1);
    assert(angdroid_menu_fit_column_width(23, 8, 2, 20) == 1);
    assert(angdroid_menu_fit_column_width(23, 8, 8, 2) == 1);
    assert(angdroid_menu_fit_column_width(23, 8, 9, 2) == 1);
    assert(angdroid_menu_fit_column_width(23, 80, 0, 0) == 1);
    assert(angdroid_menu_fit_column_width(0, 80, 0, 2) == 1);
    puts("PASS: narrow multi-column menu width never becomes zero");
    return 0;
}
