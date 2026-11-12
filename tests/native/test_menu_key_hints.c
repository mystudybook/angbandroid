#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/menu-key-hints.h"

/* Fixed command keys are guaranteed room even after hundreds of tags.
 * Duplicate key labels must not consume a second slot before de-duplication. */
static void test_unique_reserved_hints(void)
{
    size_t capacity;
    for (capacity = 1; capacity <= 256; ++capacity) {
        char storage[260];
        char *buf = storage + 1;
        size_t used = 0, limit;
        unsigned char seen[256] = {0};
        size_t i;

        memset(storage, '#', sizeof(storage));
        buf[0] = '\0';
        used = angdroid_menu_hint_append_keys_unique(
            buf, used, capacity, "001");
        limit = angdroid_menu_hint_tag_capacity(
            capacity, used, "p?z", "I");

        for (i = 0; i < 700; ++i)
            used = angdroid_menu_hint_append_unique(
                buf, used, limit, (char)('a' + i % 26));
        used = angdroid_menu_hint_append_keys_unique(
            buf, used, capacity, "p?z");
        used = angdroid_menu_hint_append_keys_unique(
            buf, used, capacity, "Ipz");

        assert(storage[0] == '#');
        assert(storage[capacity + 1] == '#');
        assert(used == strlen(buf));
        assert(used < capacity);
        for (i = 0; i < used; ++i) {
            unsigned char key = (unsigned char)buf[i];
            assert(!seen[key]);
            seen[key] = 1;
        }
        if (capacity >= 35) {
            assert(strchr(buf, 'p') != NULL);
            assert(strchr(buf, '?') != NULL);
            assert(strchr(buf, 'z') != NULL);
            assert(strchr(buf, 'I') != NULL);
        }
    }
    {
        char buf[4] = "";
        size_t n = 0;
        n = angdroid_menu_hint_append_unique(buf, n, sizeof(buf), 'x');
        n = angdroid_menu_hint_append_unique(buf, n, sizeof(buf), 'x');
        n = angdroid_menu_hint_append_unique(buf, n, sizeof(buf), '\0');
        assert(n == 1 && strcmp(buf, "x") == 0);
        assert(angdroid_menu_hint_append_unique(NULL, n, 4, 'y') == n);
        assert(angdroid_menu_hint_append_unique(buf, SIZE_MAX, 4, 'y') == SIZE_MAX);
        assert(angdroid_menu_hint_append_keys_unique(
            buf, n, sizeof(buf), NULL) == n);
        assert(angdroid_menu_hint_tag_capacity(0, 0, "x", "y") == 0);
        assert(angdroid_menu_hint_tag_capacity(4, 4, "x", "y") == 0);
    }
}

int main(void)
{
    const int filtered[] = { 4, 1, 3 };
    char hints[8] = "";
    char minimal[1] = "";
    size_t length = 0;
    int i;

    assert(angdroid_menu_hint_item_index(NULL, 2) == 2);
    assert(angdroid_menu_hint_item_index(filtered, 0) == 4);
    assert(angdroid_menu_hint_item_index(filtered, 1) == 1);
    assert(angdroid_menu_hint_item_index(filtered, 2) == 3);

    length = angdroid_menu_hint_append(hints, length, sizeof(hints), 'a');
    length = angdroid_menu_hint_append(hints, length, sizeof(hints), 0);
    assert(length == 1 && strcmp(hints, "a") == 0);
    for (i = 0; i < 1000; ++i)
        length = angdroid_menu_hint_append(hints, length, sizeof(hints), 'x');
    assert(length == sizeof(hints) - 1);
    assert(strcmp(hints, "axxxxxx") == 0);
    assert(angdroid_menu_hint_append(minimal, 0, sizeof(minimal), 'z') == 0);
    assert(minimal[0] == '\0');
    assert(angdroid_menu_hint_append(NULL, 0, sizeof(hints), 'z') == 0);
    assert(angdroid_menu_hint_append(hints, length, 0, 'z') == length);

    test_unique_reserved_hints();
    puts("PASS: filtered menu shortcuts, fixed-key reservation and unique hints");
    return 0;
}
