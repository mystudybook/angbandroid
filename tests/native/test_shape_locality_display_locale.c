/* Canonical Angband/FAangband player shape and locality display names.
 * Host C99 test for exact matching and unchanged English/unknown fallback.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-shape-display.h"
#include "../../app/src/main/cpp/angband/zh-tw-faangband-localities.h"

static unsigned tests;
#define CHECK(x) do { assert(x); ++tests; } while (0)

static void shape(const char *en, const char *zh)
{
    CHECK(strcmp(angdroid_shape_display_name(en, 1), zh) == 0);
    CHECK(angdroid_shape_display_name(en, 0) == en);
}

int main(void)
{
    static const struct { const char *en, *zh; } shapes[] = {
        { "normal", "正常" }, { "fox", "狐狸" },
        { "Pukel-man", "普克爾人" }, { "bear", "熊" },
        { "eagle", "老鷹" }, { "bat", "蝙蝠" },
        { "warg", "座狼" }, { "vampire", "吸血鬼" },
        { "werewolf", "狼人" }, { "bear cub", "小熊" },
        { "great bear", "巨熊" }, { "black dragon", "黑龍" },
        { "blue dragon", "藍龍" }, { "white dragon", "白龍" },
        { "red dragon", "紅龍" }, { "green dragon", "綠龍" },
        { "shining dragon", "輝光龍" }, { "law dragon", "秩序龍" },
        { "chaos dragon", "混沌龍" }, { "gold dragon", "金龍" },
        { "multi-hued dragon", "多彩龍" },
        { "balance dragon", "平衡龍" }, { "power dragon", "威能龍" },
    };
    static const struct { const char *en, *zh; } localities[] = {
        { "Hithaeglir", "迷霧山脈" },
        { "Eriador", "伊利雅德" },
        { "East Beleriand", "東貝烈瑞安德" },
        { "West Beleriand", "西貝烈瑞安德" },
        { "Dor Dínen", "多爾迪能" },
        { "Amon Rûdh", "阿蒙魯茲" },
        { "Khazad Dûm", "凱薩督姆" },
        { "Gondolin", "剛多林" },
        { "Underworld", "地底世界" },
        { "Mountain Top", "山頂" },
        { "Arena", "競技場" },
    };
    size_t i;
    const char *unknown_shape = "future dragon";
    const char *unknown_location = "future locality";

    CHECK(sizeof(shapes) / sizeof(shapes[0]) == 23);
    for (i = 0; i < sizeof(shapes) / sizeof(shapes[0]); ++i)
        shape(shapes[i].en, shapes[i].zh);

    for (i = 0; i < sizeof(localities) / sizeof(localities[0]); ++i) {
        CHECK(strcmp(angdroid_faangband_locality_text(localities[i].en, 1),
                     localities[i].zh) == 0);
        CHECK(angdroid_faangband_locality_text(localities[i].en, 0) ==
              localities[i].en);
    }
    CHECK(angdroid_shape_display_name(NULL, 1) == NULL);
    CHECK(angdroid_shape_display_name(NULL, 0) == NULL);
    CHECK(angdroid_faangband_locality_text(NULL, 1) == NULL);
    CHECK(angdroid_faangband_locality_text(NULL, 0) == NULL);
    CHECK(angdroid_shape_display_name(unknown_shape, 1) == unknown_shape);
    CHECK(angdroid_faangband_locality_text(unknown_location, 1) ==
          unknown_location);
    CHECK(angdroid_shape_display_name("Bear", 1)[0] == 'B');
    printf("PASS: 23 shape translations and locality display fallbacks (%u checks)\n",
           tests);
    return 0;
}
