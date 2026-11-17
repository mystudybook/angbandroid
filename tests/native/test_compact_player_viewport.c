/* Host C99 regression for rows owned by the compact player window.
 * Invalid/short windows must never be accessed; taller windows retain
 * their other rows when player status events trigger a refresh.
 */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/compact-player-viewport.h"

static unsigned long checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static void check_redraw(int stat_count, int height)
{
    int visible = angdroid_compact_player_visible_rows(stat_count, height);
    int expect = (height <= 0 || stat_count < 0) ? 0 :
        stat_count >= height - ANGDROID_COMPACT_PLAYER_FIXED_ROWS ?
        height : stat_count + ANGDROID_COMPACT_PLAYER_FIXED_ROWS;
    CHECK(visible == expect);
    CHECK(visible >= 0);
    if (height > 0) CHECK(visible <= height);
    if (height > 0 && stat_count >= 0) {
        int row;
        /* The fast path erases only owned rows, never the window tail. */
        for (row = 0; row < height; ++row) {
            int erased = row < visible;
            CHECK(erased == (row < expect));
        }
    }
}

int main(void)
{
    int height, stats;

    CHECK(ANGDROID_COMPACT_PLAYER_FIXED_ROWS == 12);
    for (height = -2; height <= 128; ++height)
        for (stats = -2; stats <= 24; ++stats)
            check_redraw(stats, height);

    CHECK(angdroid_compact_player_visible_rows(5, 1) == 1);
    CHECK(angdroid_compact_player_visible_rows(5, 16) == 16);
    CHECK(angdroid_compact_player_visible_rows(5, 17) == 17);
    CHECK(angdroid_compact_player_visible_rows(5, 18) == 17);
    CHECK(angdroid_compact_player_visible_rows(5, 60) == 17);
    CHECK(angdroid_compact_player_visible_rows(5, INT_MAX) == 17);
    CHECK(angdroid_compact_player_visible_rows(INT_MAX, INT_MAX) == INT_MAX);
    CHECK(angdroid_compact_player_visible_rows(-1, 100) == 0);
    CHECK(angdroid_compact_player_visible_rows(0, INT_MAX) == 12);
    CHECK(angdroid_compact_player_visible_rows(INT_MAX, 1) == 1);

    /* One-line updates are safe only after an isolated compact window
     * has received a full draw and has kept the same term and dimensions. */
    {
        int screen_a, screen_b;
        struct angdroid_compact_player_redraw_state state = {
            &screen_a, 80, 25, 0
        };
        int w, h;
        CHECK(!angdroid_compact_player_can_redraw_one(
            &state, &screen_a, 80, 25, 1));
        state.ready = 1;
        CHECK(angdroid_compact_player_can_redraw_one(
            &state, &screen_a, 80, 25, 1));
        CHECK(!angdroid_compact_player_can_redraw_one(
            &state, &screen_a, 80, 25, 0)); /* Mixed window flags */
        CHECK(!angdroid_compact_player_can_redraw_one(
            &state, &screen_b, 80, 25, 1)); /* Replaced terminal */
        CHECK(!angdroid_compact_player_can_redraw_one(
            &state, NULL, 80, 25, 1));
        CHECK(!angdroid_compact_player_can_redraw_one(
            NULL, &screen_a, 80, 25, 1));
        for (w = -1; w <= 100; ++w) {
            for (h = -1; h <= 60; ++h) {
                int expected = w == 80 && h == 25;
                CHECK(!!angdroid_compact_player_can_redraw_one(
                    &state, &screen_a, w, h, 1) == expected);
            }
        }
        state.ready = 0; /* Changed PW_PLAYER_2 or flag combination */
        CHECK(!angdroid_compact_player_can_redraw_one(
            &state, &screen_a, 80, 25, 1));
    }
    /* Returning to the same terminal at the same dimensions must not
     * carry a previous character's ready state into the next world. */
    {
        int screen;
        struct angdroid_compact_player_redraw_state state = {
            &screen, 80, 25, 1
        };
        CHECK(angdroid_compact_player_can_redraw_one(
            &state, &screen, 80, 25, 1));
        angdroid_compact_player_invalidate(&state);
        CHECK(state.terminal == NULL);
        CHECK(state.width == 0);
        CHECK(state.height == 0);
        CHECK(state.ready == 0);
        CHECK(!angdroid_compact_player_can_redraw_one(
            &state, &screen, 80, 25, 1));
        /* It is safe to invalidate twice or without a state. */
        angdroid_compact_player_invalidate(&state);
        angdroid_compact_player_invalidate(NULL);
        CHECK(state.ready == 0);
        /* A full redraw may then seed the newly loaded world. */
        state.terminal = &screen;
        state.width = 80;
        state.height = 25;
        state.ready = 1;
        CHECK(angdroid_compact_player_can_redraw_one(
            &state, &screen, 80, 25, 1));
    }
    CHECK(ANGDROID_COMPACT_ROW_TITLE == 2);
    CHECK(ANGDROID_COMPACT_ROW_EXP == 4);
    CHECK(ANGDROID_COMPACT_ROW_GOLD == 5);
    CHECK(ANGDROID_COMPACT_ROW_EQUIPMENT == 6);
    CHECK(angdroid_compact_row_hp(5) == 14);
    CHECK(angdroid_compact_row_sp(5) == 15);
    CHECK(angdroid_compact_row_monster(5) == 16);
    CHECK(angdroid_compact_row_hp(-1) == -1);
    CHECK(angdroid_compact_row_sp(-1) == -1);
    CHECK(angdroid_compact_row_monster(-1) == -1);
    CHECK(angdroid_compact_row_hp(INT_MAX) == -1);
    CHECK(angdroid_compact_row_sp(INT_MAX) == -1);
    CHECK(angdroid_compact_row_monster(INT_MAX) == -1);

    printf("PASS: compact player selective clearing (%lu checks)\n", checks);
    return 0;
}
