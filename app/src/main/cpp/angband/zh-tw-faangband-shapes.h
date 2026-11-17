/* FAangband-only shape names and flying ability description, for UI only.
 * Never change shape identifiers, combat flags, breath codes or save data.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_SHAPES_H
#define ANGDROID_ZH_TW_FAANGBAND_SHAPES_H
#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_FAANGBAND_SHAPE_COUNT 14

static inline const char *angdroid_faangband_shape_name(
    const char *english, int zh_tw)
{
    static const struct { const char *english, *traditional; }
        shapes[ANGDROID_ZH_TW_FAANGBAND_SHAPE_COUNT] = {
        { "bear cub", "小熊" },
        { "great bear", "巨熊" },
        { "black dragon", "黑龍" },
        { "blue dragon", "藍龍" },
        { "white dragon", "白龍" },
        { "red dragon", "紅龍" },
        { "green dragon", "綠龍" },
        { "shining dragon", "輝光龍" },
        { "law dragon", "秩序龍" },
        { "chaos dragon", "混沌龍" },
        { "gold dragon", "金龍" },
        { "multi-hued dragon", "多彩龍" },
        { "balance dragon", "平衡龍" },
        { "power dragon", "威能龍" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_SHAPE_COUNT; ++i)
        if (strcmp(english, shapes[i].english) == 0)
            return shapes[i].traditional;
    return english;
}

/* Exact player_property.txt display description for the FLYING flag. */
static inline const char *angdroid_faangband_shape_ability_desc(
    const char *english, int zh_tw)
{
    if (!english || !zh_tw) return english;
    return strcmp(english, "You can fly.") == 0 ? "你能飛行。" : english;
}

#endif /* ANGDROID_ZH_TW_FAANGBAND_SHAPES_H */
