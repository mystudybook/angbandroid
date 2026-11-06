/* Display-only translations for original Angband projection feedback.
 * Never modify projections, resistance math, status names or monster records.
 */
#ifndef ANGDROID_ZH_TW_PROJECTION_FEEDBACK_H
#define ANGDROID_ZH_TW_PROJECTION_FEEDBACK_H
#include <stddef.h>
#include <string.h>

struct angdroid_project_feedback_pair { const char *en; const char *zh; };

static inline const char *angdroid_project_feedback(const char *english, int zh_tw)
{
    static const struct angdroid_project_feedback_pair map[] = {
        { "The intense heat saps you.", "強烈的熱氣讓你虛弱不堪。" },
        { "Your eyes fill with smoke!", "濃煙灌進你的雙眼！" },
        { "You are assailed by poisonous fumes!", "有毒煙霧向你襲來！" },
        { "The cold seeps into your bones.", "寒氣滲入你的骨髓。" },
        { "The cold withers your life force!", "嚴寒正在侵蝕你的生命力！" },
        { "You resist the effect!", "你抵抗了這次效果！" },
        { "The venom stings your skin!", "毒液刺痛了你的皮膚！" },
        { "The stench sickens you.", "惡臭讓你感到噁心。" },
        { "You are dazzled!", "強光讓你眼花撩亂！" },
        { "The darkness steals your life force!", "黑暗奪走了你的生命力！" },
        { "You feel unsure of yourself in the darkness.", "你在黑暗中感到不安。" },
        { "Darkness penetrates your mind!", "黑暗侵入了你的心智！" },
        { "The noise disorients you.", "刺耳的聲響讓你失去方向感。" },
        { "You avoid the effect!", "你避開了這次效果！" },
        { "You feel your life force draining away!", "你感覺生命力正在流失！" },
        { "Your mind is dulled.", "你的思緒變得遲鈍。" },
        { "Your energy is sapped!", "你的精力被抽走了！" },
        { "Gravity warps around you.", "你周圍的重力發生扭曲。" },
        { "The rubble turns into mud!", "碎石堆化成了泥漿！" },
        { "There was something buried in the rubble!", "碎石堆裡原來埋著東西！" },
        { "The door turns into mud!", "門化成了泥漿！" },
        { "The vein turns into mud!", "礦脈化成了泥漿！" },
        { "You have found something!", "你發現了某樣東西！" },
        { "The wall turns into mud!", "牆壁化成了泥漿！" },
        { "There is a bright flash of light!", "一道耀眼的光芒閃過！" },
        { "The trap seizes up.", "陷阱卡住不動了。" },
        { "Click!", "喀噠！" },
        { "You come to rest next to a wall.", "你在牆邊停了下來。" },
        { "You are thrown into molten lava!", "你被拋進滾燙的熔岩中！" },
        { "You're not as powerful as you used to be...", "你感覺自己不再像以前那麼強大……" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}

static inline const char *angdroid_project_stat(const char *english, int zh_tw)
{
    static const struct angdroid_project_feedback_pair map[] = {
        { "strong", "力量" },
        { "bright", "智力" },
        { "wise", "智慧" },
        { "agile", "敏捷" },
        { "hale", "體魄" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}

static inline const char *angdroid_project_blind_term(const char *english, int zh_tw)
{
    static const struct angdroid_project_feedback_pair map[] = {
        { "acid", "酸液" },
        { "lightning", "閃電" },
        { "fire", "火焰" },
        { "cold", "寒氣" },
        { "poison", "毒素" },
        { "something", "不明力量" },
        { "noise", "聲波" },
        { "something sharp", "尖銳物體" },
        { "something strange", "奇異力量" },
        { "something cold", "寒冷之物" },
        { "water", "水流" },
        { "something hard", "堅硬物體" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}

#endif
