#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/menu-visible-tags.h"
#include "../../app/src/main/cpp/angband/menu-key-hints.h"

int main(void)
{
    const char tags[] = "abcdefghij";
    const int filtered[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    char hints[16] = "";
    size_t used = 0;
    int i;

    /* Page two uses absolute row indices for lookup, but page-relative
     * selection letters, matching what the scrolling skin displays. */
    assert(angdroid_menu_visible_tag_first(60, 20, 1) == 20);
    assert(angdroid_menu_visible_tag_end(60, 20, 10, 1) == 30);
    assert(angdroid_menu_tag_at(tags, 20 - 20) == 'a');
    assert(angdroid_menu_tag_at(tags, 29 - 20) == 'j');
    for (i = 20; i < 30; ++i) {
        char key = angdroid_menu_tag_at(tags, i - 20);
        used = angdroid_menu_hint_append(hints, used, sizeof(hints), key);
        assert(key == (char)('a' + i - 20));
    }
    assert(strcmp(hints, tags) == 0);

    /* A last page shorter than the number of tag characters is bounded. */
    assert(angdroid_menu_visible_tag_first(60, 58, 1) == 58);
    assert(angdroid_menu_visible_tag_end(60, 58, 10, 1) == 60);

    /* Column menus show all rows and must not truncate to one page. */
    assert(angdroid_menu_visible_tag_first(60, 20, 0) == 0);
    assert(angdroid_menu_visible_tag_end(60, 20, 10, 0) == 60);

    /* A filter is indexed by visible row, not by the raw game item ID. */
    assert(angdroid_menu_hint_item_index(filtered, 2) == 7);
    assert(angdroid_menu_tag_at(tags, 2) == 'c');

    /* Empty, invalid and extreme inputs must not produce invalid ranges. */
    assert(angdroid_menu_visible_tag_first(60, 80, 1) == 60);
    assert(angdroid_menu_visible_tag_end(60, 80, 10, 1) == 60);
    assert(angdroid_menu_visible_tag_first(60, -2, 1) == 0);
    assert(angdroid_menu_visible_tag_end(60, -2, 10, 1) == 10);
    assert(angdroid_menu_visible_tag_first(-1, 0, 1) == 0);
    assert(angdroid_menu_visible_tag_end(-1, 0, 10, 1) == 0);
    assert(angdroid_menu_visible_tag_end(60, 5, 0, 1) == 5);
    assert(angdroid_menu_visible_tag_end(60, INT_MAX, INT_MAX, 1) == 60);
    assert(angdroid_menu_tag_at(tags, -1) == '\0');
    assert(angdroid_menu_tag_at(tags, 10) == '\0');
    assert(angdroid_menu_tag_at(NULL, 0) == '\0');

    puts("PASS: relative menu tags use visible page, filtered row and safe bounds");
    return 0;
}
