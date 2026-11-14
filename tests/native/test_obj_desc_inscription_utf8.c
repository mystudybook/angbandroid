/* Real inscription-layout helper, compiled independently of the game. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/obj-desc-inscription-utf8.h"

static unsigned long assertions;
#define CHECK(expr) do { assert(expr); ++assertions; } while (0)

static void test_examples(void)
{
    char buf[160] = "劍";
    const char *plain[] = { "hello", "cursed", "ignore" };
    const char *translated[] = { "中文刻印", "受詛咒", "忽略" };
    const char *bad[] = { "\xE4" "bad" };
    const char *one[] = { "中文刻印" };
    const char *statuses[] = { "受詛咒", "忽略" };
    const char *empty[] = { "" };
    size_t end;

    end = strlen(buf);
    CHECK(angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        plain, 3, false, true) == strlen(" {hello, cursed, ignore}"));
    CHECK(strcmp(buf, "劍 {hello, cursed, ignore}") == 0);
    end = strlen("劍");
    buf[end] = 0;
    CHECK(angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        translated, 3, true, true) == strlen(" {中文刻印、受詛咒、忽略}"));
    CHECK(strcmp(buf, "劍 {中文刻印、受詛咒、忽略}") == 0);

    end = 0;
    CHECK(angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        one, 1, false, true) == strlen(" {中文刻印}"));
    CHECK(strcmp(buf, " {中文刻印}") == 0);
    end = 0;
    CHECK(angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        statuses, 2, true, false) == strlen(" {受詛咒、忽略}"));
    CHECK(strcmp(buf, " {受詛咒、忽略}") == 0);
    end = 0;
    CHECK(angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        bad, 1, true, true) == strlen(" {?bad}"));
    CHECK(strcmp(buf, " {?bad}") == 0);
    end = 0;
    CHECK(angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        empty, 1, true, true) == strlen(" {}"));
    CHECK(strcmp(buf, " {}") == 0);
    end = 0;
    CHECK(!angdroid_obj_desc_inscription_append(buf, 0, &end,
        one, 1, false, true));
    CHECK(!angdroid_obj_desc_inscription_append(NULL, sizeof(buf), &end,
        one, 1, false, true));
    CHECK(!angdroid_obj_desc_inscription_append(buf, sizeof(buf), NULL,
        one, 1, false, true));
    CHECK(!angdroid_obj_desc_inscription_append(buf, sizeof(buf), &end,
        NULL, 1, false, true));
}

static void test_lengths(void)
{
    const char *examples[][5] = {
        { "中文中文中文", "受詛咒", "忽略", NULL, NULL },
        { "first note", "cursed", "ignore", "tried", "??" },
        { "中文與English混合", "已試用", NULL, NULL, NULL },
        { "單", NULL, NULL, NULL, NULL },
        { "a", "中文", NULL, NULL, NULL },
        { "", NULL, NULL, NULL, NULL },
    };
    size_t which, cap;
    for (which = 0; which < sizeof(examples) / sizeof(examples[0]); ++which) {
        for (cap = 0; cap < 90; ++cap) {
            char memory[96], *out = memory + 1;
            size_t end = 0, written;
            uint32_t scalar;
            size_t pos = 0;
            memset(memory, '!', sizeof(memory));
            written = angdroid_obj_desc_inscription_append(out,
                cap, &end, examples[which], 5, which % 2 == 0,
                which != 3);
            CHECK(memory[0] == '!');
            CHECK(memory[cap + 1] == '!');
            CHECK(memory[95] == '!');
            if (written) {
                CHECK(end == written);
                CHECK(out[0] == ' ' && out[1] == '{');
                CHECK(out[end - 1] == '}' && out[end] == 0);
                CHECK(strlen(out) == end);
                while (pos < end) {
                    size_t n = angdroid_utf8_next_scalar(out + pos, &scalar);
                    CHECK(n != 0 && scalar != 0xFFFDu);
                    pos += n;
                }
                CHECK(pos == end);
            } else {
                CHECK(end == 0);
                if (cap >= 4) CHECK(out[0] == 0);
            }
        }
    }
}

int main(void)
{
    test_examples();
    test_lengths();
    printf("PASS: inscription display UTF-8 (%lu assertions)\n", assertions);
    return 0;
}
