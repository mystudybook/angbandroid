/* Exact Druid spell description display strings, no game engine required. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-descriptions.h"
static int checks = 0;
static void check(const char *name, const char *english, const char *chinese)
{
    assert(strcmp(angdroid_zh_tw_spell_description(name, english, 1),
        chinese) == 0);
    assert(angdroid_zh_tw_spell_description(name, english, 0) == english);
    checks += 2;
}
int main(void)
{
    const char *unknown = "A newly revised spell description.";
    const char *ranger_herbs = "Cures all cuts, poison, stunning and black breath.";
    const char *druid_herbs = "Cures all cuts, poison, stunning and black breath, and leaves you well-nourished.";
    check("Detect Life", "Detects all living monsters in the immediate area, for one turn only.", "偵測附近所有活著的怪物，效果僅持續一回合。");
    check("Fox Form", "Changes you into the shape of a quick and stealthy fox.  While shapechanged you cannot use items.", "化身為敏捷且善於潛行的狐狸。變身期間無法使用物品。");
    check("Remove Hunger", "Magically renders you comfortably fed (but not satiated).", "以魔法讓你恢復到不飢餓、但尚未飽足的狀態。");
    check("Stinking Cloud", "Shoots a radius-2 poison ball.", "射出爆炸半徑為 2 格的毒雲球。");
    check("Confuse Monster", "Confuse a single monster; more effective against animals.", "使一隻怪物陷入混亂；對動物更有效。");
    check("Slow Monster", "Slows down a monster; more effective against animals.", "使一隻怪物的動作變慢；對動物更有效。");
    check("Cure Poison", "Neutralizes poison.", "解除中毒狀態。");
    check("Resist Poison", "Grants you resistance to poison for 20+1d20 turns.", "獲得毒素抗性，持續 20+1d20 回合。");
    check("Turn Stone to Mud", "Produces a beam that destroys the first section of wall it hits, unless it is a permanent wall.  Each monster caught in the beam that is susceptible to rock remover takes 20+1d30 points of damage.", "發射一道光束，摧毀擊中的第一段非永久牆壁。光束路徑上會受到移除岩石效果影響的每隻怪物，將受到 20+1d30 點傷害。");
    check("Sense Surroundings", "Maps the nearby area.", "繪製附近區域的地圖。");
    check("Lightning Strike", "Strikes a target from above with a lightning bolt, followed by a clap of thunder.  Target grid must be targetable and known to the player; no target grid will mean the player grid is hit.", "從上方召來一道閃電擊中目標，隨後響起雷聲。目標格必須是玩家已知且可瞄準的位置；若未指定目標格，閃電將擊中玩家所在位置。");
    check("Earth Rising", "Causes shards to explode in a line out of the earth, striking objects and monsters.  Length of the line increases with player level.", "讓岩石碎片從地面沿直線爆發，打擊路徑上的物品與怪物。攻擊長度隨角色等級增加。");
    check("Trance", "Holds every adjacent, non-resistant monster for a level-dependent number of turns; animals are entranced for longer.", "使所有相鄰且未能抵抗的怪物定身，持續時間取決於角色等級；動物被定身的時間更長。");
    check("Mass Sleep", "Attempts to put to sleep each monster within line of sight.  Monsters that resist sleep are not affected.", "嘗試使視線內的每隻怪物入睡。能抵抗睡眠的怪物不受影響。");
    check("Become Pukel-man", "Gives you the form of a statue of the Drúedain, slow but hard to hurt.", "變身為德魯阿丹人的石像；行動緩慢，但不容易受到傷害。");
    check("Eagle's Flight", "Gives you the form of one of the great eagles of Manwë.", "化身為曼威麾下巨鷹的形態。");
    check("Bear Form", "Allows you to take the form of a great bear of the mountains, sweeping aside your enemies with your mighty paws.", "化身為山中的巨熊，以強大的熊掌掃開敵人。");
    check("Tremor", "Causes a targeted 4-square radius earthquake.  Earthquakes may hurt the player, damage monsters and destroy items in some squares in the area of effect.  Artifacts will not be destroyed by this spell.  This spell has no effect when used in the town.", "在指定目標處引發半徑 4 格的地震。地震可能傷害玩家、使怪物受傷，並摧毀效果範圍內部分格子的物品。神器不會被此法術摧毀；在城鎮中使用則沒有效果。");
    check("Haste Self", "Hastes you (+10 to speed) for a level-dependent duration.", "使自身速度提高 10 點，持續時間取決於角色等級。");
    check("Revitalize", "Restores all stats and experience points to maximum.", "將所有能力值與經驗值恢復至最大值。");
    check("Rapid Regeneration", "Speed up digestion to increase regeneration of hitpoints for the next 5+1d3 turns (or until hungry).", "加快消化速度，以提高生命值再生速度，持續 5+1d3 回合（或直到感到飢餓）。");
    check("Herbal Curing", "Cures all cuts, poison, stunning and black breath, and leaves you well-nourished.", "治癒所有割傷、中毒、暈眩與黑色吐息，並讓你處於營養充足的狀態。");
    check("Meteor Swarm", "Shoots a swarm of 3 or 4 meteors.  Meteors will travel until hitting a wall, a monster, or reaching the target location.  Meteors will explode as a radius-1 ball that does unresistable damage to each monster in its area of effect.  If this kills the monster that blocked the path to the target location, the next meteor may continue to find another target.", "射出 3 或 4 顆流星。流星會飛行至撞上牆壁、怪物或抵達目標位置；每顆流星會爆炸成半徑 1 格的法球，對範圍內每隻怪物造成無法抵抗的傷害。若擋在前方的怪物因此死亡，後續流星可能繼續命中其他目標。");
    check("Rift", "Fires a gravity beam that hurts each monster in its path and has a chance of teleporting it away.", "發射重力光束，傷害路徑上的每隻怪物，並有機率將其傳送走。");
    check("Ice Storm", "Strikes every monster in line of sight with sheets of ice from above.", "從上方降下大片冰霜，襲擊視線內的每隻怪物。");
    check("Volcanic Eruption", "Creates a radius 5 ball of fire, followed by a radius 5 earthquake, around the player.", "以玩家為中心產生半徑 5 格的火球，接著引發半徑 5 格的地震。");
    check("River of Lightning", "Spews plasma in a long narrow cone from the player.", "從玩家所在位置向前噴射一道狹長的電漿錐形攻擊。");
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        ranger_herbs, 1) == ranger_herbs);
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        druid_herbs, 1) != druid_herbs);
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        unknown, 1) == unknown);
    assert(angdroid_zh_tw_spell_description("Unknown Druid Spell",
        druid_herbs, 1) == druid_herbs);
    assert(angdroid_zh_tw_spell_description(NULL, druid_herbs, 1)
        == druid_herbs);
    assert(angdroid_zh_tw_spell_description("Herbal Curing", NULL, 1)
        == NULL);
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        druid_herbs, 0) == druid_herbs);
    assert(angdroid_zh_tw_spell_description("", "", 1)[0] == '\0');
    checks += 8;
    printf("PASS: Druid spell descriptions (%d checks)\n", checks);
    return 0;
}
