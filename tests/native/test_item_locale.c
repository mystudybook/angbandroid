#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/zh-tw-items.h"

static void expect_name(const char *english, const char *traditional)
{
    const char *actual = angdroid_zh_tw_item_kind_name(english);
    assert(actual != NULL);
    assert(strcmp(actual, traditional) == 0);
}

int main(void)
{
    assert(ANGDROID_ZH_TW_ITEM_KINDS == 326);
    expect_name("& Wooden Torch~", "木製火把");
    expect_name("& Lantern~", "提燈");
    expect_name("& Dagger~", "匕首");
    expect_name("Strength", "力量");
    expect_name("Resist Poison", "抗毒");
    expect_name("*Healing*", "強效治療");
    expect_name("Word of Recall", "回返咒語");
    expect_name("Dragon's Breath", "龍之吐息");
    expect_name("Recall", "回返");
    expect_name("& Zweihander~", "雙手大劍");
    expect_name("& Mithril Bolt~", "秘銀弩箭");
    expect_name("Power Dragon Scale Mail~", "力量龍鱗甲");
    expect_name("& Massive Iron Crown~", "巨大鐵冠");
    /* Exact grammar markers in object.txt matter to display-only lookup. */
    expect_name("& Main~ Gauche~", "副手短劍");
    assert(angdroid_zh_tw_item_kind_name("& Main Gauche~") == NULL);
    expect_name("& Short Sword~", "短劍");
    expect_name("& Long Sword~", "長劍");
    expect_name("& Katana~", "武士刀");
    expect_name("& Arrow~", "箭矢");
    expect_name("& Pair~ of Leather Boots", "皮靴");
    expect_name("& Robe~", "長袍");
    expect_name("Chain Mail~", "鎖子甲");
    expect_name("Full Plate Armour~", "全身板甲");
    expect_name("& Elven Cloak~", "精靈披風");
    expect_name("& Set~ of Gauntlets", "護手");
    expect_name("& Mithril Shield~", "秘銀盾");
    assert(angdroid_zh_tw_item_kind_name("unknown item") == NULL);
    assert(angdroid_zh_tw_item_kind_name("& dagger~") == NULL);
    assert(angdroid_zh_tw_item_kind_name(NULL) == NULL);

    puts("PASS: 326 item-kind display names, exact matching and fallback");
    return 0;
}
