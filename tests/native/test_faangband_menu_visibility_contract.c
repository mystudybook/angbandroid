/* Strict C99: filtered menu visibility, on-screen tag, key choice, and
 * Android keyboard hint must agree. Calls production helper headers.
 */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include "../../app/src/main/cpp/faangband/ui-menu-key-guard.h"
#include "../../app/src/main/cpp/angband/menu-valid-cursor.h"
#include "../../app/src/main/cpp/angband/menu-visible-tags.h"
#include "../../app/src/main/cpp/angband/menu-compact-layout.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

struct menu_rows {
    int count;
    int filtered_count;
    const int *filter;
    int states[6];  /* 0 disabled, 1 visible, 2 hidden */
};

static int visible_row(void *context, int row)
{
    struct menu_rows *m = (struct menu_rows *)context;
    int oid = angdroid_fa_menu_source_index(
        m->filter, row, m->filtered_count, m->count);
    return oid >= 0 && m->states[oid] == 1;
}

static void test_filtered_hidden_labels(void)
{
    const int mapping[] = {5, 1, 3, 2, 0, 4};
    const char *labels = "abcdef";
    struct menu_rows m = {6, 6, mapping, {0, 0, 0, 0, 0, 0}};
    int mask;
    for (mask = 0; mask < 729; ++mask) {
        int code = mask, i, top, width;
        int visible = 0;
        for (i = 0; i < 6; ++i) {
            m.states[i] = code % 3;
            code /= 3;
        }
        for (i = 0; i < 6; ++i) {
            int oid = angdroid_fa_menu_source_index(
                mapping, i, 6, m.count);
            CHECK(oid >= 0 && oid < m.count);
            CHECK(visible_row(&m, i) == (m.states[oid] == 1));
            if (m.states[oid] == 1) ++visible;
        }
        for (width = 8; width <= 48; width += 5) {
            /* In a full columns view every relative tag is addressed by
             * absolute view row; portrait fallback uses the page offset. */
            int paged = angdroid_menu_use_compact_columns(6, 2, width, 8);
            for (top = 0; top < 6; ++top) {
                int first = angdroid_menu_visible_tag_first(6, top, paged);
                int end = angdroid_menu_visible_tag_end(6, top, 2, paged);
                char displayed[8] = "";
                size_t used = 0;
                int j;
                for (j = first; j < end; ++j) {
                    int label_index = paged ? j - first : j;
                    char letter = angdroid_fa_menu_selection_key(
                        labels, label_index);
                    CHECK(letter != 0);
                    if (!visible_row(&m, j)) {
                        /* Hidden/disabled entries have no shown tags and
                         * must not be emitted as relative keyboard hints. */
                        continue;
                    }
                    CHECK(angdroid_fa_menu_add_key(
                        displayed, sizeof(displayed), &used, letter));
                    CHECK(visible_row(&m, j));
                }
                CHECK(used <= (size_t)(end - first));
                if (!visible) CHECK(used == 0);
                /* Each advertised relative tag must uniquely identify a
                 * genuinely visible row in the proper current page. */
                for (j = first; j < end; ++j) {
                    char c = angdroid_fa_menu_selection_key(
                        labels, paged ? j - first : j);
                    size_t k;
                    int found = 0;
                    for (k = 0; k < used; ++k)
                        if (displayed[k] == c) found = 1;
                    CHECK(found == !!visible_row(&m, j));
                }
            }
        }
        {
            int first = angdroid_menu_find_valid_cursor(
                0, 6, 1, &m, visible_row);
            if (visible) {
                CHECK(first >= 0 && first < 6);
                CHECK(visible_row(&m, first));
            } else CHECK(first == -1);
        }
    }
}

static void test_invalid_filters_and_bounds(void)
{
    const int filter[] = {-1, 7, 1, 3, 2, 0};
    struct menu_rows m = {5, 6, filter, {1, 2, 1, 0, 1, 0}};
    int i;
    for (i = -2; i <= 8; ++i) {
        int oid = angdroid_fa_menu_source_index(
            filter, i, m.filtered_count, m.count);
        if (i < 0 || i >= 6 || i == 0 || i == 1)
            CHECK(oid == -1);
        else CHECK(oid >= 0 && oid < 5);
    }
    CHECK(!visible_row(&m, 0));
    CHECK(!visible_row(&m, 1));
    CHECK(!visible_row(&m, 2)); /* mapped to hidden state[1] */
    CHECK(!visible_row(&m, 3)); /* mapped to disabled state[3] */
    CHECK(visible_row(&m, 4));
    CHECK(visible_row(&m, 5));
    CHECK(angdroid_menu_find_valid_cursor(
        0, 6, 1, &m, visible_row) == 4);
    CHECK(angdroid_menu_find_valid_cursor(
        5, 6, -1, &m, visible_row) == 5);
    CHECK(angdroid_menu_find_valid_cursor(
        INT_MIN, 6, 1, &m, visible_row) >= 0);
    CHECK(angdroid_fa_menu_selection_key(NULL, INT_MAX) == 0);
    CHECK(angdroid_menu_visible_tag_end(6, INT_MAX, 3, 1) == 6);
}

int main(void)
{
    test_filtered_hidden_labels();
    test_invalid_filters_and_bounds();
    printf("PASS: FA filtered/hidden visible tags and Android hints (%lu checks)\n",
        checks);
    return 0;
}
