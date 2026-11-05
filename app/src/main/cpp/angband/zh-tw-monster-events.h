/* Display-only monster allocation and theft labels. */
#ifndef ANGDROID_ZH_TW_MONSTER_EVENTS_H
#define ANGDROID_ZH_TW_MONSTER_EVENTS_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_monster_event(const char *en, int zh_tw)
{
    static const struct { const char *en, *zh; } pairs[] = {
        { "You vanish into the shadows!", "你消失在陰影之中！" },
        { "treasure", "財寶" },
        { "Compacting monsters...", "正在整理怪物資料……" },
        { "Too many monsters!", "怪物數量過多！" },
        { "Warning! Could not allocate a new monster.", "警告！無法配置新的怪物。" },
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof(pairs) / sizeof(pairs[0]); ++i)
        if (strcmp(en, pairs[i].en) == 0) return pairs[i].zh;
    return en;
}
#endif
