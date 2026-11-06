/* Original Mage descriptions. Bilingual and exact-source fallback, C99. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-descriptions.h"

static int checks = 0;

static void check(const char *name, const char *english, const char *chinese)
{
    const char *translated = angdroid_zh_tw_spell_description(name, english, 1);
    assert(strcmp(translated, chinese) == 0);
    assert(angdroid_zh_tw_spell_description(name, english, 0) == english);
    checks += 2;
}

int main(void)
{
    const char *unknown = "a newly revised spell description";
    const char *known = "Teleports you randomly up to 10 squares away.";
    check("Magic Missile", "Fires a magic missile that always hits its target and does unresistable damage.  Sometimes a beam is fired instead that hurts each monster in its path.  The chance to get a beam goes up with your character level.", "發射必定命中目標、造成無法抵抗傷害的魔法飛彈。有時會改成光束，傷害路徑上的每隻怪物；角色等級越高，發射光束的機率越大。");
    check("Light Room", "Lights up all squares within 2 grids of the player, and hurts all light-sensitive monsters in the area of effect.  If you are in a room, the entire room will be lit up as well.", "照亮你周圍 2 格內的所有位置，並傷害範圍內畏光的怪物。如果你身處房間內，也會照亮整個房間。");
    check("Find Traps, Doors & Stairs", "Detects all traps, doors, and stairs in the immediate area.", "偵測附近所有陷阱、門與樓梯。");
    check("Phase Door", "Teleports you randomly up to 10 squares away.", "隨機傳送到距離不超過 10 格的位置。");
    check("Electric Arc", "Creates an electrical discharge that hurts monsters and destroys vulnerable items in its path.  Length of the effect increases with player level.", "釋放一道電弧，傷害路徑上的怪物並摧毀脆弱的物品。效果距離會隨角色等級增加。");
    check("Detect Monsters", "Detects all non-invisible monsters in the immediate area, for one turn only.", "偵測附近所有非隱形的怪物，效果僅持續一回合。");
    check("Fire Ball", "Shoots a radius-2 fire ball.", "射出爆炸半徑為 2 格的火球。");
    check("Recharging", "Adds charges to a stack of wands or staves.  Chance of success and number of charges gained increase with your level and decrease with level of wand or staff and number of charges.  A failed attempt to recharge destroys one wand or staff from the stack.", "替一疊魔杖或法杖補充使用次數。角色等級越高，成功率與增加的次數越高；道具等級及現有充能越高，則越難成功、補充量也越少。充能失敗會摧毀這疊道具中的一根魔杖或法杖。");
    check("Identify Rune", "Reveals an unknown rune on an object.", "揭露一件物品上尚未辨識的符文。");
    check("Treasure Detection", "Detects gold and senses objects in the immediate area.", "偵測附近的黃金，並感知附近的物品。");
    check("Frost Bolt", "Shoots a bolt of frost that always hits its target.  Sometimes a beam is fired instead that hurts each monster in its path.  The chance to get a beam goes up with your character level.", "射出必定命中目標的寒冰箭。有時會改成光束，傷害路徑上的每隻怪物；角色等級越高，發射光束的機率越大。");
    check("Reveal Monsters", "Detects all monsters in the immediate area, for one turn only.", "偵測附近所有怪物，效果僅持續一回合。");
    check("Acid Spray", "Sprays acid in a 60-degree cone, hitting all monsters up to 10 grids away.", "噴出 60 度錐形的強酸，命中最遠 10 格內的所有怪物。");
    check("Disable Traps, Destroy Doors", "Disables traps and destroys doors within a 1-square radius of you.", "解除你周圍半徑 1 格內的陷阱，並摧毀其中的門。");
    check("Teleport Self", "Teleports you randomly within the current level.", "在目前樓層內隨機傳送。");
    check("Teleport Other", "Produces a bolt that teleports away the first monster in its path.  Distance teleported increases with player level.", "發射一道魔法箭，將路徑上遇到的第一隻怪物傳送走。傳送距離隨角色等級增加。");
    check("Resistance", "Grants you resistance to acid, cold, fire and lightning for 20+1d20 turns.", "獲得酸液、寒冷、火焰與閃電抗性，持續 20+1d20 回合。");
    check("Tap Magical Energy", "Turns wand or staff energy into mana; tapping stuns the player briefly.", "將魔杖或法杖中的能量轉換成法力，但會使角色短暫暈眩。");
    check("Mana Channel", "Increases spellcasting speed by 33% for 5+1d5 turns.", "提高施法速度 33%，持續 5+1d5 回合。");
    check("Door Creation", "A door is created on each empty floor space that is directly adjacent to you.  These doors are closed, but not locked.", "在與你直接相鄰的每一格空地上生成一扇門。這些門都是關閉的，但沒有上鎖。");
    check("Mana Bolt", "Fires a magic missile that always hits its target and does unresistable damage.  Sometimes a beam is fired instead that hurts each monster in its path.  The chance to get a beam goes up with your character level.", "發射必定命中目標、造成無法抵抗傷害的魔法飛彈。有時會改成光束，傷害路徑上的每隻怪物；角色等級越高，發射光束的機率越大。");
    check("Teleport Level", "Teleports you 1 level up or 1 level down (chosen at random).  This spell may have no effect when the forced descent option is set.", "隨機傳送到上一層或下一層。若啟用了強制下降的選項，這個法術可能沒有效果。");
    check("Detection", "Detects all traps, doors, stairs, treasure, objects and monsters in the immediate area.", "偵測附近所有陷阱、門、樓梯、寶藏、物品與怪物。");
    check("Dimension Door", "Teleports you to (or near to) a square of your choice.", "傳送到你指定的格子，或該格子附近的位置。");
    check("Thrust Away", "Fires a short range beam of force.", "發射一道短距離的力場光束。");
    check("Shock Wave", "Shoots a radius-2 sound ball.  Apart from doing sound damage, this also has a chance to stun each monster in the area of effect.", "射出爆炸半徑為 2 格的聲波球。除了造成聲波傷害，也可能使效果範圍內的怪物暈眩。");
    check("Explosion", "Shoots a radius-2 shards ball and throws around anything in the area of effect.", "射出爆炸半徑為 2 格的碎片球，並將效果範圍內的物體拋向四周。");
    check("Banishment", "Removes all monsters represented by a chosen symbol from the level.  Uniques are not affected.  You take 1d4 points of damage for every monster removed.", "從本樓層移除所有以指定符號表示的怪物，獨特怪物不受影響。每移除一隻怪物，你會受到 1d4 點傷害。");
    check("Mass Banishment", "Removes all monsters within 20 squares of you.  Uniques are not affected.  You take 1d3 points of damage for every monster removed.", "移除距離你 20 格內的所有怪物，獨特怪物不受影響。每移除一隻怪物，你會受到 1d3 點傷害。");
    check("Mana Storm", "Shoots a radius-3 mana ball that does unresistable damage to each monster in its area of effect.", "射出爆炸半徑為 3 格的法力球，對效果範圍內的每隻怪物造成無法抵抗的傷害。");
    assert(angdroid_zh_tw_spell_description("Phase Door", unknown, 1) == unknown);
    assert(angdroid_zh_tw_spell_description("unknown", known, 1) == known);
    assert(angdroid_zh_tw_spell_description(NULL, known, 1) == known);
    assert(angdroid_zh_tw_spell_description("Phase Door", NULL, 1) == NULL);
    assert(angdroid_zh_tw_spell_description("Phase Door", known, 0) == known);
    assert(angdroid_zh_tw_spell_description("", "", 1)[0] == '\0');
    checks += 6;
    printf("PASS: Mage spell long descriptions (%d checks)\n", checks);
    return 0;
}
