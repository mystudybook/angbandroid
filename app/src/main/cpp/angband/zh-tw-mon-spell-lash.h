/* Original projection.txt lash-desc: values, display-only localization. */
#ifndef ANGDROID_ZH_TW_MON_SPELL_LASH_H
#define ANGDROID_ZH_TW_MON_SPELL_LASH_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_mon_spell_lash(const char *english,
    int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "acid", "酸液" },
        { "lightning", "閃電" },
        { "fire", "火焰" },
        { "frost", "冰霜" },
        { "venom", "毒液" },
        { "brightness", "耀眼光芒" },
        { "blackness", "黑暗" },
        { "sound", "聲波" },
        { "razors", "利刃" },
        { "dislocation", "空間錯位" },
        { "withering", "枯萎之力" },
        { "confusion", "混亂" },
        { "unmagic", "破除魔法" },
        { "water", "水流" },
        { "icicles", "冰錐" },
        { "gravity", "重力" },
        { "lassitude", "遲滯之力" },
        { "impact", "強烈衝擊" },
        { "ruination", "毀滅之力" },
        { "living fire", "活生生的火焰" },
        { "meteoric iron", "隕鐵" },
        { "raw magic", "純粹魔力" },
        { "holy power", "神聖力量" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
