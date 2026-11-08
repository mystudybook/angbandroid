/* Presentation-only Angband shape knowledge. Never mutate canonical shape
 * records, gameplay effects, class/spell keys, menu order or save data.
 */
#ifndef ANGDROID_ZH_TW_SHAPE_LORE_H
#define ANGDROID_ZH_TW_SHAPE_LORE_H
#include <stddef.h>
#include <string.h>
#include "zh-tw-object-info.h"
#include "zh-tw-faangband-shapes.h"

struct angdroid_shape_pair { const char *en; const char *zh; };

static inline const char *angdroid_shape_lookup(
    const char *english, int zh_tw, const struct angdroid_shape_pair *pairs,
    size_t count)
{
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < count; ++i)
        if (strcmp(english, pairs[i].en) == 0) return pairs[i].zh;
    return english;
}

static inline const char *angdroid_shape_name(const char *english, int zh_tw)
{
    static const struct angdroid_shape_pair map[] = {
        { "normal", "一般形態" },
        { "fox", "狐狸" },
        { "Pukel-man", "普克爾人" },
        { "bear", "熊" },
        { "eagle", "鷹" },
        { "bat", "蝙蝠" },
        { "warg", "座狼" },
        { "vampire", "吸血鬼" },
        { "werewolf", "狼人" },
    };
    const char *translated = angdroid_shape_lookup(english, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
    return translated != english ? translated :
        angdroid_faangband_shape_name(english, zh_tw);
}

static inline const char *angdroid_shape_class(const char *english, int zh_tw)
{
    static const struct angdroid_shape_pair map[] = {
        { "Warrior", "戰士" },
        { "Mage", "法師" },
        { "Druid", "德魯伊" },
        { "Priest", "祭司" },
        { "Necromancer", "死靈法師" },
        { "Paladin", "聖騎士" },
        { "Rogue", "盜賊" },
        { "Ranger", "遊俠" },
        { "Blackguard", "黑衛士" },
    };
    return angdroid_shape_lookup(english, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
}

static inline const char *angdroid_shape_term(const char *english, int zh_tw)
{
    static const struct angdroid_shape_pair map[] = {
        { "physical disarming", "物理解除陷阱" },
        { "magical disarming", "魔法解除陷阱" },
        { "magic devices", "魔法裝置" },
        { "saving throws", "豁免判定" },
        { "searching", "搜索" },
        { "melee to hit", "近戰命中" },
        { "shooting to hit", "遠程命中" },
        { "throwing to hit", "投擲命中" },
        { "digging", "挖掘" },
        { "unknown skill", "未知技能" },
        { "Adds", "增加" },
        { "Makes you vulnerable to", "使你更容易受到以下傷害：" },
        { "Makes you resistant to", "使你具有以下抗性：" },
        { "Makes you immune to", "使你免疫以下傷害：" },
        { "Provides protection from", "提供以下防護：" },
        { "Sustains", "維持以下能力：" },
    };
    const char *mapped;
    if (!english || !zh_tw) return english;
    mapped = angdroid_shape_lookup(english, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
    if (mapped != english) return mapped;
    return angdroid_zh_tw_object_term(english, zh_tw);
}

static inline const char *angdroid_shape_intro(int zh_tw)
{
    if (zh_tw)
        return "\n和所有變身形態一樣，變身當下的裝備會決定基礎屬性，包括每次攻擊的傷害、攻擊次數及抗性。"
            "變身期間，無法操作背包或地上的物品（撿起與吃東西除外）。"
            "想恢復原本形態，可以施放法術，或使用吃東西以外的物品指令（例如丟棄）。\n";
    return "\nLike all shapes, the equipment at the time of "
        "the shapechange sets the base attributes, including damage "
        "per blow, number of blows and resistances.  While changed, "
        "items in your pack or on the floor (except for pickup or "
        "eating) are inaccessible.  To switch back to your normal "
        "shape, cast a spell or use an item command other than eat "
        "(drop, for instance).\n";
}

/* Source keys are the exact desc: text of flags used by shape.txt. Other
 * object/player abilities retain their original English description. */
static inline const char *angdroid_shape_misc_desc(const char *english, int zh_tw)
{
    static const struct angdroid_shape_pair map[] = {
        { "Feather Falling", "緩慢墜落" },
        { "Prevents paralysis", "防止麻痺" },
        { "Speeds regeneration", "加快生命恢復" },
        { "Sustains your life force", "維持生命力" },
        { "Grants the ability to see invisible things", "能看見隱形事物" },
        { "Gives immunity to traps", "免疫陷阱效果" },
        { "You are made of rock.", "你的身體由岩石構成。" },
    };
    const char *translated = angdroid_shape_lookup(english, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
    return translated != english ? translated :
        angdroid_faangband_shape_ability_desc(english, zh_tw);
}
#endif
