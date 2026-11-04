#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/menu-key-hints.h"

int main(void)
{
    const int filtered[] = { 4, 1, 3 };
    char hints[8] = "";
    char minimal[1] = "";
    size_t length = 0;
    int i;

    assert(angdroid_menu_hint_item_index(NULL, 2) == 2);
    assert(angdroid_menu_hint_item_index(filtered, 0) == 4);
    assert(angdroid_menu_hint_item_index(filtered, 1) == 1);
    assert(angdroid_menu_hint_item_index(filtered, 2) == 3);

    length = angdroid_menu_hint_append(hints, length, sizeof(hints), 'a');
    length = angdroid_menu_hint_append(hints, length, sizeof(hints), 0);
    assert(length == 1 && strcmp(hints, "a") == 0);
    for (i = 0; i < 1000; ++i)
        length = angdroid_menu_hint_append(hints, length, sizeof(hints), 'x');
    assert(length == sizeof(hints) - 1);
    assert(strcmp(hints, "axxxxxx") == 0);
    assert(angdroid_menu_hint_append(minimal, 0, sizeof(minimal), 'z') == 0);
    assert(minimal[0] == '\0');
    assert(angdroid_menu_hint_append(NULL, 0, sizeof(hints), 'z') == 0);
    assert(angdroid_menu_hint_append(hints, length, 0, 'z') == length);

    puts("PASS: filtered menu shortcuts and bounded keyboard hints");
    return 0;
}
