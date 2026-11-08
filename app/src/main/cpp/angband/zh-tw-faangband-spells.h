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

static inline const char *angdroid_zh_tw_faangband_spell_description(
    const char *english_name, const char *english_description)
{
    const struct angdroid_fa_spell_display *entry =
        angdroid_zh_tw_faangband_spell_lookup(english_name);
    if (!entry || !english_description ||
        strcmp(english_description, entry->english_description) != 0)
        return NULL;
    return entry->traditional_description;
}

#endif /* ANGDROID_ZH_TW_FAANGBAND_SPELLS_H */
