/* User-facing spell effect info labels only.  The canonical effect_info()
 * strings are also used to classify damage, so never translate game data.
 * Format templates retain exactly the same printf conversion specifiers. */
#ifndef ANGDROID_ZH_TW_SPELL_EFFECT_INFO_H
#define ANGDROID_ZH_TW_SPELL_EFFECT_INFO_H

#include <stddef.h>
#include <string.h>

static inline const char *angdroid_zh_tw_spell_effect_info(
    const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "hurt", "自身傷害" },
        { "heal", "治療" },
        { "dam", "傷害" },
        { "dur", "持續" },
        { "power", "威力" },
        { "range", "距離" },
        { "blows", "攻擊次數" },
        { "random", "隨機" },
        { ", rad %d", ", 半徑 %d" },
        { ", rad 2", ", 半徑 2" },
        { "rad 2", "半徑 2" },
        { ", len %d", ", 長度 %d" },
    };
    size_t i;

    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i) {
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    }
    return english;
}

#endif
