/* Verify object list names never split Chinese UTF-8 or lose name text. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../../app/src/main/cpp/angband/object-list-name-display.h"

static unsigned assertions;
#define CHECK(e) do { assert(e); ++assertions; } while (0)

static void check(const char *name, int split, const char *expected)
{
    char out[256];
    size_t n = angdroid_object_list_name_copy(out, sizeof(out), name, split);
    CHECK(n == strlen(expected));
    CHECK(strcmp(out, expected) == 0);
}

static void check_examples(void)
{
    const char broken[] = { (char)0xE4, 'X', 0 };
    char out[32];
    check("a Potion of Healing", 1, "  a Potion of Healing");
    check("an Elixir", 1, " an Elixir");
    check("2 Potions", 1, "  2 Potions");
    check("99 Potions", 1, " 99 Potions");
    check("999 Potions", 1, "999 Potions");
    check("[U] Dragon", 1, "[U] Dragon");
    check("a   Potion", 1, "  a Potion");
    check("a Potion", 0, "    a Potion");
    check("a", 1, "    a");
    check("a ", 1, "  a ");
    check("a    ", 1, "  a ");
    check("", 1, "    ");
    check("中文匕首", 1, "    中文匕首");
    check("長劍", 0, "    長劍");
    check("紅寶 石", 1, "    紅寶 石");
    check("1234 Arrows", 1, "    1234 Arrows");
    check("some bread", 1, "    some bread");
    check("a 中文短劍", 1, "  a 中文短劍");
    check("an 魔法卷軸", 1, " an 魔法卷軸");
    check("3 個護符", 1, "  3 個護符");
    check("一個 短劍", 1, "    一個 短劍");
    check("The sword", 1, "The sword");
    check("the Sword", 1, "the Sword");
    check(broken, 1, "    ?X");
    CHECK(!angdroid_object_list_name_copy(out, sizeof(out), NULL, 1));
    CHECK(!out[0]);
}

static void check_capacities(void)
{
    const char *names[] = { "魔法卷軸", "a 魔法卷軸", "一個 長劍",
        "a Long sword", "", "a", "12345 Arrows" };
    size_t j, cap;
    for (j = 0; j < sizeof(names)/sizeof(names[0]); ++j) {
        for (cap = 0; cap <= 32; ++cap) {
            char storage[34], *out = storage + 1;
            size_t pos, i;
            memset(storage, 'Z', sizeof(storage));
            pos = angdroid_object_list_name_copy(out, cap, names[j], 1);
            CHECK(storage[0] == 'Z');
            CHECK(storage[33] == 'Z');
            CHECK(storage[1 + cap] == 'Z');
            if (cap == 0) {
                CHECK(storage[1] == 'Z');
                continue;
            }
            CHECK(pos < cap);
            CHECK(out[pos] == '\0');
            CHECK(strlen(out) == pos);
            if (cap > 5) {
                CHECK(pos >= 4);
                if ((unsigned char)names[j][0] >= 0x80u ||
                    names[j][0] == '\0')
                    CHECK(memcmp(out, "    ", 4) == 0);
            }
            for (i = 0; i < pos; ++i) {
                unsigned char c = (unsigned char)out[i];
                if (c < 0x80u) continue;
                CHECK(c >= 0xE0u && c <= 0xEFu);
                CHECK(i + 2 < pos);
                CHECK(((unsigned char)out[i + 1] & 0xC0u) == 0x80u);
                CHECK(((unsigned char)out[i + 2] & 0xC0u) == 0x80u);
                i += 2;
            }
        }
    }
    CHECK(!angdroid_object_list_name_copy(NULL, 12, "x", 1));
}

int main(void)
{
    check_examples();
    check_capacities();
    printf("PASS: object list UTF-8 name prefix (%u assertions)\n", assertions);
    return 0;
}
