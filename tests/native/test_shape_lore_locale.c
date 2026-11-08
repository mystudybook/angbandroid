/* Shape knowledge: static bilingual data and exact unknown fallbacks. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-shape-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-spell-names.h"

static int checks;
static void shape(const char *en, const char *zh)
{
    assert(strcmp(angdroid_shape_name(en, 1), zh) == 0);
    assert(strcmp(angdroid_shape_name(en, 0), en) == 0);
    checks += 2;
}
static void term(const char *en, const char *zh)
{
    assert(strcmp(angdroid_shape_term(en, 1), zh) == 0);
    assert(strcmp(angdroid_shape_term(en, 0), en) == 0);
    checks += 2;
}
int main(void)
{
    shape("normal", "一般形態");
    shape("fox", "狐狸");
    shape("Pukel-man", "普克爾人");
    shape("bear", "熊");
    shape("eagle", "鷹");
    shape("bat", "蝙蝠");
    shape("warg", "座狼");
    shape("vampire", "吸血鬼");
    shape("werewolf", "狼人");
    shape("bear cub", "小熊");
    shape("great bear", "巨熊");
    shape("black dragon", "黑龍");
    shape("blue dragon", "藍龍");
    shape("white dragon", "白龍");
    shape("red dragon", "紅龍");
    shape("green dragon", "綠龍");
    shape("shining dragon", "輝光龍");
    shape("law dragon", "秩序龍");
    shape("chaos dragon", "混沌龍");
    shape("gold dragon", "金龍");
    shape("multi-hued dragon", "多彩龍");
    shape("balance dragon", "平衡龍");
    shape("power dragon", "威能龍");
    shape("unlisted", "unlisted");
    assert(angdroid_shape_name(NULL, 1) == NULL);
    ++checks;
    term("physical disarming", "物理解除陷阱");
    term("melee to hit", "近戰命中");
    term("saving throws", "豁免判定");
    term("strength", "力量");
    term("stealth", "潛行");
    term("fire", "火焰");
    term("shards", "碎片");
    term("Sustains", "維持以下能力：");
    term("unknown property", "unknown property");
    assert(angdroid_shape_term(NULL, 1) == NULL);
    ++checks;
    assert(strcmp(angdroid_shape_class("Druid", 1), "德魯伊") == 0);
    assert(strcmp(angdroid_shape_class("Mage", 0), "Mage") == 0);
    assert(strcmp(angdroid_shape_class("unknown", 1), "unknown") == 0);
    assert(strcmp(angdroid_zh_tw_spell_name("Fox Form", 1), "狐狸形態") == 0);
    checks += 4;
    assert(strstr(angdroid_shape_intro(1), "變身當下的裝備") != NULL);
    assert(strstr(angdroid_shape_intro(1), "撿起與吃東西除外") != NULL);
    assert(strstr(angdroid_shape_intro(1), "例如丟棄") != NULL);
    assert(strstr(angdroid_shape_intro(0), "items in your pack") != NULL);
    assert(strstr(angdroid_shape_intro(0), "drop, for instance") != NULL);
    checks += 5;

    /* All six misc flag desc: keys and the ROCK player flag in shape.txt. */
    {
        static const struct { const char *en; const char *zh; } cases[] = {
            { "Feather Falling", "緩慢墜落" },
            { "Prevents paralysis", "防止麻痺" },
            { "Speeds regeneration", "加快生命恢復" },
            { "Sustains your life force", "維持生命力" },
            { "Grants the ability to see invisible things", "能看見隱形事物" },
            { "Gives immunity to traps", "免疫陷阱效果" },
            { "You are made of rock.", "你的身體由岩石構成。" },
        };
        size_t i;
        for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
            assert(strcmp(angdroid_shape_misc_desc(cases[i].en, 1),
                cases[i].zh) == 0);
            assert(strcmp(angdroid_shape_misc_desc(cases[i].en, 0),
                cases[i].en) == 0);
            checks += 2;
        }
    }
    assert(strcmp(angdroid_shape_misc_desc("Custom flag.", 1),
        "Custom flag.") == 0);
    assert(angdroid_shape_misc_desc(NULL, 1) == NULL);
    checks += 2;
    /* New FAangband FLYING player-ability text, not a global property edit. */
    assert(strcmp(angdroid_shape_misc_desc("You can fly.", 1),
        "你能飛行。") == 0);
    assert(strcmp(angdroid_shape_misc_desc("You can fly.", 0),
        "You can fly.") == 0);
    checks += 2;
    printf("PASS: Angband shape knowledge bilingual labels (%d checks)\n", checks);
    return 0;
}
