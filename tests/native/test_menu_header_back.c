/* Menu Back must be limited to the visible list's left margin.
 * Header clicks are a different action, and right-click has its own path.
 */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-hit-test.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void menu_header_and_left_margin(void)
{
    int y, x, top, page;
    for (top = 0; top <= 45; ++top) {
        for (page = 0; page <= 35; ++page) {
            for (y = -2; y <= 88; ++y) {
                for (x = -2; x <= 6; ++x) {
                    int inside = page > 0 && x >= 0 && x < 1 &&
                        y >= top && (int64_t)y - top < page;
                    CHECK(!!angdroid_menu_back_margin_hit(
                        y, x, top, 1, page) == inside);
                }
            }
        }
    }
    /* Shopkeeper header is above the stock region, including x == 0. */
    CHECK(!angdroid_menu_back_margin_hit(0, 0, 4, 1, 12));
    CHECK(!angdroid_menu_back_margin_hit(1, 0, 4, 1, 12));
    CHECK(!angdroid_menu_back_margin_hit(3, 0, 4, 1, 12));
    CHECK(angdroid_menu_back_margin_hit(4, 0, 4, 1, 12));
    CHECK(angdroid_menu_back_margin_hit(15, 0, 4, 1, 12));
    CHECK(!angdroid_menu_back_margin_hit(16, 0, 4, 1, 12));
    CHECK(!angdroid_menu_back_margin_hit(4, 1, 4, 1, 12));
    CHECK(!angdroid_menu_back_margin_hit(4, 0, 4, 0, 12));
}

static void extreme_native_coordinates(void)
{
    CHECK(!angdroid_menu_back_margin_hit(
        INT_MIN, 0, INT_MAX, 1, INT_MAX));
    CHECK(!angdroid_menu_back_margin_hit(
        INT_MAX, 0, INT_MIN, 1, INT_MAX));
    CHECK(!angdroid_menu_back_margin_hit(
        INT_MAX, -1, 4, 1, INT_MAX));
    CHECK(!angdroid_menu_back_margin_hit(
        INT_MAX, 0, 4, 1, 0));
    CHECK(angdroid_menu_back_margin_hit(
        INT_MAX, 0, INT_MAX, 1, 1));
    CHECK(angdroid_menu_back_margin_hit(
        INT_MAX, 0, INT_MAX - 1, 1, 2));
    CHECK(!angdroid_menu_back_margin_hit(
        INT_MAX, 0, INT_MAX - 1, 1, 1));
}

int main(void)
{
    menu_header_and_left_margin();
    extreme_native_coordinates();
    printf("PASS: Back margin ignores shop title while retaining list-row back (%lu assertions)\n",
        checks);
    return 0;
}
