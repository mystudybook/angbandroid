#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/menu-dynamic-choice.h"

int main(void)
{
    /* A genuine click/Enter chooses the actual cursor. */
    assert(angdroid_menu_dynamic_choice_index(1, 0, 0, -1, 3) == 0);
    assert(angdroid_menu_dynamic_choice_index(1, 0, 2, -1, 3) == 2);

    /* The store's g shortcut means Buy (tag p), NOT whichever dynamic
     * row happened to have focus, such as Examine or Buy one. */
    assert(angdroid_menu_dynamic_choice_index(0, 1, 0, 1, 3) == 1);
    assert(angdroid_menu_dynamic_choice_index(0, 1, 2, 1, 3) == 1);

    /* Unknown keyboard/resize events and ESC must never execute the
     * current row just because its cursor is valid. */
    assert(angdroid_menu_dynamic_choice_index(0, 0, 0, -1, 3) == -1);
    assert(angdroid_menu_dynamic_choice_index(0, 0, 2, 1, 3) == -1);

    /* Read-only context menus have no Buy item to alias to. */
    assert(angdroid_menu_dynamic_choice_index(0, 0, 0, -1, 1) == -1);
    assert(angdroid_menu_dynamic_choice_index(0, 1, 0, -1, 1) == -1);

    assert(angdroid_menu_dynamic_choice_index(1, 0, -1, 1, 3) == -1);
    assert(angdroid_menu_dynamic_choice_index(1, 0, 3, 1, 3) == -1);
    assert(angdroid_menu_dynamic_choice_index(0, 1, 0, 5, 3) == -1);
    assert(angdroid_menu_dynamic_choice_index(1, 0, 0, 0, 0) == -1);
    assert(angdroid_menu_dynamic_choice_index(1, 0, INT_MAX, 0, 3) == -1);

    /* An actual user SELECT takes priority over an unrelated alias. */
    assert(angdroid_menu_dynamic_choice_index(1, 1, 2, 1, 3) == 2);

    puts("PASS: dynamic menu shortcuts cannot select the wrong focused row");
    return 0;
}
