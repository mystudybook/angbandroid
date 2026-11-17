/* Display-only FAangband kind names, independent of engine data. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-faangband-items.h"

static unsigned long checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)

static void coverage(void)
{
    static const struct { const char *english, *tval, *expected; } examples[] = {
        { "& Main Gauche~", "sword", "副手短劍" },
        { "& Valinorean Sword~", "sword", "維林諾長劍" },
        { "& Magestaff~", "hafted", "法師長杖" },
        { "& Storm Hammer~", "hafted", "風暴戰鎚" },
        { "& Dart~", "polearm", "飛鏢" },
        { "& Thrusting Spear~", "polearm", "突刺長矛" },
        { "& Silver Crown~", "crown", "銀冠" },
        { "& Shadow Cloak~", "cloak", "暗影斗篷" },
        { "& Body Shield~", "shield", "全身盾" },
        { "& Shield of Deflection~", "shield", "偏斜之盾" },
        { "Steel", "ring", "鋼製戒指" },
        { "Pewter", "ring", "錫鑞戒指" },
        { "Galvorn", "ring", "加爾沃恩戒指" },
        { "Brass", "ring", "黃銅戒指" },
        { "Bronze", "ring", "青銅戒指" },
        { "Copper", "ring", "銅戒指" },
        { "Silver", "ring", "銀戒指" },
        { "Gold", "ring", "金戒指" },
        { "Adamantite", "ring", "精金戒指" },
        { "Mithril", "ring", "秘銀戒指" },
        { "Chalcedony", "amulet", "玉髓護身符" },
        { "Malachite", "amulet", "孔雀石護身符" },
        { "Opal", "amulet", "蛋白石護身符" },
        { "Garnet", "amulet", "石榴石護身符" },
        { "Pearl", "amulet", "珍珠護身符" },
        { "Amethyst", "amulet", "紫水晶護身符" },
        { "Sapphire", "amulet", "藍寶石護身符" },
        { "Ruby", "amulet", "紅寶石護身符" },
        { "Emerald", "amulet", "祖母綠護身符" },
        { "Diamond", "amulet", "鑽石護身符" },
        { "Striking", "wand", "打擊魔杖" },
        { "Storms", "wand", "風暴魔杖" },
        { "Shrapnel", "wand", "破片魔杖" },
        { "Lightning Strike", "rod", "雷擊魔棒" },
        { "Northwinds", "rod", "北風魔棒" },
        { "Dragonfire", "rod", "龍焰魔棒" },
        { "Glaurung's Blood", "rod", "格勞龍之血魔棒" },
        { "& Small jeweled chest~", "chest", "小型鑲寶石寶箱" },
        { "& Large jeweled chest~", "chest", "大型鑲寶石寶箱" },
    };
    size_t i;
    CHECK(ANGDROID_ZH_TW_FAANGBAND_KINDS == 39);
    CHECK(sizeof(examples) / sizeof(examples[0]) == 39);
    for (i = 0; i < sizeof(examples) / sizeof(examples[0]); ++i) {
        const char *actual = angdroid_zh_tw_faangband_item_kind_name(
            examples[i].english, examples[i].tval);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, examples[i].expected) == 0);
        CHECK(strlen(actual) == strlen(examples[i].expected));
        CHECK(strcmp(angdroid_zh_tw_faangband_item_kind_name(
            examples[i].english, examples[i].tval), examples[i].expected) == 0);
        CHECK(angdroid_zh_tw_faangband_item_kind_name(
            examples[i].english, "other book") == NULL);
    }
    CHECK(angdroid_zh_tw_faangband_item_kind_name("<unknown item>", "none") == NULL);
    CHECK(angdroid_zh_tw_faangband_item_kind_name("& Long Sword~", "sword") == NULL);
    CHECK(angdroid_zh_tw_faangband_item_kind_name("Strength", "ring") == NULL);
    CHECK(angdroid_zh_tw_faangband_item_kind_name("& Custom Sword~", "sword") == NULL);
    CHECK(angdroid_zh_tw_faangband_item_kind_name(NULL, "ring") == NULL);
    CHECK(angdroid_zh_tw_faangband_item_kind_name("Steel", NULL) == NULL);
}

int main(void)
{
    coverage();
    printf("PASS: 39 FAangband kind translations (%lu assertions)\n", checks);
    return 0;
}
