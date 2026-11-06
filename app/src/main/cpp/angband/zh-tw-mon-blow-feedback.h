/* Original Angband monster melee and theft messages: display-only zh-TW. */
#ifndef ANGDROID_ZH_TW_MON_BLOW_FEEDBACK_H
#define ANGDROID_ZH_TW_MON_BLOW_FEEDBACK_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_mon_blow_feedback(const char *english, int zh_tw)
{
    static const struct { const char *en, *zh; } map[] = {
        { "You are covered in acid!", "酸液濺滿了你的全身！" },
        { "You are struck by electricity!", "你遭到電流電擊！" },
        { "You are enveloped in flames!", "火焰包圍了你！" },
        { "You are covered with frost!", "你的全身覆滿了冰霜！" },
        { "You keep hold of your life force!", "你成功守住了生命力！" },
        { "You feel your life slipping away!", "你感覺生命力正逐漸流失！" },
        { "You feel your life draining away!", "你感覺生命力正在被抽走！" },
        { "Energy drains from your pack!", "你的背包裡有能量被吸走了！" },
        { "You quickly protect your money pouch!", "你迅速護住了錢袋！" },
        { "Nothing was stolen.", "沒有任何東西被偷走。" },
        { "Your purse feels lighter.", "你感覺錢袋變輕了。" },
        { "All of your coins were stolen!", "你的金幣全被偷走了！" },
        { "You grab hold of your backpack!", "你緊緊抓住了背包！" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
