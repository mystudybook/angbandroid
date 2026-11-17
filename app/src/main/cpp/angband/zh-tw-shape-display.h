/* Screen-only shape names used by Angband and FAangband.
 * Keep shape.txt names, parser IDs, effects, and save data in English.
 * The existing FAangband-only mapping covers 14 extra variants.
 */
#ifndef ANGDROID_ZH_TW_SHAPE_DISPLAY_H
#define ANGDROID_ZH_TW_SHAPE_DISPLAY_H

#include <stddef.h>
#include <string.h>
#include "zh-tw-faangband-shapes.h"

static inline const char *angdroid_shape_display_name(
    const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } common[] = {
        { "normal", "正常" },
        { "fox", "狐狸" },
        { "Pukel-man", "普克爾人" },
        { "bear", "熊" },
        { "eagle", "老鷹" },
        { "bat", "蝙蝠" },
        { "warg", "座狼" },
        { "vampire", "吸血鬼" },
        { "werewolf", "狼人" },
    };
    size_t i;

    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(common) / sizeof(common[0]); ++i)
        if (strcmp(english, common[i].en) == 0) return common[i].zh;

    return angdroid_faangband_shape_name(english, zh_tw);
}
#endif
