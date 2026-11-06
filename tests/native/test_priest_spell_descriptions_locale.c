/* Priest spell details: strict original-name/source matching and
 * Paladin Heroism variant safety, independent from Angband engine.
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
    const char *unknown = "New official description";
    const char *paladin_heroism = "Cures 10 points of damage, removes fear and, from character level 15, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.";
    const char *priest_heroism = "Cures 10 points of damage, removes fear and, from character level 20, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.";
    check("Call Light", "Lights up all squares in a level-dependent area, and hurts all light-sensitive monsters in the area of effect.  If you are in a room, the entire room will be lit up as well.", "照亮隨角色等級改變的範圍內所有格子，並傷害範圍內畏光的怪物。若你身處房間，也會照亮整個房間。");
    check("Detect Evil", "Detects all evil monsters in the immediate area, for one turn only.", "偵測附近所有邪惡怪物，效果僅持續一回合。");
    check("Minor Healing", "Heals a level-dependent amount or percentage of hitpoints, and cures 20 points worth of cuts and of stunning.", "恢復隨角色等級變化的生命值數量或百分比，並各治癒 20 點割傷與暈眩。");
    check("Bless", "Blesses you, giving a bonus of +5 to AC and +10 to-hit, for a level-dependent number of turns.", "賜予祝福，使防禦等級（AC）加值提高 5 點、命中加值提高 10 點，持續時間取決於角色等級。");
    check("Sense Invisible", "Enables you to see invisible monsters for 24+1d24 turns.", "讓你能看見隱形怪物，持續 24+1d24 回合。");
    check("Heroism", "Cures 10 points of damage, removes fear and, from character level 20, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.", "恢復 10 點生命值並消除恐懼；自角色等級 20 起，還可獲得英雄氣概效果（抵抗恐懼，命中加值提高 12 點），持續時間取決於角色等級。");
    check("Orb of Draining", "Shoots a radius-2 or radius-3 ball that does unresistable damage to each monster in its area of effect.  Evil monsters take double damage.  The bigger area of effect is attained at character level 30.", "發射爆炸半徑為 2 或 3 格的法球，對範圍內每隻怪物造成無法抵抗的傷害。邪惡怪物受到雙倍傷害；角色等級達到 30 時，爆炸範圍會擴大。");
    check("Spear of Light", "Fires a beam that lights up each square and hurts each light-sensitive monster in its path.", "發射一道光束，照亮路徑上的每一格，並傷害沿途每隻畏光的怪物。");
    check("Dispel Undead", "Inflicts unresistable damage on each undead monster within line of sight.", "對視線內每隻不死怪物造成無法抵抗的傷害。");
    check("Dispel Evil", "Inflicts unresistable damage on each evil monster within line of sight.", "對視線內每隻邪惡怪物造成無法抵抗的傷害。");
    check("Protection from Evil", "Protects you from evil for a level-dependent duration: all melee attacks by evil monsters have a chance to be repelled, unless the monster's level is higher than your character level.", "在隨角色等級變化的一段時間內獲得邪惡防護：邪惡怪物的近戰攻擊有機率被阻擋，但若怪物等級高於角色等級，就無法阻擋。");
    check("Remove Curse", "Attempts to remove a curse from an item, more effective at higher character levels.", "嘗試移除一件物品上的詛咒，角色等級越高，效果越好。");
    check("Portal", "Teleports you randomly over a short distance.", "隨機傳送一小段距離。");
    check("Remembrance", "Restores experience points to maximum if it is currently below maximum.", "若目前經驗值低於最大值，將其恢復至最大值。");
    check("Word of Recall", "Teleports you from the dungeon to the town or from the town to the deepest level you have visited in the dungeon.  The recall effect is not immediate; it is delayed by 14+1d20 turns.  During that delay, the prayer can be canceled by invoking the prayer of recall again.  This prayer has no effect when the option to restrict the use of stairs and recall is set, unless Morgoth is dead.", "從地城傳送回城鎮，或從城鎮傳送到曾到達的最深地城樓層。召回不會立即生效，而是延遲 14+1d20 回合；在等待期間再次施展召回祈禱可以取消。若啟用了限制樓梯與召回的選項，此祈禱將沒有效果，除非魔苟斯已死亡。");
    check("Healing", "Heals 2000 points of damage and cures all stunning, cuts, poison and amnesia.", "恢復 2000 點生命值，並完全治癒暈眩、割傷、中毒與失憶。");
    check("Restoration", "Restores to maximum all stats that are currently below maximum.", "將目前低於最大值的所有能力值恢復至最大值。");
    check("Clairvoyance", "Lights up, maps out, and senses all objects on the complete current dungeon level.", "照亮、繪製目前整個地城樓層的地圖，並感知該樓層的所有物品。");
    check("Enchant Weapon", "Attempts to improve the to-hit bonus and the to-dam bonus of a weapon.", "嘗試提高一把武器的命中加值與傷害加值。");
    check("Enchant Armour", "Attempts to improve the armour class bonus of a piece of armour.", "嘗試提高一件護甲的防禦等級加值。");
    check("Smite Evil", "Temporarily makes your melee attacks slay evil creatures.", "暫時讓你的近戰攻擊能夠剋制邪惡生物。");
    check("Glyph of Warding", "Inscribes a glyph of warding beneath you.  Summoned monsters can't appear on the glyph.  If a monster attempts to move onto the glyph or melee you while you are standing on the glyph, it must first succeed in breaking the glyph.  Higher level monsters are more likely to break the glyph.  The glyph will remain where it is, until a monster succeeds in breaking it, or until you leave the level.", "在腳下刻下防護符文。召喚的怪物無法直接出現在符文上。怪物若想踏上符文，或你站在符文上時想對你進行近戰攻擊，都必須先成功破壞符文；等級越高的怪物越容易破壞。符文會持續存在，直到被怪物破壞或你離開此樓層。");
    check("Demon Bane", "Temporarily makes your melee attacks powerfully slay demons.", "暫時大幅增強近戰攻擊對惡魔的剋制能力。");
    check("Banish Evil", "Teleports away each evil monster within line of sight.", "將視線內每隻邪惡怪物傳送走。");
    check("Word of Destruction", "Destroys everything in a 15-square radius circle around you.  All monsters, objects, and terrain features in the area of effect are destroyed, except stairs and permanent walls.  You will also be blinded for 10+1d10 turns, unless you have resistance to blindness or light.  This prayer has no effect when used in the town.", "摧毀你周圍半徑 15 格圓形範圍內的所有怪物、物品與地形，但樓梯及永久牆壁除外。你還會失明 10+1d10 回合，除非具有抗失明或抗光能力。在城鎮中施展此祈禱沒有效果。");
    check("Holy Word", "Inflicts unresistable damage on each evil monster within line of sight, cures 1000 points of damage, cures all stunning, heals all cut damage, neutralizes poison, and removes fear.", "對視線內每隻邪惡怪物造成無法抵抗的傷害，同時恢復 1000 點生命值、完全治癒暈眩與割傷、解毒並消除恐懼。");
    check("Spear of Oromë", "Fires a beam of holy magic that does extra damage to evil creatures.", "發射一道神聖魔法光束，對邪惡生物造成額外傷害。");
    check("Light of Manwë", "Casts a radius 4 ball of strong light, and then bathes the entire area in light doing 100 damage to every light-sensitive creature.", "施放爆炸半徑為 4 格的強光法球，隨後以光芒照亮整個區域，對每隻畏光生物造成 100 點傷害。");
    /* Priest starts Heroism at level 20, Paladin at level 15. */
    assert(angdroid_zh_tw_spell_description("Heroism",
        paladin_heroism, 1) == paladin_heroism);
    assert(angdroid_zh_tw_spell_description("Heroism",
        priest_heroism, 1) != priest_heroism);
    assert(angdroid_zh_tw_spell_description("Heroism",
        unknown, 1) == unknown);
    assert(angdroid_zh_tw_spell_description("Unknown Priest",
        priest_heroism, 1) == priest_heroism);
    assert(angdroid_zh_tw_spell_description(NULL,
        priest_heroism, 1) == priest_heroism);
    assert(angdroid_zh_tw_spell_description("Heroism",
        NULL, 1) == NULL);
    assert(angdroid_zh_tw_spell_description("Heroism",
        priest_heroism, 0) == priest_heroism);
    assert(angdroid_zh_tw_spell_description("", "", 1)[0] == '\0');
    checks += 8;
    printf("PASS: Priest spell descriptions (%d checks)\n", checks);
    return 0;
}
