/* Display-only translation for the Angband 4.2.4 object.txt msg and
 * vis-msg fields. Keep gamedata, item IDs, effects and RNG untouched.
 * Unknown or future messages always fall back to their original text.
 */
#ifndef ANGDROID_ZH_TW_ITEM_USE_MESSAGES_H
#define ANGDROID_ZH_TW_ITEM_USE_MESSAGES_H

#include <stddef.h>
#include <string.h>

static inline const char *angdroid_zh_tw_item_use_message(
    const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "That tastes good.", "味道不錯。" },
        { "That tastes better than nothing.", "總比什麼都不吃好。" },
        { "That tastes disgusting.", "味道真噁心。" },
        { "Delicious!", "真美味！" },
        { "That tastes great.", "味道很棒。" },
        { "That tastes great!", "味道真棒！" },
        { "That tastes great!  A fine vintage.", "味道極佳！真是上等佳釀。" },
        { "You feel your heart warm.", "你感覺心頭一暖。" },
        { "It burns your throat.", "它灼燒著你的喉嚨。" },
        { "Refreshing!", "清爽極了！" },
        { "Your stomach heaves.", "你感到胃裡一陣翻騰。" },
        { "There is a high pitched humming noise.", "傳來一陣尖銳的嗡鳴聲。" },
        { "You hear a low-pitched whistling sound.", "你聽見一陣低沉的哨音。" },
        { "You feel life flow through your body!", "你感覺生命力流遍全身！" },
        { "The potion makes you queasy.", "這瓶藥水讓你反胃。" },
        { "A line of shimmering blue light appears.", "一道閃爍的藍光出現了。" },
    };
    size_t i;

    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i) {
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    }
    return english;
}
#endif
