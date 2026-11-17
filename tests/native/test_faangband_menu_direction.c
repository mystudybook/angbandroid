/* FAangband: directional navigation must terminate on disabled rows.
 * Verify shared production helpers used by single/column menu skins.
 */
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-valid-cursor.h"
#include "../../app/src/main/cpp/angband/menu-visible-tags.h"
#include "../../app/src/main/cpp/angband/menu-compact-layout.h"
#include "../../app/src/main/cpp/faangband/ui-menu-viewport.h"

static unsigned long checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

struct rows {
    int count;
    unsigned long long valid_bits;
};

static int row_is_valid(void *ptr, int row)
{
    const struct rows *list = (const struct rows *)ptr;
    CHECK(row >= 0 && row < list->count);
    return ((list->valid_bits >> row) & 1ULL) != 0;
}

static int reference_next(int cursor, int count, int direction,
                          unsigned long long bits)
{
    int i, index;
    if (count <= 0) return -1;
    index = cursor % count;
    if (index < 0) index += count;
    for (i = 0; i < count; ++i) {
        if (bits & (1ULL << index)) return index;
        if (direction < 0)
            index = index == 0 ? count - 1 : index - 1;
        else
            index = index == count - 1 ? 0 : index + 1;
    }
    return -1;
}

static void navigation_all_small_menus(void)
{
    int count, cursor, dir, mask;
    for (count = 0; count <= 12; ++count)
        for (mask = 0; mask < 2048; ++mask)
            for (cursor = -2; cursor <= count + 2; ++cursor)
                for (dir = -1; dir <= 1; ++dir) {
                    struct rows r = {count, (unsigned)mask};
                    int actual = angdroid_menu_find_valid_cursor(
                        cursor, count, dir, &r, row_is_valid);
                    int expected = reference_next(
                        cursor, count, dir, r.valid_bits);
                    CHECK(actual == expected);
                    if (actual >= 0) CHECK(actual < count);
                }
    CHECK(angdroid_menu_find_valid_cursor(
        INT_MIN, 12, -1, NULL, NULL) == -1);
    {
        struct rows r = {6, 0x24}; /* only 2 and 5 */
        CHECK(angdroid_menu_find_valid_cursor(
            3, 6, 1, &r, row_is_valid) == 5);
        CHECK(angdroid_menu_find_valid_cursor(
            3, 6, -1, &r, row_is_valid) == 2);
        r.valid_bits = 0;
        CHECK(angdroid_menu_find_valid_cursor(
            3, 6, -1, &r, row_is_valid) == -1);
    }
}

/* Hidden entries (value 2) are still available by explicit tags,
 * but directional navigation must land only on visible entries (value 1). */
struct visible_rows {
    int count;
    const unsigned char *state;
};

static int only_visible(void *context, int row)
{
    const struct visible_rows *r = (const struct visible_rows *)context;
    CHECK(row >= 0 && row < r->count);
    return r->state[row] == 1;
}

static void hidden_entries_tag_only(void)
{
    const unsigned char state[] = {2, 0, 1, 2, 0, 1};
    const unsigned char none[] = {2, 2, 2, 2};
    struct visible_rows rows = {6, state};
    struct visible_rows hidden = {4, none};

    CHECK(angdroid_menu_find_valid_cursor(
        0, 6, 1, &rows, only_visible) == 2);
    CHECK(angdroid_menu_find_valid_cursor(
        3, 6, 1, &rows, only_visible) == 5);
    CHECK(angdroid_menu_find_valid_cursor(
        3, 6, -1, &rows, only_visible) == 2);
    CHECK(angdroid_menu_find_valid_cursor(
        0, 4, 1, &hidden, only_visible) == -1);
}

static void page_navigation(void)
{
    int count, rows, cursor;
    for (count = 0; count <= 40; ++count)
        for (rows = -1; rows <= 40; ++rows)
            for (cursor = -2; cursor <= 42; ++cursor) {
                int actual = angdroid_menu_page_next_cursor(
                    cursor, count, rows);
                int expected;
                if (count <= 0 || rows <= 0 || rows >= count)
                    expected = -1;
                else if (cursor < 0 || cursor >= count ||
                         cursor >= count - rows)
                    expected = 0;
                else
                    expected = cursor + rows;
                CHECK(actual == expected);
            }
    CHECK(angdroid_menu_page_next_cursor(4, 9, 4) == 8);
    CHECK(angdroid_menu_page_next_cursor(8, 9, 4) == 0);
    CHECK(angdroid_menu_page_next_cursor(
        INT_MAX - 2, INT_MAX, 1) == INT_MAX - 1);
}

static void relative_tag_modes(void)
{
    int count, rows, top, width;
    for (count = 0; count <= 32; ++count)
        for (rows = 0; rows <= 8; ++rows)
            for (top = -2; top <= 34; ++top)
                for (width = 1; width <= 80; width += 5) {
                    int compact = angdroid_menu_use_compact_columns(
                        count, rows, width, 8);
                    int paged = compact;
                    int first = angdroid_menu_visible_tag_first(
                        count, top, paged);
                    int end = angdroid_menu_visible_tag_end(
                        count, top, rows, paged);
                    int i;
                    CHECK(first >= 0 && first <= count);
                    CHECK(end >= first && end <= count);
                    if (!paged) CHECK(first == 0 && end == count);
                    if (paged) CHECK(end - first <= rows);
                    for (i = first; i < end; ++i) {
                        int label_index = paged ? i - first : i;
                        CHECK(label_index >= 0 && label_index < count);
                        CHECK((paged ? first + label_index : label_index) == i);
                    }
                }
    /* A full-width columns menu shows all column tags; a portrait menu
     * has only the current page, and matches the visible relative letters. */
    CHECK(!angdroid_menu_use_compact_columns(12, 4, 80, 8));
    CHECK(angdroid_menu_use_compact_columns(12, 4, 10, 8));
    CHECK(angdroid_menu_visible_tag_first(12, 4, 1) == 4);
    CHECK(angdroid_menu_visible_tag_end(12, 4, 4, 1) == 8);
    CHECK(angdroid_menu_visible_tag_end(12, 4, 4, 0) == 12);
}

int main(void)
{
    navigation_all_small_menus();
    hidden_entries_tag_only();
    page_navigation();
    relative_tag_modes();
    printf("PASS: FA direction, last-page selection, column shortcuts (%lu checks)\n",
        checks);
    return 0;
}
