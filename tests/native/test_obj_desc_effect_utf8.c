/* Real effect suffix helper; compile on host without the Angband runtime. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/obj-desc-effect-utf8.h"

static unsigned long checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)

static void example(const char *base, const char *name, int mode,
    const char *expected)
{
    char out[256];
    size_t end = strlen(base);
    strcpy(out, base);
    if (mode == 0)
        CHECK(angdroid_obj_desc_append_flavor_effect(out, sizeof(out),
            &end, name, false));
    else if (mode == 1)
        CHECK(angdroid_obj_desc_append_flavor_effect(out, sizeof(out),
            &end, name, true));
    else
        CHECK(angdroid_obj_desc_append_scroll_effect(out, sizeof(out),
            &end, name));
    CHECK(strcmp(out, expected) == 0);
    CHECK(end == strlen(expected));
}

static void examples(void)
{
    const char bad[] = { (char)0xE4, 'x', 0 };
    char text[32] = "卷軸";
    size_t end = strlen(text);
    example("a Scroll", "Identify", 0, "a Scroll of Identify");
    example("a Scroll", "Identify", 1, "a Scroll 'Identify'");
    example("卷軸", "辨識", 2, "卷軸（辨識）");
    example("卷軸", "Identify", 2, "卷軸（Identify）");
    example("Potion", "治癒效果", 0, "Potion of 治癒效果");
    example("Scroll", "中文效果😀", 1, "Scroll '中文效果😀'");
    CHECK(!angdroid_obj_desc_append_scroll_effect(text, sizeof(text),
        &end, bad));
    CHECK(end == strlen("卷軸") && strcmp(text, "卷軸") == 0);
    CHECK(!angdroid_obj_desc_append_flavor_effect(text, sizeof(text),
        &end, "", false));
    CHECK(!angdroid_obj_desc_append_scroll_effect(text, sizeof(text),
        &end, NULL));
    CHECK(!angdroid_obj_desc_append_scroll_effect(NULL, sizeof(text),
        &end, "Identify"));
    CHECK(!angdroid_obj_desc_append_scroll_effect(text, sizeof(text),
        NULL, "Identify"));
    CHECK(!angdroid_obj_desc_append_scroll_effect(text, 0,
        &end, "Identify"));
    CHECK(!angdroid_obj_desc_append_flavor_effect(text, sizeof(text),
        &end, "中文長效果需要更多空間中文長效果", false));
    CHECK(end == strlen("卷軸") && strcmp(text, "卷軸") == 0);
}

static void capacities(void)
{
    const char *names[] = {
        "Identify", "辨識", "魔法中文效果", "😀", "", "Potion of Healing",
        "a particularly long English effect name", "長度", "「名字」"
    };
    size_t i, cap, start, mode;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        for (cap = 0; cap <= 80; ++cap)
            for (start = 0; start <= 11; ++start)
                for (mode = 0; mode <= 2; ++mode) {
                    char guard[96], original[96];
                    char *out = guard + 1;
                    size_t end = start, prefix_length, suffix_length;
                    const char *prefix = mode == 2 ? "（" : (mode == 1 ? " '" : " of ");
                    const char *suffix = mode == 2 ? "）" : (mode == 1 ? "'" : "");
                    const size_t name_length = strlen(names[i]);
                    bool result;
                    memset(guard, '!', sizeof(guard));
                    if (cap > start) {
                        memset(out, 'z', start);
                        out[start] = '\0';
                    }
                    memcpy(original, guard, sizeof(guard));
                    if (mode == 2)
                        result = angdroid_obj_desc_append_scroll_effect(
                            out, cap, &end, names[i]);
                    else
                        result = angdroid_obj_desc_append_flavor_effect(
                            out, cap, &end, names[i], mode == 1);
                    CHECK(guard[0] == '!');
                    CHECK(guard[cap + 1] == '!');
                    CHECK(guard[95] == '!');
                    prefix_length = strlen(prefix);
                    suffix_length = strlen(suffix);
                    if (!result) {
                        CHECK(end == start);
                        CHECK(memcmp(original, guard, sizeof(guard)) == 0);
                    } else {
                        CHECK(name_length != 0);
                        CHECK(cap > start + prefix_length + name_length + suffix_length);
                        CHECK(end == start + prefix_length + name_length + suffix_length);
                        CHECK(out[end] == 0);
                        CHECK(memcmp(out, original + 1, start) == 0);
                        CHECK(memcmp(out + start, prefix, prefix_length) == 0);
                        CHECK(memcmp(out + start + prefix_length, names[i], name_length) == 0);
                        CHECK(memcmp(out + start + prefix_length + name_length,
                            suffix, suffix_length) == 0);
                    }
                }
}

int main(void)
{
    examples();
    capacities();
    printf("PASS: atomic bilingual effect suffix (%lu assertions)\n", checks);
    return 0;
}
