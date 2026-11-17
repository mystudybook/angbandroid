/* Host C99 test for the real shared statusline bounds helper. */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/statusline-bounds.h"
static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)
int main(void)
{
    int width, column;
    size_t advance;
    for (width = 0; width <= 64; ++width) {
        for (column = -4; column <= width + 4; ++column) {
            for (advance = 0; advance <= 130; ++advance) {
                int next = angdroid_statusline_next_col(column, advance, width);
                int clamped = column < 0 ? 0 : column;
                int expected = clamped >= width ? width :
                    advance >= (size_t)(width - clamped) ? width :
                    clamped + (int)advance;
                int room = width > 0 && column >= 0 && column < width &&
                    advance < (size_t)(width - column);
                CHECK(next == expected);
                CHECK(next >= 0 && next <= width);
                CHECK(angdroid_statusline_has_room(column, advance, width) == room);
            }
        }
    }
    CHECK(angdroid_statusline_next_col(INT_MAX - 2, 2, INT_MAX) == INT_MAX);
    CHECK(angdroid_statusline_next_col(INT_MAX - 3, 2, INT_MAX) == INT_MAX - 1);
    CHECK(angdroid_statusline_next_col(1, SIZE_MAX, INT_MAX) == INT_MAX);
    CHECK(angdroid_statusline_next_col(INT_MIN, SIZE_MAX, INT_MAX) == INT_MAX);
    CHECK(!angdroid_statusline_has_room(INT_MAX, 0, INT_MAX));
    CHECK(angdroid_statusline_has_room(INT_MAX - 1, 0, INT_MAX));
    CHECK(!angdroid_statusline_has_room(INT_MAX - 1, 1, INT_MAX));
    CHECK(!angdroid_statusline_has_room(-1, 0, 80));
    CHECK(!angdroid_statusline_has_room(12, 0, 12));
    CHECK(!angdroid_statusline_has_room(0, 0, 0));
    printf("PASS: statusline terminal bounds (%lu checks)\n", checks);
    return 0;
}
