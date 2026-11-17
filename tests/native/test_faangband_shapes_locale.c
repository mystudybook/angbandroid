/* Test the FAangband display mapping on the actual production header. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-faangband-shapes.h"

static unsigned long checks;
#define CHECK(condition) do { assert(condition); ++checks; } while (0)

int main(void)
{
    static const struct { const char *english, *traditional; } examples[] = {
        { "bear cub", "小熊" },
        { "great bear", "巨熊" },
        { "black dragon", "黑龍" },
        { "blue dragon", "藍龍" },
        { "white dragon", "白龍" },
        { "red dragon", "紅龍" },
        { "green dragon", "綠龍" },
        { "shining dragon", "輝光龍" },
        { "law dragon", "秩序龍" },
        { "chaos dragon", "混沌龍" },
        { "gold dragon", "金龍" },
        { "multi-hued dragon", "多彩龍" },
        { "balance dragon", "平衡龍" },
        { "power dragon", "威能龍" },
    };
    size_t i;
    CHECK(ANGDROID_ZH_TW_FAANGBAND_SHAPE_COUNT == 14);
    CHECK(sizeof(examples) / sizeof(examples[0]) == 14);
    for (i = 0; i < sizeof(examples) / sizeof(examples[0]); ++i) {
        const char *actual = angdroid_faangband_shape_name(
            examples[i].english, 1);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, examples[i].traditional) == 0);
        CHECK(strcmp(angdroid_faangband_shape_name(
            examples[i].english, 0), examples[i].english) == 0);
        CHECK(strlen(actual) == strlen(examples[i].traditional));
    }
    CHECK(angdroid_faangband_shape_name("bear", 1)[0] == 'b');
    CHECK(strcmp(angdroid_faangband_shape_name("unseen shape", 1),
        "unseen shape") == 0);
    CHECK(angdroid_faangband_shape_name(NULL, 1) == NULL);
    CHECK(angdroid_faangband_shape_name(NULL, 0) == NULL);
    CHECK(strcmp(angdroid_faangband_shape_ability_desc(
        "You can fly.", 1), "你能飛行。") == 0);
    CHECK(strcmp(angdroid_faangband_shape_ability_desc(
        "You can fly.", 0), "You can fly.") == 0);
    CHECK(strcmp(angdroid_faangband_shape_ability_desc(
        "You can fly?", 1), "You can fly?") == 0);
    CHECK(angdroid_faangband_shape_ability_desc(NULL, 1) == NULL);
    printf("PASS: 14 FAangband shape names and flying ability (%lu assertions)\n",
        checks);
    return 0;
}
