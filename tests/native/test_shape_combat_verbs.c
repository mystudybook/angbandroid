/* All canonical player-shape blow verbs must have localized combat output. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-combat.h"

static unsigned checks;
#define CHECK(c) do { assert(c); ++checks; } while (0)
int main(void)
{
    static const struct { const char *en, *zh; } cases[] = {
        {"bite", "咬住了"},
        {"claw", "用利爪抓傷了"},
        {"clout", "猛擊了"},
        {"crush", "重壓了"},
        {"cuff", "掌擊了"},
        {"hit", "擊中了"},
        {"peck", "啄擊了"},
        {"punch", "揮拳擊中了"},
        {"scratch", "抓傷了"},
        {"slash", "劈砍了"},
        {"strike", "擊打了"},
        {"swipe", "猛抓了"},
        {"tear", "撕裂了"},
        {"thump", "重捶了"},
    };
    size_t i;
    CHECK(sizeof(cases) / sizeof(cases[0]) == 14);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const char *actual = angdroid_melee_verb_zh_tw(cases[i].en);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, cases[i].zh) == 0);
        CHECK(strcmp(actual, cases[i].en) != 0);
    }
    /* Unknown verbs must retain the original full English sentence. */
    CHECK(angdroid_melee_verb_zh_tw(NULL) == NULL);
    CHECK(angdroid_melee_verb_zh_tw("custom shape blow") == NULL);
    CHECK(angdroid_melee_verb_zh_tw("Claw") == NULL);
    CHECK(angdroid_melee_verb_zh_tw("claw ") == NULL);
    CHECK(strcmp(angdroid_melee_verb_zh_tw("fail to harm"),
        "未能傷害") == 0);
    CHECK(strcmp(angdroid_hit_quality_zh_tw("It was a good hit!"),
        "這是一次漂亮的攻擊！") == 0);
    CHECK(angdroid_ranged_verb_zh_tw("bite") == NULL);
    printf("PASS: 14 canonical shape combat verbs, fallback and isolation (%u checks)\n",
        checks);
    return 0;
}
