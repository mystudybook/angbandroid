/*
 * FAangband monster list compact geometry / UTF-8 visible rows.
 * Compile the exact shared production helpers (not copies).
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/monster-list-compact-layout.h"
#include "app/src/main/cpp/angband/zh-tw-list-location.h"
#include "app/src/main/cpp/angband/zh-tw-monster-list-ui.h"
#include "app/src/main/cpp/curses/unicode_display.h"

static int checks;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL at %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    struct angdroid_monster_row_budget budget;
    struct angdroid_monster_list_popup popup;
    char distance[40], label[40], clipped[8];

    budget = angdroid_monster_row_fit(6, 12);
    CHECK(budget.name_columns == 3 && !budget.show_location);
    budget = angdroid_monster_row_fit(11, 10);
    CHECK(budget.name_columns == 8 && !budget.show_location);
    budget = angdroid_monster_row_fit(24, 9);
    CHECK(budget.name_columns == 12 && budget.show_location);
    budget = angdroid_monster_row_fit(2, 12);
    CHECK(budget.name_columns == 0 && !budget.show_location);
    CHECK(!angdroid_monster_sleep_fits(8, 8));
    CHECK(!angdroid_monster_sleep_fits(8, 5));
    CHECK(angdroid_monster_sleep_fits(12, 8));
    CHECK(!angdroid_monster_sleep_fits(0, 20));

    CHECK(!angdroid_monster_list_popup_fit(11, 18, 50, 9, &popup));
    CHECK(!angdroid_monster_list_popup_fit(32, 6, 50, 9, &popup));
    CHECK(angdroid_monster_list_popup_fit(32, 18, 50, 9, &popup));
    CHECK(popup.width == 30 && popup.col == 2);
    CHECK(popup.row == 1 && popup.height == 12);
    CHECK(angdroid_monster_list_popup_fit(80, 25, 30, 10, &popup));
    CHECK(popup.width == 30 && popup.col == 50);

    angdroid_list_relative_location(distance, sizeof(distance),
        -3, 8, 1);
    CHECK(strcmp(distance, " 3北 8東") == 0);
    CHECK(angdroid_utf8_display_columns(distance) == 6);
    angdroid_list_relative_location(distance, sizeof(distance),
        -3, 8, 0);
    CHECK(strcmp(distance, " 3 N 8 E") == 0);
    CHECK(angdroid_utf8_display_columns(distance) == 8);

    CHECK(angdroid_utf8_display_columns("獸人") == 4);
    (void)angdroid_utf8_copy_complete(label, sizeof(label),
        "獸人（睡眠中）");
    CHECK(angdroid_utf8_display_columns(label) == 14);
    angdroid_utf8_clip_columns(label, 5);
    CHECK(strcmp(label, "獸人") == 0);
    (void)angdroid_utf8_copy_complete(clipped, sizeof(clipped), "獸人");
    CHECK(strcmp(clipped, "獸人") == 0);
    (void)angdroid_utf8_copy_complete(clipped, 5, "獸人");
    CHECK(strcmp(clipped, "獸") == 0);
    CHECK(angdroid_utf8_printf_field_bytes("獸人", 8) == 10);

    CHECK(strcmp(angdroid_monster_list_sort_hint(1, 0),
        "按 x 開啟依經驗值排序") == 0);
    CHECK(strcmp(angdroid_monster_list_sort_hint(0, 1),
        "Press 'x' to turn OFF 'sort by exp'") == 0);
    CHECK(angdroid_monster_list_hallucination_columns(1) > 0);

    printf("PASS: %d FA monster list layout and UTF-8 checks\n", checks);
    return 0;
}
