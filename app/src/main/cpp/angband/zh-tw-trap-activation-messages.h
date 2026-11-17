/* All canonical Angband and FAangband trap activation notifications.
 * Exact English matching; never modify trap effects, names, parser data or RNG.
 */
#ifndef ANGDROID_ZH_TW_TRAP_ACTIVATION_MESSAGES_H
#define ANGDROID_ZH_TW_TRAP_ACTIVATION_MESSAGES_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_trap_activation_message(
    const char *en, int zh_tw)
{
    static const struct { const char *english, *traditional; } map[] = {
        { "You fall through a trap door!", "你掉進了一道活板門！" },
        { "You float gently down to the next level.", "你輕輕飄落到下一層。" },
        { "You fall into a pit!", "你跌進陷坑！" },
        { "You float gently to the bottom of the pit.", "你緩緩飄落到陷坑底部。" },
        { "You fall into a spiked pit!", "你跌進尖刺陷坑！" },
        { "You float gently to the bottom of the pit, neatly avoiding the spikes.", "你緩緩飄落到陷坑底部，巧妙避開了尖刺。" },
        { "You are impaled!", "尖刺刺穿了你的身體！" },
        { "You are impaled on poisonous spikes!", "你被帶毒的尖刺刺穿了！" },
        { "A new foe appears from nowhere!", "一名新敵人憑空出現！" },
        { "You are enveloped in a cloud of smoke!", "一團煙霧籠罩了你！" },
        { "A sense of dread washes over you...", "一陣恐懼感湧上心頭……" },
        { "You hear a dragon's call!", "你聽見龍的召喚聲！" },
        { "Demons appear!", "惡魔出現了！" },
        { "You hit a teleport trap!", "你觸發了傳送陷阱！" },
        { "You are enveloped in flames!", "烈焰籠罩著你！" },
        { "You are splashed with acid!", "酸液濺到了你身上！" },
        { "A small dart barely misses you.", "一支小飛鏢擦身而過。" },
        { "A small dart hits you!", "一支小飛鏢擊中了你！" },
        { "You are surrounded by a black gas!", "一團黑色氣體包圍了你！" },
        { "You are surrounded by a gas of scintillating colors!", "一團閃爍著各色光芒的氣體包圍了你！" },
        { "You are surrounded by a pungent green gas!", "一團刺鼻的綠色氣體包圍了你！" },
        { "You are surrounded by a strange white mist!", "一團奇異的白霧包圍了你！" },
        { "A high-pitched whistle blares through the dungeon!", "尖銳的哨音響徹地城！" },
        { "A horn blast sounds!", "一聲號角響起！" },
        { "The floor explodes around you!", "你周圍的地板爆炸了！" },
        { "The air seems to catch fire!", "周圍的空氣彷彿燃燒了起來！" },
        { "You feel pressure on your mind.", "你感覺到心靈受到壓迫。" },
        { "You shake your head and are back to normal.", "你搖了搖頭，恢復了正常。" },
        { "Your mind is blasted by psionic energy!", "你的心智遭到靈能衝擊！" },
        { "A rock falls on your head!", "一塊岩石砸在你的頭上！" },
        { "The ground shakes.", "地面震動了。" },
        { "A section of the ceiling falls in behind you!", "你身後的一部分洞頂塌了下來！" },
        { "The trap explodes, blasting the area clear!", "陷阱爆炸，將周圍區域炸得一片空曠！" },
        { "A blazing light descends on you!", "一道熾烈的光芒照向你！" },
        { "Everything starts to fade...", "周圍的一切開始逐漸消失……" },
        { "You feel your energies sap away.", "你感覺自己的精力正在流失。" },
        { "Blades whirl around you, slicing your skin!", "刀刃在你身邊旋轉，割破你的皮膚！" },
        { "You feel yourself turning to stone!", "你感覺自己正逐漸石化！" },
        { "A high-pitched whistle blares out!", "一聲尖銳的哨音響起！" },
        { "A branch hits you from above.", "一根樹枝從上方砸中了你。" },
        { "It was heavy!", "那根樹枝相當沉重！" },
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof map / sizeof map[0]; ++i)
        if (strcmp(en, map[i].english) == 0) return map[i].traditional;
    return en;
}
#endif
