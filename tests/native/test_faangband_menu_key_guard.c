/* C99: FA soft keyboard hints and filtered menu indices.
 * Include the actual Native helper used by menu_select(), key rendering and
 * Android's soft keyboard rather than copying the application algorithm.
 */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/ui-menu-key-guard.h"

static unsigned long checks;
#define CHECK(c) do { assert(c); ++checks; } while (0)

static void filtered_indices(void)
{
    const int filter[] = {4, 1, 7, 0, -1, 9};
    int pos, count;
    for (count = -2; count <= 12; ++count) {
        for (pos = -2; pos <= 12; ++pos) {
            const int got = angdroid_fa_menu_source_index(
                filter, pos, 6, count);
            const int plain = angdroid_fa_menu_source_index(
                NULL, pos, 6, count);
            const int expected = pos >= 0 && pos < 6 &&
                count > 0 && filter[pos] >= 0 && filter[pos] < count ?
                filter[pos] : -1;
            const int expected_plain = pos >= 0 && pos < 6 &&
                pos < count ? pos : -1;
            CHECK(got == expected);
            CHECK(plain == expected_plain);
        }
    }
    CHECK(angdroid_fa_menu_source_index(filter, 2, 6, 8) == 7);
    CHECK(angdroid_fa_menu_source_index(filter, 3, 6, 8) == 0);
    CHECK(angdroid_fa_menu_source_index(filter, 4, 6, 8) == -1);
    CHECK(angdroid_fa_menu_source_index(filter, 5, 6, 8) == -1);
    CHECK(angdroid_fa_menu_source_index(NULL, INT_MAX, 6, INT_MAX) == -1);
}

static void visible_selection_keys(void)
{
    const char *keys = "abcdef";
    int i;
    for (i = -2; i <= 128; ++i) {
        char expected = (i >= 0 && i < 6) ? keys[i] : 0;
        CHECK(angdroid_fa_menu_selection_key(keys, i) == expected);
    }
    CHECK(angdroid_fa_menu_selection_key(NULL, 1) == 0);
    CHECK(angdroid_fa_menu_selection_key(keys, INT_MAX) == 0);
}

static void bounded_keyboard_text(void)
{
    size_t cap;
    for (cap = 0; cap <= 260; ++cap) {
        char storage[264];
        char *out = storage + 1;
        size_t used = 0;
        unsigned i;
        memset(storage, '$', sizeof(storage));
        if (cap) out[0] = '\0';
        for (i = 0; i < 3000; ++i)
            (void)angdroid_fa_menu_add_key(
                out, cap, &used, (char)('a' + i % 26));
        CHECK(storage[0] == '$');
        CHECK(storage[cap + 1] == '$');
        CHECK(storage[263] == '$');
        if (!cap) {
            CHECK(out[0] == '$');
            CHECK(used == 0);
        } else {
            CHECK(used == cap - 1);
            CHECK(strlen(out) == used);
            CHECK(out[used] == '\0');
        }
    }
    {
        char buf[] = "already";
        size_t used = strlen(buf);
        CHECK(!angdroid_fa_menu_add_key(buf, sizeof(buf),
            &used, 'x'));
        CHECK(used == 7 && strcmp(buf, "already") == 0);
        CHECK(!angdroid_fa_menu_add_key(buf, sizeof(buf),
            &used, '\0'));
    }
    {
        char buf[8] = "";
        size_t used = 0;
        CHECK(!angdroid_fa_menu_add_key(NULL, 8, &used, 'a'));
        CHECK(!angdroid_fa_menu_add_key(buf, 8, NULL, 'a'));
        CHECK(!angdroid_fa_menu_add_key(buf, 0, &used, 'a'));
        CHECK(angdroid_fa_menu_add_key(buf, 8, &used, 'a'));
        CHECK(angdroid_fa_menu_add_key(buf, 8, &used, 'b'));
        CHECK(strcmp(buf, "ab") == 0 && used == 2);
    }
}

static void page_relative_tags(void)
{
    int count, top, rows;
    for (count = -1; count <= 16; ++count)
        for (top = -1; top <= 18; ++top)
            for (rows = -1; rows <= 18; ++rows) {
                int expected = 0;
                if (count > 0 && top >= 0 && top < count && rows > 0)
                    expected = rows < count - top ? rows : count - top;
                CHECK(angdroid_fa_menu_page_rows(
                    count, top, rows) == expected);
            }
    CHECK(angdroid_fa_menu_page_rows(INT_MAX, INT_MAX - 1,
        INT_MAX) == 1);
    CHECK(angdroid_fa_menu_page_rows(INT_MIN, 0, INT_MAX) == 0);
}

int main(void)
{
    filtered_indices();
    visible_selection_keys();
    bounded_keyboard_text();
    page_relative_tags();
    printf("PASS: FA filtered menu source IDs, soft keyboard labels and visible page tags (%lu checks)\n",
        checks);
    return 0;
}
