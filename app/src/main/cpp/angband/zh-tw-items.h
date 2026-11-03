/*
 * Item kind DISPLAY names for the knowledge/insight UI only.
 * Canonical English kind templates are from Angband 4.2.4 object.txt.
 * The '&', '~' grammar templates stay unchanged in actual object records.
 * Never touch object_desc() or unidentified flavor presentation.
 */
#ifndef ANGDROID_ZH_TW_ITEMS_H
#define ANGDROID_ZH_TW_ITEMS_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_ITEM_KINDS 37

static inline const char *angdroid_zh_tw_item_kind_name(const char *english)
{
    static const struct {
        const char *english;
        const char *traditional;
    } names[ANGDROID_ZH_TW_ITEM_KINDS] = {
        { "& Wooden Torch~", "木製火把" },
        { "& Lantern~", "提燈" },
        { "& Flask~ of oil", "油瓶" },
        { "& Hard Biscuit~", "硬餅乾" },
        { "& Apple~", "蘋果" },
        { "& Handful~ of Dried Fruits", "一把乾果" },
        { "& Slime Mold~", "黏菌" },
        { "& Ration~ of Food", "口糧" },
        { "& Honey-cake~", "蜂蜜蛋糕" },
        { "& Piece~ of Elvish Waybread", "精靈乾糧" },
        { "& Dagger~", "匕首" },
        { "& Main Gauche~", "副手短劍" },
        { "& Rapier~", "細劍" },
        { "& Short Sword~", "短劍" },
        { "& Cutlass~", "短彎刀" },
        { "& Scimitar~", "彎刀" },
        { "& Long Sword~", "長劍" },
        { "& Broad Sword~", "闊劍" },
        { "& Bastard Sword~", "手半劍" },
        { "& Katana~", "武士刀" },
        { "& Whip~", "鞭子" },
        { "& Mace~", "釘頭錘" },
        { "& War Hammer~", "戰鎚" },
        { "& Spear~", "長矛" },
        { "& Trident~", "三叉戟" },
        { "& Halberd~", "戟" },
        { "& Pike~", "長槍" },
        { "& Sling~", "投石索" },
        { "& Short Bow~", "短弓" },
        { "& Long Bow~", "長弓" },
        { "& Arrow~", "箭矢" },
        { "& Bolt~", "弩箭" },
        { "& Shovel~", "鏟子" },
        { "& Pick~", "十字鎬" },
        { "& Pair~ of Leather Boots", "皮靴" },
        { "& Metal Cap~", "金屬帽" },
        { "& Hard Leather Cap~", "硬皮帽" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_ITEM_KINDS; ++i) {
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    }
    return NULL;
}

#endif
