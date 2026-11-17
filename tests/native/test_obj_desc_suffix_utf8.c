/* Real display suffix helper: English and Chinese names must append atomically. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/obj-desc-suffix-utf8.h"

static unsigned long assertions;
#define CHECK(x) do { assert(x); ++assertions; } while (0)

static void sample(const char *base, const char *suffix, int space,
    const char *expected)
{
    char out[256];
    size_t end = strlen(base);
    strcpy(out, base);
    CHECK(angdroid_obj_desc_append_suffix_atomic(out, sizeof(out), &end,
        suffix, space != 0));
    CHECK(strcmp(out, expected) == 0);
    CHECK(end == strlen(expected));
}

static void examples(void)
{
    const char invalid[] = { (char)0xE4, 'x', 0 };
    char output[24] = "長劍";
    size_t end = strlen(output);
    sample("a Sword", "of Gondolin", 1, "a Sword of Gondolin");
    sample("長劍", "（守護者）", 0, "長劍（守護者）");
    sample("長劍", "「龍焰」", 0, "長劍「龍焰」");
    sample("戒指", "火焰之力", 1, "戒指 火焰之力");
    sample("a Ring", "of Speed", 1, "a Ring of Speed");
    sample("光劍", "（炎）", 0, "光劍（炎）");
    sample("盾", "😀", 1, "盾 😀");
    CHECK(!angdroid_obj_desc_append_suffix_atomic(output, sizeof(output),
        &end, invalid, true));
    CHECK(end == strlen("長劍") && strcmp(output, "長劍") == 0);
    CHECK(!angdroid_obj_desc_append_suffix_atomic(output, sizeof(output),
        &end, "", true));
    CHECK(end == strlen("長劍") && strcmp(output, "長劍") == 0);
    CHECK(!angdroid_obj_desc_append_suffix_atomic(output, sizeof(output),
        &end, NULL, true));
    CHECK(!angdroid_obj_desc_append_suffix_atomic(NULL, sizeof(output),
        &end, "foo", true));
    CHECK(!angdroid_obj_desc_append_suffix_atomic(output, sizeof(output),
        NULL, "foo", true));
    CHECK(!angdroid_obj_desc_append_suffix_atomic(output, 0,
        &end, "foo", true));
    CHECK(!angdroid_obj_desc_append_suffix_atomic(output, sizeof(output),
        &end, "中文文字非常長超出緩衝", true));
    CHECK(end == strlen("長劍") && strcmp(output, "長劍") == 0);
}

static void capacities(void)
{
    const char *texts[] = { "（守護者）", "of Speed", "😀魔劍", "中文",
        "火焰之力", "「魔法之盾」", "", "a very long English suffix" };
    size_t i, cap, head, space;
    for (i = 0; i < sizeof(texts)/sizeof(texts[0]); ++i)
        for (cap = 0; cap <= 80; ++cap)
            for (head = 0; head <= 9; ++head)
                for (space = 0; space <= 1; ++space) {
                    char guard[96], prior[96];
                    char *out = guard + 1;
                    size_t end = head, initial = head;
                    bool ok;
                    size_t length = strlen(texts[i]);
                    memset(guard, '@', sizeof(guard));
                    if (cap > head) {
                        memset(out, 'b', head);
                        out[head] = '\0';
                    }
                    memcpy(prior, guard, sizeof(guard));
                    ok = angdroid_obj_desc_append_suffix_atomic(out, cap,
                        &end, texts[i], space != 0);
                    CHECK(guard[0] == '@');
                    CHECK(guard[cap + 1] == '@');
                    CHECK(guard[95] == '@');
                    if (!ok) {
                        CHECK(end == initial);
                        CHECK(memcmp(guard, prior, sizeof(guard)) == 0);
                    } else {
                        CHECK(length != 0);
                        CHECK(cap > head + space + length);
                        CHECK(end == head + space + length);
                        CHECK(out[end] == '\0');
                        CHECK(memcmp(out, prior + 1, head) == 0);
                        if (space) CHECK(out[head] == ' ');
                        CHECK(memcmp(out + head + space, texts[i], length) == 0);
                    }
                }
}

int main(void)
{
    examples();
    capacities();
    printf("PASS: item suffix atomic UTF-8 (%lu assertions)\n", assertions);
    return 0;
}
