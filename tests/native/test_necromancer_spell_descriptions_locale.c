/* Necromancer original descriptions: display-only C99 test;
 * shared Sense Invisible text and all exact-source fallback conditions.
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
    const char *new_description = "Official effect changed.";
    const char *crush = "Instantly kill any monster in line-of-sight with less than four times the player's level in hitpoints, also hurting the player.  Other monsters are unharmed.";
    const char *sacrifice = "Exchanges 50 hitpoints for 50 mana.";
    check("Nether Bolt", "Fires a bolt of nether magic.", "發射一道幽冥魔法箭。");
    check("Sense Invisible", "Enables you to see invisible monsters for 24+1d24 turns.", "讓你能看見隱形怪物，持續 24+1d24 回合。");
    check("Create Darkness", "Darkens all squares in a level-dependent area.  If you are in a room, the entire room will be darkened as well.", "使隨角色等級而變化的範圍內所有格子變暗。如果你身處房間，整個房間也會陷入黑暗。");
    check("Bat Form", "Changes you into the shape of a bat, weak but with excellent senses.  While shapechanged you cannot use items.", "化身為蝙蝠，雖然身體虛弱，卻有極佳的感官。變身期間無法使用物品。");
    check("Read Minds", "Detects all monsters which have a spirit in the immediate area, and then maps in a region around them, increasing in size with character level.", "偵測附近所有具有靈魂的怪物，並繪製其周圍區域的地圖；繪製範圍隨角色等級增加。");
    check("Tap Unlife", "Targets the nearest undead monster and damages it to gain player mana.", "以最近的一隻不死怪物為目標，對其造成傷害以恢復玩家的法力。");
    check("Crush", "Instantly kill any monster in line-of-sight with less than four times the player's level in hitpoints, also hurting the player.  Other monsters are unharmed.", "立刻殺死視線內生命值低於角色等級四倍的怪物，但玩家也會受到傷害。不符合條件的怪物不受影響。");
    check("Sleep Evil", "Attempts to put to sleep each evil monster within line of sight.  Monsters that resist confusion are not affected.", "嘗試使視線內每隻邪惡怪物入睡。能抵抗混亂的怪物不受影響。");
    check("Shadow Shift", "Displaces your spirit a short distance, hurting your body as it is dragged along.", "將你的靈魂移動一小段距離，身體被拖曳跟隨時會受到傷害。");
    check("Disenchant", "Casts a bolt which wounds, and makes spellcasting monsters less successful in casting spells.", "發射一道魔法箭造成傷害，並降低施法型怪物成功施法的機率。");
    check("Frighten", "Attempts to scare a single monster for a level-dependent duration.  Monsters that resist fear are not affected.", "嘗試使一隻怪物恐懼，持續時間取決於角色等級。能抵抗恐懼的怪物不受影響。");
    check("Vampire Strike", "Teleports you to the nearest living monster and drains a level-dependent number of hitpoints, healing and nourishing the player.", "傳送至最近的一隻活著的怪物身旁，吸取隨角色等級變化的生命值，藉此治療玩家並補充營養。");
    check("Dispel Life", "Inflicts unresistable damage on each living monster within line of sight.", "對視線內每隻活著的怪物造成無法抵抗的傷害。");
    check("Dark Spear", "Fires a beam of darkness.", "發射一道黑暗光束。");
    check("Warg Form", "Changes you into the form of a warg, and gives you temporary berserk strength, at the cost of some hitpoints.", "化身為座狼，並暫時獲得狂暴力量，但會犧牲部分生命值。");
    check("Banish Spirits", "Teleports away each monster with a spirit within line of sight.", "將視線內所有具有靈魂的怪物傳送走。");
    check("Annihilate", "Inflicts damage equal to four times player level on a single monster that is not undead, an elemental, a golem, or a vortex.", "對單一怪物造成等於角色等級四倍的傷害，但不影響不死生物、元素生物、魔像或漩渦怪物。");
    check("Grond's Blow", "Destroys everything in a 15-square radius circle around you.  All monsters, objects, and terrain features in the area of effect are destroyed, except stairs and permanent walls.  You will also be blinded for 10+1d10 turns, unless you have resistance to blindness or dark.  This ritual has no effect when used in the town.", "摧毀你周圍半徑 15 格內的所有怪物、物品與地形，但樓梯及永久牆壁除外。你還會失明 10+1d10 回合，除非具有抗失明或抗黑暗能力。此儀式在城鎮中沒有效果。");
    check("Unleash Chaos", "Fires a chaos beam, arc or ball in the chosen direction, wakes everything in the area, and attempts to frighten everything in line of sight.", "朝選定方向發射混沌光束、扇形攻擊或法球，喚醒效果範圍內的一切，並嘗試使視線內的所有目標恐懼。");
    check("Fume of Mordor", "Darkens, maps out, and detects all objects on the complete current dungeon level.", "使目前整個地城樓層陷入黑暗、繪製完整地圖，並偵測該樓層的所有物品。");
    check("Storm of Darkness", "Invokes a radius-4 darkness ball.", "召喚爆炸半徑為 4 格的黑暗法球。");
    check("Power Sacrifice", "Exchanges 50 hitpoints for 50 mana.", "犧牲 50 點生命值，換取 50 點法力。");
    check("Zone of Unmagic", "Produces a zone of disenchantment affecting the player and monsters within radius 4.", "產生半徑 4 格的解除附魔區域，影響玩家自身與範圍內的怪物。");
    check("Vampire Form", "Allows you to assume the form of a vampire at the cost of half your current hitpoints, than teleports you to the nearest living monster and drains a level-dependent number of hitpoints, healing and nourishing you.  When first transformed you will temporarily be able to take hitpoints from living monsters with your bite attack.", "犧牲目前生命值的一半，化身為吸血鬼，接著傳送至最近的一隻活著的怪物身旁，吸取隨角色等級變化的生命值以治療並補充營養。剛變身後，還能暫時透過撕咬攻擊吸取活著怪物的生命值。");
    check("Curse", "Deals direct damage to a single monster, more the more damaged the monster is already, at the cost of 100 hitpoints.", "對一隻怪物造成直接傷害；怪物原本受傷越重，傷害越高，但會消耗玩家 100 點生命值。");
    check("Command", "Allows you to take control of a monster, and move it instead of yourself.  Possible actions are move, stand still, 'd'rop, 'm'agic, or 'r'elease.", "控制一隻怪物，改為操縱牠而不是玩家自身。可執行的動作有移動、原地不動、d 丟棄、m 施法，或 r 解除控制。");
    assert(angdroid_zh_tw_spell_description("Crush",
        new_description, 1) == new_description);
    assert(angdroid_zh_tw_spell_description("Unknown",
        crush, 1) == crush);
    assert(angdroid_zh_tw_spell_description("Power Sacrifice",
        sacrifice, 0) == sacrifice);
    assert(angdroid_zh_tw_spell_description(NULL, sacrifice, 1) == sacrifice);
    assert(angdroid_zh_tw_spell_description("Crush", NULL, 1) == NULL);
    assert(angdroid_zh_tw_spell_description("", "", 1)[0] == '\0');
    checks += 6;
    printf("PASS: Necromancer spell descriptions (%d checks)\n", checks);
    return 0;
}
