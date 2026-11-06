/* Every native generic effect display-only source label and fallback. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-general-effect-messages.h"
int main(void)
{
    static const struct { const char *en; const char *zh; } cases[] = {
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
        { "Enchant which item? ", "要為哪件物品附魔？ " },
        { "You have nothing to enchant.", "你沒有可以附魔的物品。" },
        { "Uncurse which item? ", "要解除哪件物品的詛咒？ " },
        { "You have no curses to remove.", "你沒有可以解除的詛咒。" },
        { "Identify which item? ", "要鑑定哪件物品？ " },
        { "You have nothing to identify.", "你沒有需要鑑定的物品。" },
        { "Recharge which item? ", "要為哪件物品重新充能？ " },
        { "You have nothing to recharge.", "你沒有可以重新充能的物品。" },
        { "Brand which kind of ammunition? ", "要為哪種彈藥附加屬性？ " },
        { "You have nothing to brand.", "你沒有可以附加屬性的彈藥。" },
        { "Brand which bolts? ", "要為哪一組弩矢附加屬性？ " },
        { "You have no bolts to brand.", "你沒有可附加屬性的弩矢。" },
        { "Make arrows from which staff? ", "要用哪根木杖製作箭矢？ " },
        { "You have no staff to use.", "你沒有可用來製箭的木杖。" },
        { "Drain charges from which item? ", "要從哪件物品汲取充能？ " },
        { "You have nothing to drain charges from.", "你沒有可以汲取充能的物品。" },
        { "Are you sure you want to descend? ", "確定要下到更深的樓層嗎？ " },
        { "Set recall depth to current depth? ", "要將下次回返深度設為目前樓層嗎？ " },
        { "Word of Recall is already active.  Do you want to cancel it? ", "回返法術已啟動，要取消嗎？ " },
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
    assert(sizeof(cases) / sizeof(cases[0]) == 90);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const char *zh = angdroid_general_effect_message(cases[i].en, 1);
        assert(zh && strcmp(zh, cases[i].zh) == 0);
        assert(angdroid_general_effect_message(cases[i].en, 0) == cases[i].en);
    }
    {
        const char *unknown = "A new effect without a translation.";
        assert(angdroid_general_effect_message(unknown, 1) == unknown);
        assert(angdroid_general_effect_message(NULL, 1) == NULL);
    }
    puts("PASS: 90 generic effect messages and prompts, fallback and NULL handling");
    return 0;
}
