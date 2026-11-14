/* Equipment slot labels occupy 14 terminal columns, not 14 codepoints. */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/inventory-equip-mention.h"

static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static void check_label(const char *input, const char *expected)
{
    char display[80];
    size_t cols = angdroid_inventory_equip_mention(display,
        sizeof display, input);
    CHECK(cols == 14);
    CHECK(strcmp(display, expected) == 0);
    CHECK(angdroid_utf8_display_columns(display) == 14);
}

int main(void)
{
    char tiny[3], exact[4], partial[8], untouched = 'X';
    char invalid[] = {(char)0xFF, '\0'};
    check_label("Head", "Head          ");
    check_label("Chest armor", "Chest armor   ");
    check_label("abcdefghijklmnop", "abcdefghijklmn");
    check_label("頭部", "頭部          ");
    check_label("一二三四五六七八", "一二三四五六七");
    check_label("一二三四五六七A", "一二三四五六七");
    check_label("頭盔 Helm of Eternity", "頭盔 Helm of E");
    check_label("武器 (weapon)", "武器 (weapon) ");
    check_label("", "              ");
    check_label(invalid, "?             ");

    CHECK(angdroid_inventory_equip_mention(tiny, sizeof tiny,
        "中文字") == 2);
    CHECK(strcmp(tiny, "  ") == 0);
    CHECK(angdroid_inventory_equip_mention(exact, sizeof exact,
        "中文字") == 2);
    CHECK(strcmp(exact, "中") == 0);
    CHECK(angdroid_inventory_equip_mention(partial, sizeof partial,
        "中文字") == 5);
    CHECK(strcmp(partial, "中文 ") == 0);
    CHECK(angdroid_inventory_equip_mention(tiny, 0, "頭部") == 0);
    CHECK(angdroid_inventory_equip_mention(NULL, 0, "頭部") == 0);
    CHECK(angdroid_inventory_equip_mention(&untouched, 1, "頭部") == 0);
    CHECK(untouched == '\0');
    CHECK(angdroid_inventory_equip_mention(exact, sizeof exact, NULL) == 0);
    CHECK(exact[0] == '\0');

    puts("PASS: equipment mention 14 display cells, UTF-8 and byte capacity");
    return 0;
}
