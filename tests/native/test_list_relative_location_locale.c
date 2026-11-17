/* Side-list directions must stay within UTF-8 and terminal width budgets. */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-list-location.h"
#include "../../app/src/main/cpp/angband/monster-list-compact-layout.h"
#include "../../app/src/main/cpp/angband/object-list-compact-layout.h"

static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static void check_location(int dy, int dx, const char *english,
    const char *traditional)
{
    char en[64], zh[64];
    int width;
    angdroid_list_relative_location(en, sizeof en, dy, dx, 0);
    angdroid_list_relative_location(zh, sizeof zh, dy, dx, 1);
    CHECK(strcmp(en, english) == 0);
    CHECK(strcmp(zh, traditional) == 0);
    /* " 12 S 35 W" and " 12南 35西" take the same screen columns. */
    CHECK(angdroid_utf8_display_columns(en) ==
          angdroid_utf8_display_columns(zh));
    CHECK(strlen(zh) > strlen(en));
    for (width = 0; width < 35; ++width) {
        struct angdroid_monster_row_budget men =
            angdroid_monster_row_fit(width,
                angdroid_utf8_display_columns(en));
        struct angdroid_monster_row_budget mzh =
            angdroid_monster_row_fit(width,
                angdroid_utf8_display_columns(zh));
        struct angdroid_object_row_budget oen =
            angdroid_object_row_fit(width,
                angdroid_utf8_display_columns(en));
        struct angdroid_object_row_budget ozh =
            angdroid_object_row_fit(width,
                angdroid_utf8_display_columns(zh));
        CHECK(men.show_location == mzh.show_location);
        CHECK(men.name_columns == mzh.name_columns);
        CHECK(oen.show_location == ozh.show_location);
        CHECK(oen.name_columns == ozh.name_columns);
        if (mzh.show_location) CHECK(mzh.name_columns >= 4);
        if (ozh.show_location) CHECK(ozh.name_columns >= 8);
    }
}

int main(void)
{
    char out[20];
    char tiny[8];
    char exact[13];
    char full[64];
    check_location(12, -35, " 12 S 35 W", " 12南 35西");
    check_location(-4, 9, " 4 N 9 E", " 4北 9東");
    check_location(0, 0, " 0 N 0 W", " 0北 0西");
    check_location(0, 12, " 0 N 12 E", " 0北 12東");
    check_location(127, 7, " 127 S 7 E", " 127南 7東");

    angdroid_list_relative_location(out, sizeof out, 12, -35, 1);
    CHECK(strcmp(out, " 12南 35西") == 0);
    angdroid_list_relative_location(tiny, sizeof tiny, 12, -35, 1);
    CHECK(tiny[0] == '\0');
    angdroid_list_relative_location(exact, sizeof exact, 12, -35, 1);
    CHECK(strcmp(exact, " 12南 35西") == 0);
    angdroid_list_relative_location(exact, sizeof exact - 1, 12, -35, 1);
    CHECK(exact[0] == '\0');

    /* Original English output is left unchanged (including zero directions). */
    angdroid_list_relative_location(out, sizeof out, 12, -35, 0);
    CHECK(strcmp(out, " 12 S 35 W") == 0);
    angdroid_list_relative_location(out, sizeof out, 0, 0, 0);
    CHECK(strcmp(out, " 0 N 0 W") == 0);

    /* The signed INT_MIN case must not pass through abs(INT_MIN), which is UB. */
    angdroid_list_relative_location(full, sizeof full, INT_MIN, INT_MAX, 1);
    CHECK(strcmp(full, " 2147483648北 2147483647東") == 0);
    angdroid_list_relative_location(full, sizeof full, INT_MIN, INT_MAX, 0);
    CHECK(strcmp(full, " 2147483648 N 2147483647 E") == 0);
    angdroid_list_relative_location(out, sizeof out, INT_MIN, INT_MIN, 1);
    CHECK(out[0] == '\0');
    angdroid_list_relative_location(out, sizeof out, INT_MIN, INT_MIN, 0);
    CHECK(out[0] == '\0');
    angdroid_list_relative_location(NULL, 0, 12, -35, 1);
    angdroid_list_relative_location(out, 0, 12, -35, 1);

    CHECK(angdroid_monster_row_fit(24, 10).show_location);
    CHECK(angdroid_object_row_fit(24, 10).show_location);
    CHECK(!angdroid_monster_row_fit(13, 10).show_location);
    CHECK(!angdroid_object_row_fit(20, 10).show_location);
    printf("PASS: bilingual list location safe widths (%u checks)\n", checks);
    return 0;
}
