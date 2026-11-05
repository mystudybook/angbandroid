/* UI-only translations of the 79 distinct canonical ego-item labels.
 * Ego records themselves have 107 entries; repeated labels share one key.
 * No save data, identification state, or English fallback is modified.
 */
#ifndef ANGDROID_ZH_TW_EGO_NAMES_H
#define ANGDROID_ZH_TW_EGO_NAMES_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_EGO_NAMES 79
static inline const char *angdroid_zh_tw_ego_name(const char *english)
{
    static const struct {
        const char *english;
        const char *traditional;
    } names[ANGDROID_ZH_TW_EGO_NAMES] = {
        { "of Shadows", "暗影" },
        { "of Brightness", "光輝" },
        { "(Everburning)", "（永燃）" },
        { "of True Sight", "真視" },
        { "(Holy Avenger)", "（神聖復仇者）" },
        { "(Defender)", "（守護者）" },
        { "(Blessed)", "（祝福）" },
        { "of Gondolin", "貢多林之力" },
        { "of Westernesse", "西方之力" },
        { "of Extra Attacks", "追加攻擊" },
        { "of Fury", "狂怒" },
        { "of Acid", "酸蝕" },
        { "of Lightning", "雷電" },
        { "of Flame", "火焰" },
        { "of Frost", "冰霜" },
        { "of Venom", "劇毒" },
        { "of Slay Animal", "屠獸" },
        { "of Slay Evil", "誅邪" },
        { "of Slay Undead", "除不死" },
        { "of Slay Demon", "斬惡魔" },
        { "of Slay Orc", "殺半獸人" },
        { "of Slay Troll", "屠食人妖" },
        { "of Slay Giant", "斬巨人" },
        { "of Slay Dragon", "屠龍" },
        { "of *Slay Animal*", "強效屠獸" },
        { "of *Slay Evil*", "強效誅邪" },
        { "of *Slay Undead*", "強效除不死" },
        { "of *Slay Demon*", "強效斬惡魔" },
        { "of *Slay Orc*", "強效殺半獸人" },
        { "of *Slay Troll*", "強效屠食人妖" },
        { "of *Slay Giant*", "強效斬巨人" },
        { "of *Slay Dragon*", "強效屠龍" },
        { "of Morgul", "魔窟" },
        { "of Accuracy", "精準" },
        { "of Power", "威能" },
        { "of Lothlórien", "羅斯洛立安" },
        { "of the Haradrim", "哈拉德林" },
        { "of Extra Might", "額外威力" },
        { "of Extra Shots", "追加射擊" },
        { "of Buckland", "巴克蘭" },
        { "of the Nazgûl", "戒靈" },
        { "of Holy Might", "神聖威力" },
        { "of Wounding", "重創" },
        { "of Backbiting", "反噬" },
        { "of Digging", "挖掘" },
        { "of Earthquakes", "地震" },
        { "of Slow Descent", "緩降" },
        { "of Stealth", "潛行" },
        { "of Free Action", "自由行動" },
        { "of Speed", "加速" },
        { "of Stability", "穩固" },
        { "of Elvenkind", "精靈族" },
        { "of Intelligence", "智力" },
        { "of Wisdom", "智慧" },
        { "of the Magi", "法師" },
        { "of Might", "力量" },
        { "of Lordliness", "尊貴" },
        { "of Seeing", "洞察" },
        { "of Infravision", "紅外視覺" },
        { "of Light", "光明" },
        { "of Telepathy", "心靈感應" },
        { "of Regeneration", "再生" },
        { "of Serenity", "寧靜" },
        { "of Night and Day", "晝夜" },
        { "of Resist Acid", "抗酸" },
        { "of Resist Lightning", "抗電" },
        { "of Resist Fire", "抗火" },
        { "of Resist Cold", "抗寒" },
        { "of Resistance", "全抗性" },
        { "of Permanence", "恆久" },
        { "(Dwarven)", "（矮人製）" },
        { "of Protection", "防護" },
        { "of Aman", "阿曼" },
        { "of Slaying", "殺戮" },
        { "of Agility", "敏捷" },
        { "of Thievery", "偷竊" },
        { "of Combat", "戰鬥" },
        { "of Preservation", "保存" },
        { "of Craftsmanship", "精工" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_EGO_NAMES; ++i)
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    return NULL;
}
#endif
