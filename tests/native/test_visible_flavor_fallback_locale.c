/* Display-only recovery for uncovered flavors and untranslated known effects. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-flavored-object.h"

static unsigned long checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static void example(const char *tval, const char *appearance, const char *effect,
    unsigned int count, bool prefix, const char *expected)
{
    char output[256];
    CHECK(angdroid_zh_tw_flavored_object(output, sizeof(output), tval,
        appearance, effect, prefix, count));
    CHECK(strcmp(output, expected) == 0);
    CHECK(strlen(output) == strlen(expected));
}

static void cases(void)
{
    char output[256] = "unchanged";
    example("ring", "Unmapped Azure", NULL, 1, true, "Unmapped Azure戒指");
    example("ring", "Unmapped Azure", "Strength", 1, true,
        "Unmapped Azure戒指（Strength）");
    example("ring", "紅寶石", "力量", 1, true, "紅寶石戒指（力量）");
    example("potion", "未知色彩", "Healing", 3, true,
        "3 瓶未知色彩藥水（Healing）");
    example("wand", "Etched Copper", NULL, 2, true,
        "2 根Etched Copper魔杖");
    example("rod", "Arcane Alloy", "Detection", 0, true,
        "沒有剩餘的Arcane Alloy魔棒（Detection）");
    example("mushroom", "Unmapped", NULL, 2, false, "Unmapped蘑菇");
    example("staff", "Rosewood", "Light", 1, true,
        "Rosewood法杖（Light）");
    example("amulet", "Azure", "WIS", 1, true,
        "Azure護身符（WIS）");
    CHECK(!angdroid_zh_tw_flavored_object(output, sizeof(output),
        "scroll", "Title", NULL, true, 1));
    CHECK(output[0] == 0);
    CHECK(!angdroid_zh_tw_flavored_object(output, sizeof(output),
        "ring", "Unmapped", "", true, 1));
    CHECK(output[0] == 0);
    CHECK(!angdroid_zh_tw_flavored_object(output, sizeof(output),
        "ring", NULL, NULL, true, 1));
    CHECK(output[0] == 0);
}

static void bounds(void)
{
    const char *appearances[] = { "Unmapped", "中文外觀", "😀灰色", "Bronze" };
    const char *effects[] = { NULL, "Strength", "力量", "Power😀" };
    size_t cap, i, j, count;
    for (cap = 0; cap <= 128; ++cap)
        for (i = 0; i < 4; ++i)
            for (j = 0; j < 4; ++j)
                for (count = 0; count < 4; ++count) {
                    char guard[140], *output = guard + 1;
                    size_t actual;
                    bool ok;
                    memset(guard, '#', sizeof(guard));
                    ok = angdroid_zh_tw_flavored_object(output, cap,
                        "ring", appearances[i], effects[j], true,
                        (unsigned int)count);
                    CHECK(guard[0] == '#');
                    CHECK(guard[cap + 1] == '#');
                    CHECK(guard[139] == '#');
                    if (ok) {
                        actual = strlen(output);
                        CHECK(actual < cap);
                        CHECK(strstr(output, appearances[i]) != NULL);
                        if (effects[j]) {
                            CHECK(strstr(output, effects[j]) != NULL);
                            CHECK(actual >= strlen("）"));
                            CHECK(strcmp(output + actual - strlen("）"), "）") == 0);
                        }
                    } else if (cap) CHECK(output[0] == 0);
                }
}

int main(void)
{
    cases();
    bounds();
    printf("PASS: unknown flavor fallback with Chinese nouns (%lu assertions)\n", checks);
    return 0;
}
