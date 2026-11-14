/* Display-only extensions for FAangband spell names and exact descriptions.
 * Keep canonical class.txt identifiers, spell effects, and game data intact.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_SPELLS_H
#define ANGDROID_ZH_TW_FAANGBAND_SPELLS_H
#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_FAANGBAND_SPELL_COUNT 2
struct angdroid_fa_spell_display {
    const char *english_name;
    const char *english_description;
    const char *traditional_name;
    const char *traditional_description;
};

static inline const struct angdroid_fa_spell_display *
angdroid_zh_tw_faangband_spell_lookup(const char *name)
{
    static const struct angdroid_fa_spell_display spells[
        ANGDROID_ZH_TW_FAANGBAND_SPELL_COUNT] = {
        { "Timeless Blink",
          "Teleports you to a square of your choice within your line of sight.  This spell takes no time.",
          "無時瞬移",
          "傳送至視線內你所選擇的位置。此法術不消耗時間。" },
        { "Fume of Thangorodrim",
          "Darkens, maps out, and detects all objects on the complete current dungeon level.",
          "桑戈洛錐姆之煙",
          "使目前整個地城樓層陷入黑暗、繪製地圖，並偵測該樓層的所有物品。" },
    };
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_SPELL_COUNT; ++i)
        if (strcmp(name, spells[i].english_name) == 0)
            return &spells[i];
    return NULL;
}

static inline const char *angdroid_zh_tw_faangband_spell_name(
    const char *english_name)
{
    const struct angdroid_fa_spell_display *entry =
        angdroid_zh_tw_faangband_spell_lookup(english_name);
    return entry ? entry->traditional_name : NULL;
}

/* The common spell identifiers below have DIFFERENT descriptions in
 * FAangband class.txt than in Angband class.txt.  Match the complete source
 * text so a gameplay or numerical change cannot reuse an obsolete summary.
 */
#define ANGDROID_ZH_TW_FAANGBAND_VARIANT_DESC_COUNT 2

static inline const char *angdroid_zh_tw_faangband_variant_description(
    const char *english_name, const char *english_description)
{
    static const struct { const char *name, *english, *traditional; }
        variants[ANGDROID_ZH_TW_FAANGBAND_VARIANT_DESC_COUNT] = {
        { "Word of Recall",
          "Teleports you from the dungeon or wilderness to the town, or from the town to any of four points you have visited and set.  The recall effect is not immediate; it is delayed by 14+1d20 turns.  During that delay, the prayer can be canceled by invoking the prayer of recall again.  This prayer has no effect when the option to restrict the use of stairs and recall is set, unless Morgoth is dead.",
          "從地城或荒野返回城鎮，或從城鎮傳送至曾到訪並設定的四個地點之一。召回效果將在 14+1d20 回合後生效；等待期間再次施放召回祈禱可以取消。若啟用了限制樓梯與召回的選項，除非魔苟斯已死亡，否則此祈禱無效。" },
        { "Berserk Strength",
          "Removes fear and grants you berserk strength (resistance to fear, +72 melee skill, -10 to AC, and prevents healing over time) for a level-dependent duration.",
          "解除恐懼並獲得狂暴力量：抵抗恐懼、近戰技能提高 72 點、防禦等級（AC）降低 10 點，且期間無法隨時間自然恢復生命值。持續時間取決於角色等級。" },
    };
    size_t i;
    if (!english_name || !english_description) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_VARIANT_DESC_COUNT; ++i)
        if (strcmp(english_name, variants[i].name) == 0 &&
            strcmp(english_description, variants[i].english) == 0)
            return variants[i].traditional;
    return NULL;
}

static inline const char *angdroid_zh_tw_faangband_spell_description(
    const char *english_name, const char *english_description)
{
    const struct angdroid_fa_spell_display *entry =
        angdroid_zh_tw_faangband_spell_lookup(english_name);
    if (!entry || !english_description ||
        strcmp(english_description, entry->english_description) != 0)
        return angdroid_zh_tw_faangband_variant_description(
            english_name, english_description);
    return entry->traditional_description;
}

#endif /* ANGDROID_ZH_TW_FAANGBAND_SPELLS_H */
