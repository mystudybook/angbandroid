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

    printf("PASS: compact player selective clearing (%lu checks)\n", checks);
    return 0;
}
