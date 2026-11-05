/* Presentation-only inventory and equipment status messages. */
#ifndef ANGDROID_ZH_TW_GEAR_MESSAGES_H
#define ANGDROID_ZH_TW_GEAR_MESSAGES_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_gear_message(const char *en, int zh_tw) {
    static const struct { const char *en; const char *zh; } map[] = {
        { "Mushrooms for breakfast!", "蘑菇早餐真美味！" },
        { "Oops! It feels deathly cold!", "糟糕！裝備傳來一股令人發寒的冰冷感！" },
        { "You combine some items in your pack.", "你把背包中的部分物品合併了。" },
        { "Your pack overflows!", "你的背包裝不下了！" },
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof(map)/sizeof(map[0]); ++i)
        if (strcmp(en, map[i].en) == 0) return map[i].zh;
    return en;
}
#endif
