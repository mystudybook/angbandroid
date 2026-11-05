/* UI-only original Angband generic effect messages, no changes to
 * effects, item identifiers, spell actions, RNG or save-file formats.
 */
#ifndef ANGDROID_ZH_TW_GENERAL_EFFECT_MESSAGES_H
#define ANGDROID_ZH_TW_GENERAL_EFFECT_MESSAGES_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_general_effect_message(const char *english,
    int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "The removal fails.", "移除失敗了。" },
        { "The enchantment failed.", "附魔失敗了。" },
        { "The branding failed.", "賦予屬性失敗了。" },
        { "You vomit!", "你吐了！" },
        { "It's crunchy.", "咬起來嘎吱作響。" },
        { "It nearly breaks your tooth!", "差點把你的牙齒崩掉！" },
        { "You can only deploy one decoy at a time.", "你一次只能放置一個誘餌。" },
        { "There is no clear floor on which to cast the spell.", "附近沒有空曠的地板可以施放此法術。" },
        { "You feel your life energies returning.", "你感覺生命活力逐漸恢復。" },
        { "You feel more experienced.", "你感覺自己的經驗有所增長。" },
        { "Your light dims.", "你的光源變暗了。" },
        { "The draining fails.", "吸取失敗了。" },
        { "You feel your head clear.", "你感覺頭腦恢復了清晰。" },
        { "You feel your head clear somewhat.", "你感覺頭腦稍微清醒了些。" },
        { "Nothing happens.", "什麼事也沒發生。" },
        { "The air about you becomes charged...", "你周圍的空氣開始充滿能量……" },
        { "A tension leaves the air around you...", "你感覺周圍空氣中的緊繃感消失了……" },
        { "The world changes!", "世界發生了變化！" },
        { "Images form in your mind!", "你的腦海中浮現出畫面！" },
        { "You sense the presence of traps!", "你感覺到了陷阱的存在！" },
        { "You sense no traps.", "你沒有感覺到陷阱。" },
        { "You sense the presence of doors!", "你感覺到了門的存在！" },
        { "You sense no doors.", "你沒有感覺到門。" },
        { "You sense the presence of stairs!", "你感覺到了樓梯的存在！" },
        { "You sense no stairs.", "你沒有感覺到樓梯。" },
        { "You sense the presence of buried treasure!", "你感覺到了埋藏的寶藏！" },
        { "You sense no buried treasure.", "你沒有感覺到埋藏的寶藏。" },
        { "You sense the presence of gold!", "你感覺到了金子的存在！" },
        { "You sense no gold.", "你沒有感覺到金子。" },
        { "You detect the presence of gold!", "你偵測到了金子！" },
        { "You detect no gold.", "你沒有偵測到金子。" },
        { "You sense the presence of objects!", "你感覺到了物品的存在！" },
        { "You sense no objects.", "你沒有感覺到物品。" },
        { "You detect the presence of objects!", "你偵測到了物品！" },
        { "You detect no objects.", "你沒有偵測到物品。" },
        { "You sense life!", "你感覺到了生命氣息！" },
        { "You sense no life.", "你沒有感覺到生命氣息。" },
        { "You sense the presence of monsters!", "你感覺到了怪物的存在！" },
        { "You sense no monsters.", "你沒有感覺到怪物。" },
        { "You sense the presence of invisible creatures!", "你感覺到了隱形生物的存在！" },
        { "You sense no invisible creatures.", "你沒有感覺到隱形生物。" },
        { "These monsters could provide good sport.", "這些怪物或許能讓你好好活動筋骨。" },
        { "You smell no fear in the air.", "空氣中沒有恐懼的氣味。" },
        { "You sense the presence of evil creatures!", "你感覺到了邪惡生物的存在！" },
        { "You sense no evil creatures.", "你沒有感覺到邪惡生物。" },
        { "You sense the presence of spirits!", "你感覺到了靈體的存在！" },
        { "You sense no spirits.", "你沒有感覺到靈體。" },
        { "There is no empty floor here.", "這裡沒有空地。" },
        { "Nothing happens!", "什麼事也沒發生！" },
        { "The recharge backfires!", "重新充能失控了！" },
        { "There is a bright flash of light.", "一道耀眼的光芒閃過。" },
        { "You hear a sudden stirring in the distance!", "你聽見遠處突然傳來騷動！" },
        { "But nothing comes.", "但什麼也沒有出現。" },
        { "Probing...", "正在探查……" },
        { "That's all.", "探查完畢。" },
        { "Teleportation forbidden!", "禁止傳送！" },
        { "Failed to find teleport destination!", "找不到傳送目的地！" },
        { "You resist the effect!", "你抵抗了這次效果！" },
        { "An image of your surroundings forms in your mind...", "你的腦海中浮現出周圍環境的影像……" },
        { "A great blackness rolls through the dungeon...", "一股巨大的黑暗席捲整座地城……" },
        { "You are surrounded by a white light.", "一道白光環繞著你。" },
        { "Darkness surrounds the decoy.", "黑暗籠罩了誘餌。" },
        { "Darkness surrounds you.", "黑暗籠罩著你。" },
        { "Your gear merges into your body.", "你的裝備與身體融為一體。" },
        { "No monster selected!", "尚未選擇怪物！" },
        { "You are surrounded by a malignant aura.", "一股邪惡的氣息環繞著你。" },
        { "You are surrounded by a powerful aura.", "一股強大的氣息環繞著你。" },
        { "The air around you starts to swirl...", "你周圍的空氣開始旋轉……" },
        { "You sense a malevolent presence blocking passage to the levels below.", "你感覺到一股邪惡力量阻止你前往更深的樓層。" },
        { "You rise up through the ceiling.", "你穿過洞頂向上升起。" },
        { "You sink through the floor.", "你穿過地板向下沉去。" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
