/* Original Angband Mage and Druid class.txt spell descriptions, translated for UI only.
 * Match both canonical spell name AND original descriptive text: unrelated
 * variants / updated spell rules must fall back to English, not show stale
 * instructions.  Do not change class.txt, spell data, or saved game records.
 */
#ifndef ANGDROID_ZH_TW_SPELL_DESCRIPTIONS_H
#define ANGDROID_ZH_TW_SPELL_DESCRIPTIONS_H

#include <stddef.h>
#include <string.h>

static inline const char *angdroid_zh_tw_spell_description(
    const char *name, const char *english, int zh_tw)
{
    static const struct {
        const char *name;
        const char *en;
        const char *zh;
    } map[] = {
        { "Magic Missile", "Fires a magic missile that always hits its target and does unresistable damage.  Sometimes a beam is fired instead that hurts each monster in its path.  The chance to get a beam goes up with your character level.", "發射必定命中目標、造成無法抵抗傷害的魔法飛彈。有時會改成光束，傷害路徑上的每隻怪物；角色等級越高，發射光束的機率越大。" },
        { "Light Room", "Lights up all squares within 2 grids of the player, and hurts all light-sensitive monsters in the area of effect.  If you are in a room, the entire room will be lit up as well.", "照亮你周圍 2 格內的所有位置，並傷害範圍內畏光的怪物。如果你身處房間內，也會照亮整個房間。" },
        { "Find Traps, Doors & Stairs", "Detects all traps, doors, and stairs in the immediate area.", "偵測附近所有陷阱、門與樓梯。" },
        { "Phase Door", "Teleports you randomly up to 10 squares away.", "隨機傳送到距離不超過 10 格的位置。" },
        { "Electric Arc", "Creates an electrical discharge that hurts monsters and destroys vulnerable items in its path.  Length of the effect increases with player level.", "釋放一道電弧，傷害路徑上的怪物並摧毀脆弱的物品。效果距離會隨角色等級增加。" },
        { "Detect Monsters", "Detects all non-invisible monsters in the immediate area, for one turn only.", "偵測附近所有非隱形的怪物，效果僅持續一回合。" },
        { "Fire Ball", "Shoots a radius-2 fire ball.", "射出爆炸半徑為 2 格的火球。" },
        { "Recharging", "Adds charges to a stack of wands or staves.  Chance of success and number of charges gained increase with your level and decrease with level of wand or staff and number of charges.  A failed attempt to recharge destroys one wand or staff from the stack.", "替一疊魔杖或法杖補充使用次數。角色等級越高，成功率與增加的次數越高；道具等級及現有充能越高，則越難成功、補充量也越少。充能失敗會摧毀這疊道具中的一根魔杖或法杖。" },
        { "Identify Rune", "Reveals an unknown rune on an object.", "揭露一件物品上尚未辨識的符文。" },
        { "Treasure Detection", "Detects gold and senses objects in the immediate area.", "偵測附近的黃金，並感知附近的物品。" },
        { "Frost Bolt", "Shoots a bolt of frost that always hits its target.  Sometimes a beam is fired instead that hurts each monster in its path.  The chance to get a beam goes up with your character level.", "射出必定命中目標的寒冰箭。有時會改成光束，傷害路徑上的每隻怪物；角色等級越高，發射光束的機率越大。" },
        { "Reveal Monsters", "Detects all monsters in the immediate area, for one turn only.", "偵測附近所有怪物，效果僅持續一回合。" },
        { "Acid Spray", "Sprays acid in a 60-degree cone, hitting all monsters up to 10 grids away.", "噴出 60 度錐形的強酸，命中最遠 10 格內的所有怪物。" },
        { "Disable Traps, Destroy Doors", "Disables traps and destroys doors within a 1-square radius of you.", "解除你周圍半徑 1 格內的陷阱，並摧毀其中的門。" },
        { "Teleport Self", "Teleports you randomly within the current level.", "在目前樓層內隨機傳送。" },
        { "Teleport Other", "Produces a bolt that teleports away the first monster in its path.  Distance teleported increases with player level.", "發射一道魔法箭，將路徑上遇到的第一隻怪物傳送走。傳送距離隨角色等級增加。" },
        { "Resistance", "Grants you resistance to acid, cold, fire and lightning for 20+1d20 turns.", "獲得酸液、寒冷、火焰與閃電抗性，持續 20+1d20 回合。" },
        { "Tap Magical Energy", "Turns wand or staff energy into mana; tapping stuns the player briefly.", "將魔杖或法杖中的能量轉換成法力，但會使角色短暫暈眩。" },
        { "Mana Channel", "Increases spellcasting speed by 33% for 5+1d5 turns.", "提高施法速度 33%，持續 5+1d5 回合。" },
        { "Door Creation", "A door is created on each empty floor space that is directly adjacent to you.  These doors are closed, but not locked.", "在與你直接相鄰的每一格空地上生成一扇門。這些門都是關閉的，但沒有上鎖。" },
        { "Mana Bolt", "Fires a magic missile that always hits its target and does unresistable damage.  Sometimes a beam is fired instead that hurts each monster in its path.  The chance to get a beam goes up with your character level.", "發射必定命中目標、造成無法抵抗傷害的魔法飛彈。有時會改成光束，傷害路徑上的每隻怪物；角色等級越高，發射光束的機率越大。" },
        { "Teleport Level", "Teleports you 1 level up or 1 level down (chosen at random).  This spell may have no effect when the forced descent option is set.", "隨機傳送到上一層或下一層。若啟用了強制下降的選項，這個法術可能沒有效果。" },
        { "Detection", "Detects all traps, doors, stairs, treasure, objects and monsters in the immediate area.", "偵測附近所有陷阱、門、樓梯、寶藏、物品與怪物。" },
        { "Dimension Door", "Teleports you to (or near to) a square of your choice.", "傳送到你指定的格子，或該格子附近的位置。" },
        { "Thrust Away", "Fires a short range beam of force.", "發射一道短距離的力場光束。" },
        { "Shock Wave", "Shoots a radius-2 sound ball.  Apart from doing sound damage, this also has a chance to stun each monster in the area of effect.", "射出爆炸半徑為 2 格的聲波球。除了造成聲波傷害，也可能使效果範圍內的怪物暈眩。" },
        { "Explosion", "Shoots a radius-2 shards ball and throws around anything in the area of effect.", "射出爆炸半徑為 2 格的碎片球，並將效果範圍內的物體拋向四周。" },
        { "Banishment", "Removes all monsters represented by a chosen symbol from the level.  Uniques are not affected.  You take 1d4 points of damage for every monster removed.", "從本樓層移除所有以指定符號表示的怪物，獨特怪物不受影響。每移除一隻怪物，你會受到 1d4 點傷害。" },
        { "Mass Banishment", "Removes all monsters within 20 squares of you.  Uniques are not affected.  You take 1d3 points of damage for every monster removed.", "移除距離你 20 格內的所有怪物，獨特怪物不受影響。每移除一隻怪物，你會受到 1d3 點傷害。" },
        { "Mana Storm", "Shoots a radius-3 mana ball that does unresistable damage to each monster in its area of effect.", "射出爆炸半徑為 3 格的法力球，對效果範圍內的每隻怪物造成無法抵抗的傷害。" },

        /* Druid: both spell name and the exact original description must match. */
        { "Detect Life", "Detects all living monsters in the immediate area, for one turn only.", "偵測附近所有活著的怪物，效果僅持續一回合。" },
        { "Fox Form", "Changes you into the shape of a quick and stealthy fox.  While shapechanged you cannot use items.", "化身為敏捷且善於潛行的狐狸。變身期間無法使用物品。" },
        { "Remove Hunger", "Magically renders you comfortably fed (but not satiated).", "以魔法讓你恢復到不飢餓、但尚未飽足的狀態。" },
        { "Stinking Cloud", "Shoots a radius-2 poison ball.", "射出爆炸半徑為 2 格的毒雲球。" },
        { "Confuse Monster", "Confuse a single monster; more effective against animals.", "使一隻怪物陷入混亂；對動物更有效。" },
        { "Slow Monster", "Slows down a monster; more effective against animals.", "使一隻怪物的動作變慢；對動物更有效。" },
        { "Cure Poison", "Neutralizes poison.", "解除中毒狀態。" },
        { "Resist Poison", "Grants you resistance to poison for 20+1d20 turns.", "獲得毒素抗性，持續 20+1d20 回合。" },
        { "Turn Stone to Mud", "Produces a beam that destroys the first section of wall it hits, unless it is a permanent wall.  Each monster caught in the beam that is susceptible to rock remover takes 20+1d30 points of damage.", "發射一道光束，摧毀擊中的第一段非永久牆壁。光束路徑上會受到移除岩石效果影響的每隻怪物，將受到 20+1d30 點傷害。" },
        { "Sense Surroundings", "Maps the nearby area.", "繪製附近區域的地圖。" },
        { "Lightning Strike", "Strikes a target from above with a lightning bolt, followed by a clap of thunder.  Target grid must be targetable and known to the player; no target grid will mean the player grid is hit.", "從上方召來一道閃電擊中目標，隨後響起雷聲。目標格必須是玩家已知且可瞄準的位置；若未指定目標格，閃電將擊中玩家所在位置。" },
        { "Earth Rising", "Causes shards to explode in a line out of the earth, striking objects and monsters.  Length of the line increases with player level.", "讓岩石碎片從地面沿直線爆發，打擊路徑上的物品與怪物。攻擊長度隨角色等級增加。" },
        { "Trance", "Holds every adjacent, non-resistant monster for a level-dependent number of turns; animals are entranced for longer.", "使所有相鄰且未能抵抗的怪物定身，持續時間取決於角色等級；動物被定身的時間更長。" },
        { "Mass Sleep", "Attempts to put to sleep each monster within line of sight.  Monsters that resist sleep are not affected.", "嘗試使視線內的每隻怪物入睡。能抵抗睡眠的怪物不受影響。" },
        { "Become Pukel-man", "Gives you the form of a statue of the Drúedain, slow but hard to hurt.", "變身為德魯阿丹人的石像；行動緩慢，但不容易受到傷害。" },
        { "Eagle's Flight", "Gives you the form of one of the great eagles of Manwë.", "化身為曼威麾下巨鷹的形態。" },
        { "Bear Form", "Allows you to take the form of a great bear of the mountains, sweeping aside your enemies with your mighty paws.", "化身為山中的巨熊，以強大的熊掌掃開敵人。" },
        { "Tremor", "Causes a targeted 4-square radius earthquake.  Earthquakes may hurt the player, damage monsters and destroy items in some squares in the area of effect.  Artifacts will not be destroyed by this spell.  This spell has no effect when used in the town.", "在指定目標處引發半徑 4 格的地震。地震可能傷害玩家、使怪物受傷，並摧毀效果範圍內部分格子的物品。神器不會被此法術摧毀；在城鎮中使用則沒有效果。" },
        { "Haste Self", "Hastes you (+10 to speed) for a level-dependent duration.", "使自身速度提高 10 點，持續時間取決於角色等級。" },
        { "Revitalize", "Restores all stats and experience points to maximum.", "將所有能力值與經驗值恢復至最大值。" },
        { "Rapid Regeneration", "Speed up digestion to increase regeneration of hitpoints for the next 5+1d3 turns (or until hungry).", "加快消化速度，以提高生命值再生速度，持續 5+1d3 回合（或直到感到飢餓）。" },
        { "Herbal Curing", "Cures all cuts, poison, stunning and black breath, and leaves you well-nourished.", "治癒所有割傷、中毒、暈眩與黑色吐息，並讓你處於營養充足的狀態。" },
        { "Meteor Swarm", "Shoots a swarm of 3 or 4 meteors.  Meteors will travel until hitting a wall, a monster, or reaching the target location.  Meteors will explode as a radius-1 ball that does unresistable damage to each monster in its area of effect.  If this kills the monster that blocked the path to the target location, the next meteor may continue to find another target.", "射出 3 或 4 顆流星。流星會飛行至撞上牆壁、怪物或抵達目標位置；每顆流星會爆炸成半徑 1 格的法球，對範圍內每隻怪物造成無法抵抗的傷害。若擋在前方的怪物因此死亡，後續流星可能繼續命中其他目標。" },
        { "Rift", "Fires a gravity beam that hurts each monster in its path and has a chance of teleporting it away.", "發射重力光束，傷害路徑上的每隻怪物，並有機率將其傳送走。" },
        { "Ice Storm", "Strikes every monster in line of sight with sheets of ice from above.", "從上方降下大片冰霜，襲擊視線內的每隻怪物。" },
        { "Volcanic Eruption", "Creates a radius 5 ball of fire, followed by a radius 5 earthquake, around the player.", "以玩家為中心產生半徑 5 格的火球，接著引發半徑 5 格的地震。" },
        { "River of Lightning", "Spews plasma in a long narrow cone from the player.", "從玩家所在位置向前噴射一道狹長的電漿錐形攻擊。" },
    };
    size_t i;
    if (!english || !zh_tw || !name) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i) {
        if (strcmp(name, map[i].name) == 0 &&
            strcmp(english, map[i].en) == 0) {
            return map[i].zh;
        }
    }
    return english;
}

#endif
