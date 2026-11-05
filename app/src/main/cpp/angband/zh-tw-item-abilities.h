/* Display-only descriptions from object_property.txt.
 * Stable property IDs and gameplay flags are never translated.
 */
#ifndef ANGDROID_ZH_TW_ITEM_ABILITIES_H
#define ANGDROID_ZH_TW_ITEM_ABILITIES_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_zh_tw_item_ability(const char *english,
    int zh_tw)
{
    static const struct { const char *en; const char *zh; } entries[] = {
        { "fear", "恐懼" },
        { "blindness", "失明" },
        { "confusion", "混亂" },
        { "stunning", "暈眩" },
        { "Slows your metabolism", "減緩新陳代謝" },
        { "Feather Falling", "羽落術：減輕墜落影響" },
        { "Speeds regeneration", "加速生命恢復" },
        { "Grants telepathy", "賦予心靈感應能力" },
        { "Grants the ability to see invisible things", "讓你看見隱形事物" },
        { "Prevents paralysis", "防止麻痺" },
        { "Sustains your life force", "保護你的生命能量" },
        { "Sometimes creates earthquakes on impact", "命中時有時會引發地震" },
        { "Blessed by the gods (combat bonuses for holy casters)", "受到眾神祝福，神聖施法者可獲得戰鬥加成" },
        { "Impairs hitpoint recovery", "妨礙生命值恢復" },
        { "Impairs mana recovery", "妨礙法力恢復" },
        { "Makes you afraid of melee, and worse at shooting and casting spells", "使你害怕近戰，並降低射擊和施法能力" },
        { "Prevents teleportation", "阻止瞬間傳送" },
        { "Aggravates creatures nearby", "激怒附近的生物" },
        { "Drains experience", "吸取經驗值" },
        { "Can't be removed", "無法卸下" },
        { "Can be destroyed if you attempt to remove its curses", "嘗試解除詛咒時可能摧毀物品" },
        { "Gives immunity to traps", "免疫陷阱" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(entries) / sizeof(entries[0]); ++i)
        if (strcmp(english, entries[i].en) == 0) return entries[i].zh;
    return english;
}
#endif
