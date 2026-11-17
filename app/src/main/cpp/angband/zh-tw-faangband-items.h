/* Supplementary DISPLAY translations for FAangband-only object.txt kinds.
 * Canonical names, tvals, gameplay, savefiles and flavor data are untouched.
 * The standard Angband catalog is defined separately in zh-tw-items.h.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_ITEMS_H
#define ANGDROID_ZH_TW_FAANGBAND_ITEMS_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_FAANGBAND_KINDS 39

static inline const char *angdroid_zh_tw_faangband_item_kind_name(
    const char *english, const char *tval)
{
    static const struct { const char *english, *tval, *traditional; }
        names[ANGDROID_ZH_TW_FAANGBAND_KINDS] = {
        /* Weapons, armour and chests unique to FAangband. */
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
        /* Rings and amulets are named for their material in FAangband. */
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
        /* Wand and rod effects keep a distinct device noun. */
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
    if (!english || !tval) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_KINDS; ++i)
        if (strcmp(english, names[i].english) == 0 &&
            strcmp(tval, names[i].tval) == 0)
            return names[i].traditional;
    return NULL;
}

#endif /* ANGDROID_ZH_TW_FAANGBAND_ITEMS_H */
