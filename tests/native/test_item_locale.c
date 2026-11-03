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
    assert(ANGDROID_ZH_TW_ITEM_KINDS == 76);
    expect_name("& Wooden Torch~", "木製火把");
    expect_name("& Lantern~", "提燈");
    expect_name("& Dagger~", "匕首");
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

    puts("PASS: 76 item-kind display names, exact matching and fallback");
    return 0;
}
