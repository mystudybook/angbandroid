/* Display-only player inventory, equipment and spell-status feedback.
 * No game-state or canonical spell/realm identifier changes. */
#ifndef ANGDROID_ZH_TW_PLAYER_CALCS_FEEDBACK_H
#define ANGDROID_ZH_TW_PLAYER_CALCS_FEEDBACK_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_player_calcs_feedback(const char *english,
    int zh_tw)
{
    static const struct { const char *en, *zh; } map[] = {
        { "You re-arrange your quiver.", "你重新整理了箭袋。" },
        { "You re-arrange your pack.", "你重新整理了背包。" },
        { "You have trouble wielding such a heavy bow.", "這把弓太重了，讓你難以操作。" },
        { "You have no trouble wielding your bow.", "你現在可以輕鬆使用這把弓。" },
        { "You feel relieved to put down your heavy bow.", "放下沉重的弓後，你感到輕鬆多了。" },
        { "You have trouble wielding such a heavy weapon.", "這把武器太重了，讓你難以揮舞。" },
        { "You have no trouble wielding your weapon.", "你現在可以輕鬆揮舞武器。" },
        { "You feel relieved to put down your heavy weapon.", "放下沉重的武器後，你感到輕鬆多了。" },
        { "You feel attuned to your weapon.", "你感覺自己與武器更加契合。" },
        { "You feel less attuned to your weapon.", "你感覺自己與武器不再那麼契合。" },
        { "The weight of your armor encumbers your movement.", "護甲的重量妨礙了你的行動。" },
        { "You feel able to move more freely.", "你感覺行動變得更加自如。" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
