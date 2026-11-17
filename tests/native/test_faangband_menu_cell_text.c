/* FAangband generic menu terminal-cell text and bounded row placement.
 * Compile against production UTF-8 helpers, independently of Android SDK.
 */
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/ui-menu-cell-text.h"

static unsigned long checks;
#define CHECK(test) do { assert(test); ++checks; } while (0)

static void check_utf8(const char *str)
{
    const size_t length = strlen(str);
    size_t pos = 0;
    while (pos < length) {
        uint32_t cp = 0;
        size_t n = angdroid_utf8_next_scalar(str + pos, &cp);
        CHECK(n > 0 && n <= length - pos);
        CHECK(cp != 0xFFFDu);
        pos += n;
    }
    CHECK(pos == length);
}

static int expected_columns(int width, int height,
                            int row, int col, int requested)
{
    if (width <= 0 || height <= 0 || row < 0 || row >= height ||
        col < 0 || col >= width || requested <= 0)
        return 0;
    return (requested < width - col) ? requested : width - col;
}

static void test_row_rectangles(void)
{
    int width, height, row, col, requested;
    for (width = -2; width <= 32; ++width)
        for (height = -2; height <= 14; ++height)
            for (row = -2; row <= 15; ++row)
                for (col = -2; col <= 33; ++col)
                    for (requested = -2; requested <= 35; ++requested) {
                        int got = angdroid_fa_menu_visible_columns(
                            width, height, row, col, requested);
                        CHECK(got == expected_columns(
                            width, height, row, col, requested));
                        CHECK(got >= 0);
                        CHECK(got <= width - (col >= 0 ? col : 0) ||
                              got == 0);
                    }
}

static void test_chinese_labels(void)
{
    char out[128];
    size_t n;
    CHECK(strlen("背包物品") == 12);
    CHECK(angdroid_utf8_display_columns("背包物品") == 8);
    CHECK(angdroid_utf8_display_columns("a) 長劍") == 7);
    CHECK(angdroid_utf8_display_columns("說明：背包") == 10);

    n = angdroid_fa_menu_copy_visible(out, sizeof(out), "背包物品",
        10, 5, 1, 4, 10);
    CHECK(n == 6 && strcmp(out, "背包物") == 0);
    check_utf8(out);

    n = angdroid_fa_menu_copy_visible(out, sizeof(out), "a) 長劍",
        6, 5, 1, 0, 6);
    CHECK(n == 5 && strcmp(out, "a) 長") == 0);
    n = angdroid_fa_menu_copy_visible(out, sizeof(out), "a) 長劍",
        7, 5, 1, 0, 7);
    CHECK(n == 7 && strcmp(out, "a) 長劍") == 0);
    n = angdroid_fa_menu_copy_visible(out, sizeof(out), "長劍",
        1, 5, 1, 0, 1);
    CHECK(n == 0 && out[0] == '\0');

    n = angdroid_fa_menu_copy_visible(out, sizeof(out), "中文與 English",
        20, 5, 1, 0, 20);
    CHECK(n == angdroid_utf8_display_columns("中文與 English"));
    CHECK(strcmp(out, "中文與 English") == 0);
}

static void test_buffer_capacities(void)
{
    const char *input = "a) 題為「神秘魔法」的卷軸";
    int screen, col;
    size_t cap;
    for (screen = 0; screen <= 48; ++screen)
        for (col = -2; col <= 48; ++col)
            for (cap = 0; cap <= 48; ++cap) {
                char buffer[52];
                char *out = buffer + 1;
                size_t n, length;
                int allowed = expected_columns(
                    screen, 12, 3, col, 48);
                memset(buffer, 'X', sizeof(buffer));
                n = angdroid_fa_menu_copy_visible(out, cap, input,
                    screen, 12, 3, col, 48);
                CHECK(buffer[0] == 'X');
                CHECK(buffer[cap + 1] == 'X');
                CHECK(buffer[51] == 'X');
                if (!cap) {
                    CHECK(n == 0 && out[0] == 'X');
                    continue;
                }
                length = strlen(out);
                CHECK(length < cap);
                CHECK(n == angdroid_utf8_display_columns(out));
                CHECK(n <= (size_t)allowed);
                CHECK(strncmp(out, input, length) == 0);
                check_utf8(out);
            }
}

static void test_extreme_geometry(void)
{
    CHECK(angdroid_fa_menu_visible_columns(
        INT_MAX, INT_MAX, INT_MAX - 1, INT_MAX - 1, INT_MAX) == 1);
    CHECK(angdroid_fa_menu_visible_columns(
        INT_MAX, INT_MAX, 0, 0, INT_MAX) == INT_MAX);
    CHECK(angdroid_fa_menu_visible_columns(
        INT_MIN, INT_MAX, 0, 0, INT_MAX) == 0);
    CHECK(angdroid_fa_menu_visible_columns(
        INT_MAX, INT_MIN, 0, 0, INT_MAX) == 0);
    CHECK(angdroid_fa_menu_visible_columns(
        INT_MAX, INT_MAX, INT_MAX, 0, 1) == 0);
    CHECK(angdroid_fa_menu_copy_visible(NULL, 10, "item",
        80, 24, 0, 0, 10) == 0);
}

int main(void)
{
    test_row_rectangles();
    test_chinese_labels();
    test_buffer_capacities();
    test_extreme_geometry();
    printf("PASS: FA Unicode menu text and row geometry (%lu checks)\n", checks);
    return 0;
}
