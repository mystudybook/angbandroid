/* Host C99 regression: modal Android shortcut hints must be replaced,
 * never appended to a previous page or a lingering outer prompt.
 */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/curses/soft-kbd-replace.h"

static unsigned long checks;
#define CHECK(condition) do { assert(condition); ++checks; } while (0)

static void replaces_previous_hints(void)
{
    char keys[32] = "old-menu-hints";
    size_t n = angdroid_soft_kbd_replace_buffer(keys, sizeof(keys), "abcabc");
    CHECK(n == 3);
    CHECK(strcmp(keys, "abc") == 0);
    CHECK(angdroid_soft_kbd_replace_buffer(
        keys, sizeof(keys), "xzy") == 3);
    CHECK(strcmp(keys, "xzy") == 0);
    CHECK(angdroid_soft_kbd_replace_buffer(
        keys, sizeof(keys), "") == 0);
    CHECK(keys[0] == '\0');
    CHECK(angdroid_soft_kbd_replace_buffer(
        keys, sizeof(keys), NULL) == 0);
    CHECK(keys[0] == '\0');
}

static void bounded_and_deduplicated(void)
{
    char storage[10];
    size_t capacity;
    for (capacity = 0; capacity <= 8; ++capacity) {
        size_t n;
        memset(storage, '!', sizeof(storage));
        n = angdroid_soft_kbd_replace_buffer(
            storage, capacity, "abcabcdeffgh");
        CHECK(n == (capacity ? (capacity - 1 < 8 ?
            capacity - 1 : 8) : 0));
        if (capacity > 0) {
            CHECK(storage[n] == '\0');
            CHECK(strncmp(storage, "abcdefgh", n) == 0);
        }
        CHECK(storage[8] == '!');
        CHECK(storage[9] == '!');
    }
    CHECK(angdroid_soft_kbd_replace_buffer(
        NULL, 12, "abc") == 0);
}

static void transitions_and_full_capacity(void)
{
    char keys[256];
    char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    int page;
    size_t i;

    memset(keys, 'z', sizeof(keys));
    for (page = 0; page < 40; ++page) {
        char next[24];
        size_t n;
        for (i = 0; i < 20; ++i)
            next[i] = alphabet[(i + (size_t)page) % 26];
        next[20] = '\0';
        n = angdroid_soft_kbd_replace_buffer(
            keys, sizeof(keys), next);
        CHECK(n == 20);
        CHECK(strcmp(keys, next) == 0);
        CHECK(keys[255] == 'z');
        n = angdroid_soft_kbd_replace_buffer(
            keys, sizeof(keys), "");
        CHECK(n == 0);
        CHECK(keys[0] == '\0');
    }
    {
        char one[1] = {'X'};
        CHECK(angdroid_soft_kbd_replace_buffer(
            one, sizeof(one), "abc") == 0);
        CHECK(one[0] == '\0');
    }
}

int main(void)
{
    replaces_previous_hints();
    bounded_and_deduplicated();
    transitions_and_full_capacity();
    printf("PASS: native soft key replacement, empty-page clearing and rotation (%lu checks)\n",
        checks);
    return 0;
}
