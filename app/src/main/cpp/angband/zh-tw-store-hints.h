/* Presentation-only Traditional Chinese hints used for shopkeeper tips.
 * Source hint data, random selection and command keys remain canonical.
 */
#ifndef ANGDROID_ZH_TW_STORE_HINTS_H
#define ANGDROID_ZH_TW_STORE_HINTS_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_STORE_HINTS 20
static inline const char *angdroid_store_hint_text(const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; }
        hints[ANGDROID_ZH_TW_STORE_HINTS] = {
        { "Cure Critical Wounds potions will cure blindness and confusion; carry lots.", "「治療重傷」藥水能解除失明和混亂，建議多帶幾瓶。" },
        { "Staves can be used even when confused and blinded.", "即使混亂或失明，仍然可以使用法杖。" },
        { "Monsters that breathe fire, acid, electricity, or cold can deal big damage.", "會噴吐火焰、強酸、閃電或寒氣的怪物可能造成大量傷害。" },
        { "Resistances cut damage to 1/3rd for the four basic elements.", "對四種基本元素的抗性，可將相關傷害降至約三分之一。" },
        { "The more hitpoints a monster has, the more damage its breath will do.", "怪物的生命值越高，吐息造成的傷害通常越大。" },
        { "You can combine equipment of Resist Fire and a potion of the same name.", "可同時使用抗火裝備和同名藥水來強化防護。" },
        { "You can combine equipment of Resist Cold and a potion of the same name.", "可同時使用抗寒裝備和同名藥水來強化防護。" },
        { "You can combine equipment of Resist Poison and a potion of the same name.", "可同時使用抗毒裝備和同名藥水來強化防護。" },
        { "If you don't know what a monster can do, use a rod of probing on it.", "不清楚怪物能力時，可以使用探查魔棒了解牠。" },
        { "Inscribe items with the '{' command.", "按 { 指令可為物品加上刻字。" },
        { "Inscribe an item with \"!s\" to get confirmation before selling it.", "刻上 !s，可在出售物品前要求確認。" },
        { "Inscribe an item with \"!d\" to get confirmation before dropping it.", "刻上 !d，可在丟棄物品前要求確認。" },
        { "Inscribe an item with \"!k\" to get confirmation before destroying it.", "刻上 !k，可在摧毀物品前要求確認。" },
        { "Inscribe an item with \"!v\" to get confirmation before throwing it.", "刻上 !v，可在投擲物品前要求確認。" },
        { "Inscribe an item with \"!*\" to get confirmation before doing anything to it.", "刻上 !*，對物品進行任何操作前都會要求確認。" },
        { "You can use the 'I' command to inspect items and get details on them.", "按 I 可查看物品的詳細資訊。" },
        { "Use '=' to check out the options and customize your game.", "按 = 可開啟選項並調整遊戲設定。" },
        { "Be sure to set your hitpoint warning in the options menu ('=').", "別忘了在 = 選項中設定低生命值警告。" },
        { "Flasks of oil make a good throwing weapon early on.", "遊戲初期，油瓶也是不錯的投擲武器。" },
        { "Word of Recall will send you to and from the dungeon.", "「召回術」可讓你往返地城與城鎮。" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < ANGDROID_ZH_TW_STORE_HINTS; ++i)
        if (strcmp(english, hints[i].en) == 0) return hints[i].zh;
    return english;
}
#endif /* ANGDROID_ZH_TW_STORE_HINTS_H */
