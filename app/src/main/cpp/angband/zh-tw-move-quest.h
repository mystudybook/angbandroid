/* Native Angband monster movement / quest UI labels. No gameplay data changes. */
#ifndef ANGDROID_ZH_TW_MOVE_QUEST_H
#define ANGDROID_ZH_TW_MOVE_QUEST_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_move_quest_text(const char *en, int zh_tw)
{
    static const struct { const char *en, *zh; } map[] = {
        { "stumbles", "踉蹌了一下" },
        { "bangs into a door", "撞上了門" },
        { "bashes into a wall", "撞上了牆壁" },
        { "bumps into some rocks", "撞上了岩石" },
        { "tramples over", "踐踏了" },
        { "pushes past", "擠過了" },
        { "You hear a door burst open!", "你聽見門被撞開的聲音！" },
        { "The rune of protection is broken!", "防護符文被破壞了！" },
        { "A magical staircase appears...", "一道魔法階梯出現了……" },
        { "*** CONGRATULATIONS ***", "＊＊＊ 恭喜你 ＊＊＊" },
        { "You have won the game!", "你贏得了這場遊戲！" },
        { "You may retire (commit suicide) when you are ready.", "準備好之後，你可以選擇退隱（結束角色生命）以完成冒險。" },
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(en, map[i].en) == 0) return map[i].zh;
    return en;
}
#endif
