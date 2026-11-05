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
    printf("PASS: Angband shape knowledge bilingual labels (%d checks)\n", checks);
    return 0;
}
