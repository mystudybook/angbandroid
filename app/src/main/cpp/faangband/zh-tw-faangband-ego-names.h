/* FAangband-only ego names: display-time Traditional Chinese lookup.
 * Exact source spellings are taken from FAangband gamedata/ego_item.txt.
 * English names, ego state, rules and saved games are never modified.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES_H
#define ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES 66
static inline const char *angdroid_zh_tw_faangband_ego_name(const char *english)
{
    static const struct { const char *english, *traditional; }
        names[ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES] = {
        { "of Doriath", "多瑞亞斯" },
        { "of the Noldor", "諾多精靈" },
        { "of Nogrod", "諾格羅德" },
        { "of the Balrog", "炎魔" },
        { "of Melting", "熔化" },
        { "of Shocking", "電擊" },
        { "of Burning", "灼燒" },
        { "of Freezing", "冰凍" },
        { "of Poisoning", "淬毒" },
        { "of the Hunter", "獵人" },
        { "of Dread", "恐懼" },
        { "of the Drûgs", "德魯格族" },
        { "of Sneaking", "潛行" },
        { "of *Slay* Animal", "強力屠獸" },
        { "of *Slay* Evil", "強力誅邪" },
        { "of *Slay* Undead", "強力驅亡" },
        { "of *Slay* Demon", "強力斬魔" },
        { "of *Slay* Orc", "強力屠半獸人" },
        { "of *Slay* Troll", "強力屠食人妖" },
        { "of *Slay* Giant", "強力屠巨人" },
        { "of *Slay* Dragon", "強力屠龍" },
        { "of Angband", "安格班" },
        { "of Velocity", "迅捷" },
        { "of Ossiriand", "歐西瑞安德" },
        { "of Hurt Animal", "傷獸" },
        { "of Hurt Evil", "傷邪" },
        { "of Hurt Undead", "傷亡靈" },
        { "of Hurt Demon", "傷惡魔" },
        { "of Hurt Dragon", "傷龍" },
        { "of *Hurt* Evil", "強力傷邪" },
        { "of Electricity", "電能" },
        { "of Poison", "毒素" },
        { "of Kicking", "踢擊" },
        { "of Trollkind", "食人妖族" },
        { "of the Shadow Realm", "暗影國度" },
        { "of Anchoring", "錨定" },
        { "(Wolfhide)", "（狼皮製）" },
        { "of Ered Engrin", "鐵山" },
        { "of Desirability", "魅力" },
        { "of Shard Protection", "碎片防護" },
        { "of Enveloping", "包覆" },
        { "of Magic Mastery", "魔法精通" },
        { "(Reinforced)", "（強化）" },
        { "(Coated)", "（覆層）" },
        { "of the Earth", "大地" },
        { "of Mental Strength", "心智力量" },
        { "of Doom", "厄運" },
        { "of Basic Resistance", "基礎抗性" },
        { "of Magical Item Mastery", "魔法道具精通" },
        { "of Clarity", "清明" },
        { "of the Shadows", "暗影" },
        { "of Metamorphosis", "變形" },
        { "of Sustenance", "維生" },
        { "of Trickery", "詭計" },
        { "of Weaponmastery", "武器精通" },
        { "of Vitality", "生命力" },
        { "of Insight", "洞察" },
        { "of the Elements", "元素" },
        { "of Physical Prowess", "體能" },
        { "of Mobility", "靈活" },
        { "of Arcane Resistance", "奧術抗性" },
        { "of Utility", "實用" },
        { "of Hindrance", "阻礙" },
        { "of the Dawn", "黎明" },
        { "of Woe", "悲厄" },
        { "of Fickleness", "善變" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES; ++i)
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    return NULL;
}
#endif /* ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES_H */
