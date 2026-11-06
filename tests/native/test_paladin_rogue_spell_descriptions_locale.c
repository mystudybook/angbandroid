/* Paladin (16) and Rogue (10) canonical long descriptions: C99.
 * Verify both sides of the Priest/Paladin Heroism difference.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-descriptions.h"

static int checks = 0;
static void check(const char *name, const char *english, const char *zh)
{
    assert(strcmp(angdroid_zh_tw_spell_description(name, english, 1),
        zh) == 0);
    assert(angdroid_zh_tw_spell_description(name, english, 0) == english);
    checks += 2;
}
int main(void)
{
    const char *priest_heroism = "Cures 10 points of damage, removes fear and, from character level 20, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.";
    const char *paladin_heroism = "Cures 10 points of damage, removes fear and, from character level 15, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.";
    const char *changed_desc = "Updated class spell description.";
    check("Bless", "Blesses you, giving a bonus of +5 to AC and +10 to-hit, for a level-dependent number of turns.", "賜予祝福，使防禦等級（AC）加值提高 5 點、命中加值提高 10 點，持續時間取決於角色等級。");
    check("Detect Evil", "Detects all evil monsters in the immediate area, for one turn only.", "偵測附近所有邪惡怪物，效果僅持續一回合。");
    check("Call Light", "Lights up all squares in a level-dependent area, and hurts all light-sensitive monsters in the area of effect.  If you are in a room, the entire room will be lit up as well.", "照亮隨角色等級改變的範圍內所有格子，並傷害範圍內畏光的怪物。若你身處房間，也會照亮整個房間。");
    check("Minor Healing", "Heals a level-dependent amount or percentage of hitpoints, and cures 20 points worth of cuts and of stunning.", "恢復隨角色等級變化的生命值數量或百分比，並各治癒 20 點割傷與暈眩。");
    check("Sense Invisible", "Enables you to see invisible monsters for 24+1d24 turns.", "讓你能看見隱形怪物，持續 24+1d24 回合。");
    check("Heroism", "Cures 10 points of damage, removes fear and, from character level 15, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.", "恢復 10 點生命值並消除恐懼；角色等級達到 15 後，還會獲得英雄氣概（抵抗恐懼，命中加值提高 12 點），持續時間取決於角色等級。");
    check("Protection from Evil", "Protects you from evil for a level-dependent duration: all melee attacks by evil monsters have a chance to be repelled, unless the monster's level is higher than your character level.", "在隨角色等級變化的一段時間內獲得邪惡防護：邪惡怪物的近戰攻擊有機率被阻擋，但若怪物等級高於角色等級，就無法阻擋。");
    check("Remove Curse", "Attempts to remove a curse from an item, more effective at higher character levels.", "嘗試移除一件物品上的詛咒，角色等級越高，效果越好。");
    check("Word of Recall", "Teleports you from the dungeon to the town or from the town to the deepest level you have visited in the dungeon.  The recall effect is not immediate; it is delayed by 14+1d20 turns.  During that delay, the prayer can be canceled by invoking the prayer of recall again.  This prayer has no effect when the option to restrict the use of stairs and recall is set, unless Morgoth is dead.", "從地城傳送回城鎮，或從城鎮傳送到曾到達的最深地城樓層。召回不會立即生效，而是延遲 14+1d20 回合；在等待期間再次施展召回祈禱可以取消。若啟用了限制樓梯與召回的選項，此祈禱將沒有效果，除非魔苟斯已死亡。");
    check("Healing", "Heals 2000 points of damage and cures all stunning, cuts, poison and amnesia.", "恢復 2000 點生命值，並完全治癒暈眩、割傷、中毒與失憶。");
    check("Clairvoyance", "Lights up, maps out, and senses all objects on the complete current dungeon level.", "照亮、繪製目前整個地城樓層的地圖，並感知該樓層的所有物品。");
    check("Smite Evil", "Temporarily makes your melee attacks slay evil creatures.", "暫時讓你的近戰攻擊能夠剋制邪惡生物。");
    check("Demon Bane", "Temporarily makes your melee attacks powerfully slay demons.", "暫時大幅增強近戰攻擊對惡魔的剋制能力。");
    check("Enchant Weapon", "Attempts to improve the to-hit bonus and the to-dam bonus of a weapon.", "嘗試提高一把武器的命中加值與傷害加值。");
    check("Enchant Armour", "Attempts to improve the armour class bonus of a piece of armour.", "嘗試提高一件護甲的防禦等級加值。");
    check("Single Combat", "Encases you and the targeted monster in a stone cell from which only the winner can emerge.", "將你與指定怪物封入石室，只有獲勝的一方能夠離開。");
    check("Detect Monsters", "Detects all non-invisible monsters in the immediate area, for one turn only.", "偵測附近所有非隱形的怪物，效果僅持續一回合。");
    check("Phase Door", "Teleports you randomly up to 10 squares away.", "隨機傳送到距離不超過 10 格的位置。");
    check("Object Detection", "Detects all objects in the immediate area.", "偵測附近的所有物品。");
    check("Detect Stairs", "Detects all stairs in a wide area.", "偵測較大範圍內的所有樓梯。");
    check("Recharging", "Adds charges to a stack of wands or staves.  Chance of success and number of charges gained increase with your level and decrease with level of wand or staff and number of charges.  A failed attempt to recharge destroys one wand or staff from the stack.", "替一疊魔杖或法杖補充使用次數。角色等級越高，成功率與增加的次數越高；道具等級及現有充能越高，則越難成功、補充量也越少。充能失敗會摧毀這疊道具中的一根魔杖或法杖。");
    check("Reveal Monsters", "Detects all monsters in the immediate area, for one turn only.", "偵測附近所有怪物，效果僅持續一回合。");
    check("Teleport Self", "Teleports you randomly within the current level.", "在目前樓層內隨機傳送。");
    check("Hit and Run", "Prepares you to teleport a short distance after stealing from a monster.", "準備在從怪物身上偷竊後，立即短距離傳送。");
    check("Teleport Other", "Produces a bolt that teleports away the first monster in its path.  Distance teleported increases with player level.", "發射一道魔法箭，將路徑上遇到的第一隻怪物傳送走。傳送距離隨角色等級增加。");
    check("Teleport Level", "Teleports you 1 level up or 1 level down (chosen at random).  This spell may have no effect when the forced descent option is set.", "隨機傳送到上一層或下一層。若啟用了強制下降的選項，這個法術可能沒有效果。");
    assert(strcmp(angdroid_zh_tw_spell_description("Heroism",
        priest_heroism, 1), "恢復 10 點生命值並消除恐懼；自角色等級 20 起，還可獲得英雄氣概效果（抵抗恐懼，命中加值提高 12 點），持續時間取決於角色等級。") == 0);
    assert(strcmp(angdroid_zh_tw_spell_description("Heroism",
        priest_heroism, 1), angdroid_zh_tw_spell_description("Heroism",
        paladin_heroism, 1)) != 0);
    assert(angdroid_zh_tw_spell_description("Heroism",
        changed_desc, 1) == changed_desc);
    assert(angdroid_zh_tw_spell_description("Unknown",
        paladin_heroism, 1) == paladin_heroism);
    assert(angdroid_zh_tw_spell_description(NULL,
        paladin_heroism, 1) == paladin_heroism);
    assert(angdroid_zh_tw_spell_description("Heroism",
        NULL, 1) == NULL);
    assert(angdroid_zh_tw_spell_description("Heroism",
        paladin_heroism, 0) == paladin_heroism);
    assert(angdroid_zh_tw_spell_description("", "", 1)[0] == '\0');
    checks += 8;
    printf("PASS: Paladin and Rogue spell descriptions (%d checks)\n", checks);
    return 0;
}
