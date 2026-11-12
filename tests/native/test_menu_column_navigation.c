/* Real column-major motion must stay in visible rows, even when the
 * last column is short or some rows are hidden/disabled by a filter.
 */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-column-navigation.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

struct visible {
    int count;
    unsigned long mask;
};

static int visible_row(void *context, int index)
{
    struct visible *v = (struct visible *)context;
    CHECK(index >= 0 && index < v->count);
    return (v->mask & (1UL << index)) != 0;
}

static int all_valid(void *context, int index)
{
    int count = *(int *)context;
    CHECK(index >= 0 && index < count);
    return 1;
}

/* Independent reference: scan the same visual row through columns, or
 * scan within one column; never visit an index beyond count. */
static int expected_move(int cursor, int count, int rows, int dx, int dy,
    unsigned long mask)
{
    int i, column, row, cols;
    if (count <= 0 || rows <= 0 || (dx && dy) || (!dx && !dy))
        return -1;
    if (cursor < 0 || cursor >= count) {
        if (dx < 0 || dy < 0) {
            for (i = count - 1; i >= 0; --i)
                if (mask & (1UL << i)) return i;
        } else {
            for (i = 0; i < count; ++i)
                if (mask & (1UL << i)) return i;
        }
        return -1;
    }
    column = cursor / rows;
    row = cursor % rows;
    cols = 1 + (count - 1) / rows;
    if (dx) {
        for (i = 1; i < cols; ++i) {
            int next_col = (column + (dx < 0 ? cols - i : i)) % cols;
            int first = next_col * rows;
            int height = count - first < rows ? count - first : rows;
            int target = row < height ? row : height - 1;
            int best = -1, best_distance = height + 1;
            int j;

            /* Independent nearest-valid-row oracle; lower row breaks a
             * tie. A hidden target must not skip an otherwise valid
             * item in the same column. */
            for (j = 0; j < height; ++j) {
                int distance = j < target ? target - j : j - target;
                if ((mask & (1UL << (first + j))) &&
                    distance < best_distance) {
                    best = first + j;
                    best_distance = distance;
                }
            }
            if (best >= 0) return best;
        }
        return -1;
    }
    {
        int first = column * rows;
        int length = count - first < rows ? count - first : rows;
        for (i = 1; i < length; ++i) {
            int next_row = (row + (dy < 0 ? length - i : i)) % length;
            int next = first + next_row;
            if (mask & (1UL << next))
                return next;
        }
    }
    return -1;
}

static void small_grids_all_visibility_masks(void)
{
    int count, rows, cursor, direction, mask;
    const int dx[] = {1, -1, 0, 0};
    const int dy[] = {0, 0, 1, -1};
    for (count = 0; count <= 9; ++count)
        for (rows = 0; rows <= 8; ++rows)
            for (mask = 0; mask < (1 << count); ++mask)
                for (cursor = -1; cursor <= count; ++cursor)
                    for (direction = 0; direction < 4; ++direction) {
                        struct visible v = {count, (unsigned long)mask};
                        int actual = angdroid_menu_column_next_valid(
                            cursor, count, rows, dx[direction], dy[direction],
                            &v, visible_row);
                        int expected = expected_move(cursor, count, rows,
                            dx[direction], dy[direction], (unsigned long)mask);
                        CHECK(actual == expected);
                        CHECK(actual < count);
                        CHECK(actual >= -1);
                        if (actual >= 0)
                            CHECK(mask & (1 << actual));
                    }
}

static void short_final_column_and_hidden_rows(void)
{
    struct visible v = {10, 0x3ffUL};
    CHECK(angdroid_menu_column_next_valid(6, 10, 4, 1, 0,
        &v, visible_row) == 9);
    CHECK(angdroid_menu_column_next_valid(9, 10, 4, 1, 0,
        &v, visible_row) == 1);
    CHECK(angdroid_menu_column_next_valid(3, 10, 4, -1, 0,
        &v, visible_row) == 9);
    CHECK(angdroid_menu_column_next_valid(8, 10, 4, 0, -1,
        &v, visible_row) == 9);
    CHECK(angdroid_menu_column_next_valid(9, 10, 4, 0, 1,
        &v, visible_row) == 8);
    v.mask = (1UL << 2) | (1UL << 8);
    CHECK(angdroid_menu_column_next_valid(2, 10, 4, 1, 0,
        &v, visible_row) == 8);
    CHECK(angdroid_menu_column_next_valid(8, 10, 4, -1, 0,
        &v, visible_row) == 2);
    v.mask = (1UL << 2);
    CHECK(angdroid_menu_column_next_valid(2, 10, 4, 1, 0,
        &v, visible_row) == -1);
    CHECK(angdroid_menu_column_next_valid(2, 10, 4, 0, 1,
        &v, visible_row) == -1);
    CHECK(angdroid_menu_column_next_valid(2, 10, 0, 1, 0,
        &v, visible_row) == -1);
    CHECK(angdroid_menu_column_next_valid(2, 10, 4, 1, 1,
        &v, visible_row) == -1);
}

static void extreme_indices_and_empty_cursor(void)
{
    int count = INT_MAX;
    CHECK(angdroid_menu_column_next_valid(INT_MAX - 2, count,
        INT_MAX - 1, 1, 0, &count, all_valid) == INT_MAX - 1);
    CHECK(angdroid_menu_column_next_valid(INT_MAX - 1, count,
        INT_MAX - 1, -1, 0, &count, all_valid) == 0);
    CHECK(angdroid_menu_column_next_valid(INT_MAX - 1, count,
        INT_MAX, 0, 1, &count, all_valid) == 0);
    CHECK(angdroid_menu_column_next_valid(0, count,
        1, -1, 0, &count, all_valid) == INT_MAX - 1);
    CHECK(angdroid_menu_column_next_valid(INT_MAX - 1, count,
        1, 1, 0, &count, all_valid) == 0);
    CHECK(angdroid_menu_column_next_valid(INT_MIN, count,
        1, 0, 1, &count, all_valid) == 0);
    CHECK(angdroid_menu_column_next_valid(INT_MAX, count,
        1, 0, -1, &count, all_valid) == INT_MAX - 1);
}

int main(void)
{
    small_grids_all_visibility_masks();
    short_final_column_and_hidden_rows();
    extreme_indices_and_empty_cursor();
    printf("PASS: column-major direction/disabled rows/short final column (%lu checks)\n",
        checks);
    return 0;
}
