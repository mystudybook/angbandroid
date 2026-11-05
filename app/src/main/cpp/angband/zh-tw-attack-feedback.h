/* Exact original Angband display descriptions. Never modify game data.
 * Unknown values and non-zh-TW callers retain their original English text.
 */
#ifndef ANGDROID_ZH_TW_ATTACK_FEEDBACK_H
#define ANGDROID_ZH_TW_ATTACK_FEEDBACK_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_attack_feedback(const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "You feel a little better.", "你感覺好了一點。" },
        { "You feel better.", "你感覺好多了。" },
        { "You feel much better.", "你感覺大幅好轉。" },
        { "You feel very good.", "你感覺身心舒暢。" },
        { "Light shoots in all directions!", "光芒向四面八方射出！" },
        { "The ground shakes for a moment.", "地面短暫震動了一下。" },
        { "There is a searing blast of light!", "一道刺眼的強光爆發了！" },
        { "Darkness seems to crush you!", "黑暗彷彿要將你壓碎！" },
        { "The ground shakes! The ceiling caves in!", "地面劇烈搖晃！洞頂崩塌了！" },
        { "The cave ceiling collapses on you!", "洞頂朝你塌了下來！" },
        { "The cave floor twists in an unnatural way!", "洞穴地面詭異地扭曲！" },
        { "The cave quakes!", "洞穴劇烈震動！" },
        { "You are pummeled with debris!", "碎石不斷砸向你！" },
        { "No monster selected!", "尚未選擇怪物！" },
        { "This spell must target a monster.", "這個法術必須以怪物為目標。" },
        { "The way is barred.", "前方道路被阻擋了。" },
        { "You are already in single combat!", "你已經處於單挑狀態！" },
        { "You must attack a monster.", "你必須攻擊一隻怪物。" },
        { "You feel a surge of power!", "你感受到一股力量湧現！" },
        { "You nimbly dodge the blast!", "你靈巧地閃過了衝擊！" },
        { "You are bashed by rubble!", "碎石猛烈地撞擊了你！" },
        { "You are crushed between the floor and ceiling!", "你被地面與洞頂夾住了！" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
