#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/menu-text-fit.h"

static void check(const char *original, size_t cols,
    const char *expected, size_t expected_columns)
{
    char buffer[256];
    size_t used = angdroid_menu_copy_columns(buffer, sizeof(buffer),
        original, cols);
    assert(strcmp(buffer, expected) == 0);
    assert(used == expected_columns);
    assert(angdroid_utf8_display_columns(buffer) == used);
}

int main(void)
{
    char a[4] = "XYZ";
    char single[1] = { 'Q' };
    char broken[] = { (char)0xE4, 'Z', '\0' };
    char tiny[3] = { 'Z', 'Z', 'Z' };

    check("A中文B", 0, "", 0);
    check("A中文B", 1, "A", 1);
    check("A中文B", 2, "A", 1);
    check("A中文B", 3, "A中", 3);
    check("A中文B", 4, "A中", 3);
    check("A中文B", 5, "A中文", 5);
    check("A中文B", 6, "A中文B", 6);
    check("繁體中文選單", 6, "繁體中", 6);
    check("Sword", 3, "Swo", 3);
    check("", 10, "", 0);
    check(NULL, 10, "", 0);
    check(broken, 5, "?Z", 2);

    assert(angdroid_menu_copy_columns(a, sizeof(a), "中", 3) == 2);
    assert(strcmp(a, "中") == 0);
    assert(angdroid_menu_copy_columns(tiny, sizeof(tiny), "中", 3) == 0);
    assert(tiny[0] == '\0'); /* Never copy half a three-byte scalar. */
    assert(angdroid_menu_copy_columns(single, sizeof(single), "X", 5) == 0);
    assert(single[0] == '\0');
    assert(angdroid_menu_copy_columns(NULL, 10, "X", 5) == 0);
    assert(angdroid_menu_copy_columns(a, 0, "X", 5) == 0);

    assert(angdroid_menu_tag_columns(-99) == 0);
    assert(angdroid_menu_tag_columns(0) == 0);
    assert(angdroid_menu_tag_columns(1) == 1);
    assert(angdroid_menu_tag_columns(2) == 2);
    assert(angdroid_menu_tag_columns(3) == 3);
    assert(angdroid_menu_tag_columns(99) == 3);

    puts("PASS: native menu CJK clipping and narrow shortcut prefix");
    return 0;
}
