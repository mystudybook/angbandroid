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

#define ANGDROID_ZH_TW_ITEM_KINDS 76

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
        { "& Main~ Gauche~", "副手短劍" },
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
        { "& Pair~ of Mithril Shod Boots", "秘銀護靴" },
        { "& Pair~ of Ethereal Slippers", "以太拖鞋" },
        { "& Iron Helm~", "鐵盔" },
        { "& Steel Helm~", "鋼盔" },
        { "& Iron Crown~", "鐵冠" },
        { "& Golden Crown~", "金冠" },
        { "& Jewel Encrusted Crown~", "寶石鑲嵌王冠" },
        { "& Robe~", "長袍" },
        { "Soft Leather Armour~", "軟皮甲" },
        { "Studded Leather Armour~", "鑲釘皮甲" },
        { "Hard Leather Armour~", "硬皮甲" },
        { "Leather Scale Mail~", "鱗片皮甲" },
        { "Metal Scale Mail~", "金屬鱗甲" },
        { "Chain Mail~", "鎖子甲" },
        { "Augmented Chain Mail~", "強化鎖子甲" },
        { "Bar Chain Mail~", "條狀鎖子甲" },
        { "Metal Brigandine Armour~", "金屬札甲" },
        { "Partial Plate Armour~", "半身板甲" },
        { "Metal Lamellar Armour~", "金屬片甲" },
        { "Full Plate Armour~", "全身板甲" },
        { "Ribbed Plate Armour~", "肋條板甲" },
        { "Mithril Chain Mail~", "秘銀鎖子甲" },
        { "Mithril Plate Mail~", "秘銀板甲" },
        { "Adamantite Plate Mail~", "精金板甲" },
        { "& Cloak~", "披風" },
        { "& Fur Cloak~", "毛皮披風" },
        { "& Elven Cloak~", "精靈披風" },
        { "& Ethereal Cloak~", "以太披風" },
        { "& Set~ of Leather Gloves", "皮手套" },
        { "& Set~ of Gauntlets", "護手" },
        { "& Set~ of Mithril Gauntlets", "秘銀護手" },
        { "& Set~ of Caestus", "拳鬥手套" },
        { "& Set~ of Alchemist's Gloves", "鍊金術士手套" },
        { "& Wicker Shield~", "藤編盾" },
        { "& Small Metal Shield~", "小型金屬盾" },
        { "& Leather Shield~", "皮盾" },
        { "& Large Metal Shield~", "大型金屬盾" },
        { "& Knight's Shield~", "騎士盾" },
        { "& Mithril Shield~", "秘銀盾" },
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
