/* Blackguard's 15 original spell descriptions, display-only native C99.
 * Strict source matching protects gameplay descriptions when Angband changes.
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
    const char *unknown = "The official Blackguard spell was changed.";
    const char *berserk = "Removes fear and grants you berserk strength (resistance to fear, +75 melee skill, -10 to AC, and prevents healing over time) for a level-dependent duration.";
    const char *bloodlust = "Gives you a powerful desire to kill, increased every time you satisfy it.  Gives you extra melee damage and extra blows, but comes with dangerous side-effects.";
    check("Seek Battle", "Detects all monsters susceptible to fear in the immediate area, for one turn only.", "偵測附近所有會受到恐懼效果影響的怪物，效果僅持續一回合。");
    check("Berserk Strength", "Removes fear and grants you berserk strength (resistance to fear, +75 melee skill, -10 to AC, and prevents healing over time) for a level-dependent duration.", "消除恐懼並獲得狂暴力量：抵抗恐懼、近戰技能提高 75 點、防禦等級（AC）降低 10 點，且期間無法隨時間自然恢復生命值。持續時間取決於角色等級。");
    check("Whirlwind Attack", "Deals a level-dependent number of melee blows to each adjacent enemy.  At level 5 you do one blow and gain an additional blow every 15 levels (2 at 20, 3 at 35, and 4 at 50).", "對每個相鄰的敵人施展隨等級增加次數的近戰攻擊。角色等級 5 時攻擊 1 次，之後每增加 15 級多攻擊 1 次：等級 20 時 2 次、35 時 3 次、50 時 4 次。");
    check("Shatter Stone", "Destroys an adjacent section of wall or rubble, unless it is a permanent wall.  Deals 20+1d30 damage to monsters vulnerable to rock remover.", "摧毀相鄰的一段牆壁或瓦礫，但無法摧毀永久牆壁。對會受到移除岩石效果影響的怪物造成 20+1d30 點傷害。");
    check("Leap into Battle", "You run up to 4 spaces towards targeted enemy, then perform a melee blow.  You gain an additional blow at levels 25 and 40.  If you moved, the number of blows is reduced by 25% for each space, then rounded.", "向選定敵人衝刺最多 4 格，然後進行近戰攻擊。角色等級達到 25 與 40 時各增加一次攻擊；若有移動，每移動一格便將攻擊次數減少 25%，最後取整。");
    check("Grim Purpose", "Your mind becomes too focused to be slowed, paralyzed or confused for 12+d12 turns.", "讓心智高度集中，在 12+d12 回合內不會受到減速、麻痺或混亂影響。");
    check("Maim Foe", "Attempts a level-dependent number of melee blows against the targeted enemy.  You get 1 blow for every 15 levels.  If any of these blows damage the enemy, it will be stunned for 6 turns.", "嘗試對指定敵人施展隨角色等級增加次數的近戰攻擊，每 15 級可攻擊 1 次。只要有任一擊造成傷害，敵人就會暈眩 6 回合。");
    check("Howl of the Damned", "Attempts to scare every monster in line of sight for a duration dependent on player level and monster depth.  Monsters that resist fear are not affected.", "嘗試讓視線內的每隻怪物陷入恐懼，持續時間取決於角色等級與怪物所在深度。能抵抗恐懼的怪物不受影響。");
    check("Relentless Taunting", "Makes any monster that can move more inclined to run toward you, and less likely to use a ranged attack (including spells and innate abilities).", "使所有能夠移動的怪物更傾向朝你衝來，並降低牠們使用遠距攻擊的機率，包括法術與天生能力。");
    check("Venom", "Coats all your melee weapons with poison for 18+d18 turns.", "讓你所有近戰武器附上毒素效果，持續 18+d18 回合。");
    check("Werewolf Form", "With a blood-curdling cry you change into a half-man half-wolf!  In this form you move, attack, and heal faster.", "伴隨令人毛骨悚然的嚎叫，化身為半人半狼的狼人！此形態下移動、攻擊及恢復生命的速度都會加快。");
    check("Bloodlust", "Gives you a powerful desire to kill, increased every time you satisfy it.  Gives you extra melee damage and extra blows, but comes with dangerous side-effects.", "激起強烈的殺戮慾望，每次滿足慾望後都會進一步增強。可增加近戰傷害與攻擊次數，但也會帶來危險的副作用。");
    check("Unholy Reprieve", "Restores STR, INT, CON, and experience at the cost of 66 HP.", "消耗 66 點生命值，恢復力量（STR）、智力（INT）、體質（CON）與經驗值。");
    check("Forceful Blow", "Attempts a melee blow that deals additional force damage when it hits, knocking the target back with a chance to stun it.", "嘗試施展一次近戰攻擊；命中時額外造成衝擊傷害，將目標擊退，並有機率使其暈眩。");
    check("Quake", "Causes a radius 13 earthquake centered around the player.", "以玩家為中心引發半徑 13 格的地震。");
    assert(angdroid_zh_tw_spell_description("Berserk Strength",
        unknown, 1) == unknown);
    assert(angdroid_zh_tw_spell_description("Unknown Blackguard",
        berserk, 1) == berserk);
    assert(angdroid_zh_tw_spell_description(NULL,
        berserk, 1) == berserk);
    assert(angdroid_zh_tw_spell_description("Berserk Strength",
        NULL, 1) == NULL);
    assert(angdroid_zh_tw_spell_description("Berserk Strength",
        berserk, 0) == berserk);
    assert(angdroid_zh_tw_spell_description("Bloodlust",
        bloodlust, 0) == bloodlust);
    assert(angdroid_zh_tw_spell_description("",
        "", 1)[0] == '\0');
    assert(strcmp(angdroid_zh_tw_spell_description("Bloodlust",
        bloodlust, 1), bloodlust) != 0);
    checks += 8;
    printf("PASS: Blackguard spell descriptions (%d checks)\n", checks);
    return 0;
}
