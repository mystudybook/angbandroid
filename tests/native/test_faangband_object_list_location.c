/* C99 regression: FAangband visible items use terminal-cell dimensions.
 * Test the production helpers shared with Angband, not a copied algorithm.
 */
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/object-list-compact-layout.h"
#include "../../app/src/main/cpp/angband/zh-tw-list-location.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void check_utf8(const char *str)
{
    size_t i = 0, len = strlen(str);
    while (i < len) {
        uint32_t cp;
        size_t n = angdroid_utf8_next_scalar(str + i, &cp);
        CHECK(n >= 1 && n <= len - i);
        CHECK(cp != 0xFFFDu);
        i += n;
    }
    CHECK(i == len);
}

static void directions_and_distances(void)
{
    char text[128], short_text[20] = "unchanged";
    angdroid_list_relative_location(text, sizeof(text), -12, 35, 1);
    CHECK(strcmp(text, " 12北 35東") == 0);
    CHECK(angdroid_utf8_display_columns(text) == 10);
    check_utf8(text);

    angdroid_list_relative_location(text, sizeof(text), -12, 35, 0);
    CHECK(strcmp(text, " 12 N 35 E") == 0);
    CHECK(angdroid_utf8_display_columns(text) == 10);

    angdroid_list_relative_location(text, sizeof(text), 5, -3, 1);
    CHECK(strcmp(text, " 5南 3西") == 0);
    CHECK(angdroid_utf8_display_columns(text) == 8);

    angdroid_list_relative_location(text, sizeof(text), 0, 0, 1);
    CHECK(strcmp(text, " 0北 0西") == 0);
    angdroid_list_relative_location(text, sizeof(text), INT_MIN, INT_MIN, 1);
    CHECK(strcmp(text, " 2147483648北 2147483648西") == 0);
    check_utf8(text);
    angdroid_list_relative_location(text, sizeof(text), INT_MAX, INT_MAX, 0);
    CHECK(strcmp(text, " 2147483647 S 2147483647 E") == 0);

    angdroid_list_relative_location(NULL, 30, 2, 3, 1);
    angdroid_list_relative_location(short_text, 0, 2, 3, 1);
    CHECK(strcmp(short_text, "unchanged") == 0);

    {
        size_t cap;
        for (cap = 1; cap <= sizeof(short_text); ++cap) {
            char memory[24], *out = memory + 1;
            size_t full_len = strlen(" 12北 35東");
            memset(memory, '#', sizeof(memory));
            angdroid_list_relative_location(out, cap, -12, 35, 1);
            CHECK(memory[0] == '#');
            CHECK(memory[cap + 1] == '#');
            CHECK(memory[23] == '#');
            if (cap > full_len) {
                CHECK(strcmp(out, " 12北 35東") == 0);
            } else {
                CHECK(out[0] == '\0');
            }
        }
    }
}

static void narrow_rows(void)
{
    int width;
    size_t loc;
    for (width = -2; width <= 160; ++width) {
        for (loc = 0; loc <= 35; ++loc) {
            struct angdroid_object_row_budget fit =
                angdroid_object_row_fit(width, loc);
            CHECK(fit.name_columns <=
                (width > 3 ? (size_t)(width - 3) : 0));
            if (fit.show_location) {
                CHECK(loc > 0);
                CHECK(fit.name_columns >= 8);
                CHECK(fit.name_columns + loc + 3 == (size_t)width);
            } else if (width > 3) {
                CHECK(fit.name_columns + 3 == (size_t)width);
            }
        }
    }
    CHECK(angdroid_object_row_fit(25, 10).show_location);
    CHECK(angdroid_object_row_fit(25, 10).name_columns == 12);
    CHECK(!angdroid_object_row_fit(20, 10).show_location);
    CHECK(angdroid_object_row_fit(20, 10).name_columns == 17);
    CHECK(angdroid_object_row_fit(3, SIZE_MAX).name_columns == 0);
    CHECK(!angdroid_object_row_fit(3, SIZE_MAX).show_location);
}

static void chinese_name_and_position(void)
{
    const char *source = "  2 題為「神祕法術」的卷軸";
    int width;
    for (width = 0; width <= 84; ++width) {
        char name[128];
        struct angdroid_object_row_budget budget =
            angdroid_object_row_fit(width, 10);
        size_t field_bytes, visible_cols;
        strcpy(name, source);
        angdroid_utf8_clip_columns(name, budget.name_columns);
        visible_cols = angdroid_utf8_display_columns(name);
        CHECK(visible_cols <= budget.name_columns);
        check_utf8(name);
        field_bytes = angdroid_utf8_printf_field_bytes(
            name, budget.name_columns);
        CHECK(field_bytes >= strlen(name));
        CHECK(field_bytes - strlen(name) +
            angdroid_utf8_display_columns(name) == budget.name_columns);
        CHECK(strcmp(source, "  2 題為「神祕法術」的卷軸") == 0);
    }
}

static void popup_rotation(void)
{
    int w, h;
    for (w = 0; w <= 160; ++w)
        for (h = 0; h <= 60; ++h) {
            struct angdroid_object_list_popup popup;
            bool shown = angdroid_object_list_popup_fit(
                w, h, 1000, 1000, &popup);
            CHECK(shown == (w >= 12 && h >= 7));
            if (shown) {
                CHECK(popup.col >= 0);
                CHECK(popup.col + popup.width <= w);
                CHECK(popup.row == 1);
                CHECK(popup.row + popup.height + 1 < h);
                CHECK(popup.width >= 7);
                CHECK(popup.height >= 4);
            } else {
                CHECK(popup.width == 0 && popup.height == 0);
            }
        }
    {
        struct angdroid_object_list_popup popup;
        CHECK(angdroid_object_list_popup_fit(
            40, 20, 1000, 1000, &popup));
        CHECK(popup.width == 38 && popup.col == 2);
        CHECK(angdroid_object_list_popup_fit(
            80, 24, 1000, 1000, &popup));
        CHECK(popup.width == 67 && popup.col == 13);
        CHECK(!angdroid_object_list_popup_fit(
            80, 24, 90, 20, NULL));
    }
}

int main(void)
{
    directions_and_distances();
    narrow_rows();
    chinese_name_and_position();
    popup_rotation();
    printf("PASS: FA dungeon CJK location, row budget, portrait popup (%lu checks)\n",
        checks);
    return 0;
}
