/* Original Angband Mage, Druid, Priest, Necromancer, Paladin, Rogue and Ranger class.txt spell descriptions, translated for UI only.
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

        /* Priest: exact-source descriptions only; Paladin Heroism differs. */
        { "Call Light", "Lights up all squares in a level-dependent area, and hurts all light-sensitive monsters in the area of effect.  If you are in a room, the entire room will be lit up as well.", "照亮隨角色等級改變的範圍內所有格子，並傷害範圍內畏光的怪物。若你身處房間，也會照亮整個房間。" },
        { "Detect Evil", "Detects all evil monsters in the immediate area, for one turn only.", "偵測附近所有邪惡怪物，效果僅持續一回合。" },
        { "Minor Healing", "Heals a level-dependent amount or percentage of hitpoints, and cures 20 points worth of cuts and of stunning.", "恢復隨角色等級變化的生命值數量或百分比，並各治癒 20 點割傷與暈眩。" },
        { "Bless", "Blesses you, giving a bonus of +5 to AC and +10 to-hit, for a level-dependent number of turns.", "賜予祝福，使防禦等級（AC）加值提高 5 點、命中加值提高 10 點，持續時間取決於角色等級。" },
        { "Sense Invisible", "Enables you to see invisible monsters for 24+1d24 turns.", "讓你能看見隱形怪物，持續 24+1d24 回合。" },
        { "Heroism", "Cures 10 points of damage, removes fear and, from character level 20, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.", "恢復 10 點生命值並消除恐懼；自角色等級 20 起，還可獲得英雄氣概效果（抵抗恐懼，命中加值提高 12 點），持續時間取決於角色等級。" },
        { "Orb of Draining", "Shoots a radius-2 or radius-3 ball that does unresistable damage to each monster in its area of effect.  Evil monsters take double damage.  The bigger area of effect is attained at character level 30.", "發射爆炸半徑為 2 或 3 格的法球，對範圍內每隻怪物造成無法抵抗的傷害。邪惡怪物受到雙倍傷害；角色等級達到 30 時，爆炸範圍會擴大。" },
        { "Spear of Light", "Fires a beam that lights up each square and hurts each light-sensitive monster in its path.", "發射一道光束，照亮路徑上的每一格，並傷害沿途每隻畏光的怪物。" },
        { "Dispel Undead", "Inflicts unresistable damage on each undead monster within line of sight.", "對視線內每隻不死怪物造成無法抵抗的傷害。" },
        { "Dispel Evil", "Inflicts unresistable damage on each evil monster within line of sight.", "對視線內每隻邪惡怪物造成無法抵抗的傷害。" },
        { "Protection from Evil", "Protects you from evil for a level-dependent duration: all melee attacks by evil monsters have a chance to be repelled, unless the monster's level is higher than your character level.", "在隨角色等級變化的一段時間內獲得邪惡防護：邪惡怪物的近戰攻擊有機率被阻擋，但若怪物等級高於角色等級，就無法阻擋。" },
        { "Remove Curse", "Attempts to remove a curse from an item, more effective at higher character levels.", "嘗試移除一件物品上的詛咒，角色等級越高，效果越好。" },
        { "Portal", "Teleports you randomly over a short distance.", "隨機傳送一小段距離。" },
        { "Remembrance", "Restores experience points to maximum if it is currently below maximum.", "若目前經驗值低於最大值，將其恢復至最大值。" },
        { "Word of Recall", "Teleports you from the dungeon to the town or from the town to the deepest level you have visited in the dungeon.  The recall effect is not immediate; it is delayed by 14+1d20 turns.  During that delay, the prayer can be canceled by invoking the prayer of recall again.  This prayer has no effect when the option to restrict the use of stairs and recall is set, unless Morgoth is dead.", "從地城傳送回城鎮，或從城鎮傳送到曾到達的最深地城樓層。召回不會立即生效，而是延遲 14+1d20 回合；在等待期間再次施展召回祈禱可以取消。若啟用了限制樓梯與召回的選項，此祈禱將沒有效果，除非魔苟斯已死亡。" },
        { "Healing", "Heals 2000 points of damage and cures all stunning, cuts, poison and amnesia.", "恢復 2000 點生命值，並完全治癒暈眩、割傷、中毒與失憶。" },
        { "Restoration", "Restores to maximum all stats that are currently below maximum.", "將目前低於最大值的所有能力值恢復至最大值。" },
        { "Clairvoyance", "Lights up, maps out, and senses all objects on the complete current dungeon level.", "照亮、繪製目前整個地城樓層的地圖，並感知該樓層的所有物品。" },
        { "Enchant Weapon", "Attempts to improve the to-hit bonus and the to-dam bonus of a weapon.", "嘗試提高一把武器的命中加值與傷害加值。" },
        { "Enchant Armour", "Attempts to improve the armour class bonus of a piece of armour.", "嘗試提高一件護甲的防禦等級加值。" },
        { "Smite Evil", "Temporarily makes your melee attacks slay evil creatures.", "暫時讓你的近戰攻擊能夠剋制邪惡生物。" },
        { "Glyph of Warding", "Inscribes a glyph of warding beneath you.  Summoned monsters can't appear on the glyph.  If a monster attempts to move onto the glyph or melee you while you are standing on the glyph, it must first succeed in breaking the glyph.  Higher level monsters are more likely to break the glyph.  The glyph will remain where it is, until a monster succeeds in breaking it, or until you leave the level.", "在腳下刻下防護符文。召喚的怪物無法直接出現在符文上。怪物若想踏上符文，或你站在符文上時想對你進行近戰攻擊，都必須先成功破壞符文；等級越高的怪物越容易破壞。符文會持續存在，直到被怪物破壞或你離開此樓層。" },
        { "Demon Bane", "Temporarily makes your melee attacks powerfully slay demons.", "暫時大幅增強近戰攻擊對惡魔的剋制能力。" },
        { "Banish Evil", "Teleports away each evil monster within line of sight.", "將視線內每隻邪惡怪物傳送走。" },
        { "Word of Destruction", "Destroys everything in a 15-square radius circle around you.  All monsters, objects, and terrain features in the area of effect are destroyed, except stairs and permanent walls.  You will also be blinded for 10+1d10 turns, unless you have resistance to blindness or light.  This prayer has no effect when used in the town.", "摧毀你周圍半徑 15 格圓形範圍內的所有怪物、物品與地形，但樓梯及永久牆壁除外。你還會失明 10+1d10 回合，除非具有抗失明或抗光能力。在城鎮中施展此祈禱沒有效果。" },
        { "Holy Word", "Inflicts unresistable damage on each evil monster within line of sight, cures 1000 points of damage, cures all stunning, heals all cut damage, neutralizes poison, and removes fear.", "對視線內每隻邪惡怪物造成無法抵抗的傷害，同時恢復 1000 點生命值、完全治癒暈眩與割傷、解毒並消除恐懼。" },
        { "Spear of Oromë", "Fires a beam of holy magic that does extra damage to evil creatures.", "發射一道神聖魔法光束，對邪惡生物造成額外傷害。" },
        { "Light of Manwë", "Casts a radius 4 ball of strong light, and then bathes the entire area in light doing 100 damage to every light-sensitive creature.", "施放爆炸半徑為 4 格的強光法球，隨後以光芒照亮整個區域，對每隻畏光生物造成 100 點傷害。" },

        /* Necromancer: Sense Invisible reuses the exact Priest entry.
         * Preserve all self-damage, targeting and control limitations. */
        { "Nether Bolt", "Fires a bolt of nether magic.", "發射一道幽冥魔法箭。" },
        { "Create Darkness", "Darkens all squares in a level-dependent area.  If you are in a room, the entire room will be darkened as well.", "使隨角色等級而變化的範圍內所有格子變暗。如果你身處房間，整個房間也會陷入黑暗。" },
        { "Bat Form", "Changes you into the shape of a bat, weak but with excellent senses.  While shapechanged you cannot use items.", "化身為蝙蝠，雖然身體虛弱，卻有極佳的感官。變身期間無法使用物品。" },
        { "Read Minds", "Detects all monsters which have a spirit in the immediate area, and then maps in a region around them, increasing in size with character level.", "偵測附近所有具有靈魂的怪物，並繪製其周圍區域的地圖；繪製範圍隨角色等級增加。" },
        { "Tap Unlife", "Targets the nearest undead monster and damages it to gain player mana.", "以最近的一隻不死怪物為目標，對其造成傷害以恢復玩家的法力。" },
        { "Crush", "Instantly kill any monster in line-of-sight with less than four times the player's level in hitpoints, also hurting the player.  Other monsters are unharmed.", "立刻殺死視線內生命值低於角色等級四倍的怪物，但玩家也會受到傷害。不符合條件的怪物不受影響。" },
        { "Sleep Evil", "Attempts to put to sleep each evil monster within line of sight.  Monsters that resist confusion are not affected.", "嘗試使視線內每隻邪惡怪物入睡。能抵抗混亂的怪物不受影響。" },
        { "Shadow Shift", "Displaces your spirit a short distance, hurting your body as it is dragged along.", "將你的靈魂移動一小段距離，身體被拖曳跟隨時會受到傷害。" },
        { "Disenchant", "Casts a bolt which wounds, and makes spellcasting monsters less successful in casting spells.", "發射一道魔法箭造成傷害，並降低施法型怪物成功施法的機率。" },
        { "Frighten", "Attempts to scare a single monster for a level-dependent duration.  Monsters that resist fear are not affected.", "嘗試使一隻怪物恐懼，持續時間取決於角色等級。能抵抗恐懼的怪物不受影響。" },
        { "Vampire Strike", "Teleports you to the nearest living monster and drains a level-dependent number of hitpoints, healing and nourishing the player.", "傳送至最近的一隻活著的怪物身旁，吸取隨角色等級變化的生命值，藉此治療玩家並補充營養。" },
        { "Dispel Life", "Inflicts unresistable damage on each living monster within line of sight.", "對視線內每隻活著的怪物造成無法抵抗的傷害。" },
        { "Dark Spear", "Fires a beam of darkness.", "發射一道黑暗光束。" },
        { "Warg Form", "Changes you into the form of a warg, and gives you temporary berserk strength, at the cost of some hitpoints.", "化身為座狼，並暫時獲得狂暴力量，但會犧牲部分生命值。" },
        { "Banish Spirits", "Teleports away each monster with a spirit within line of sight.", "將視線內所有具有靈魂的怪物傳送走。" },
        { "Annihilate", "Inflicts damage equal to four times player level on a single monster that is not undead, an elemental, a golem, or a vortex.", "對單一怪物造成等於角色等級四倍的傷害，但不影響不死生物、元素生物、魔像或漩渦怪物。" },
        { "Grond's Blow", "Destroys everything in a 15-square radius circle around you.  All monsters, objects, and terrain features in the area of effect are destroyed, except stairs and permanent walls.  You will also be blinded for 10+1d10 turns, unless you have resistance to blindness or dark.  This ritual has no effect when used in the town.", "摧毀你周圍半徑 15 格內的所有怪物、物品與地形，但樓梯及永久牆壁除外。你還會失明 10+1d10 回合，除非具有抗失明或抗黑暗能力。此儀式在城鎮中沒有效果。" },
        { "Unleash Chaos", "Fires a chaos beam, arc or ball in the chosen direction, wakes everything in the area, and attempts to frighten everything in line of sight.", "朝選定方向發射混沌光束、扇形攻擊或法球，喚醒效果範圍內的一切，並嘗試使視線內的所有目標恐懼。" },
        { "Fume of Mordor", "Darkens, maps out, and detects all objects on the complete current dungeon level.", "使目前整個地城樓層陷入黑暗、繪製完整地圖，並偵測該樓層的所有物品。" },
        { "Storm of Darkness", "Invokes a radius-4 darkness ball.", "召喚爆炸半徑為 4 格的黑暗法球。" },
        { "Power Sacrifice", "Exchanges 50 hitpoints for 50 mana.", "犧牲 50 點生命值，換取 50 點法力。" },
        { "Zone of Unmagic", "Produces a zone of disenchantment affecting the player and monsters within radius 4.", "產生半徑 4 格的解除附魔區域，影響玩家自身與範圍內的怪物。" },
        { "Vampire Form", "Allows you to assume the form of a vampire at the cost of half your current hitpoints, than teleports you to the nearest living monster and drains a level-dependent number of hitpoints, healing and nourishing you.  When first transformed you will temporarily be able to take hitpoints from living monsters with your bite attack.", "犧牲目前生命值的一半，化身為吸血鬼，接著傳送至最近的一隻活著的怪物身旁，吸取隨角色等級變化的生命值以治療並補充營養。剛變身後，還能暫時透過撕咬攻擊吸取活著怪物的生命值。" },
        { "Curse", "Deals direct damage to a single monster, more the more damaged the monster is already, at the cost of 100 hitpoints.", "對一隻怪物造成直接傷害；怪物原本受傷越重，傷害越高，但會消耗玩家 100 點生命值。" },
        { "Command", "Allows you to take control of a monster, and move it instead of yourself.  Possible actions are move, stand still, 'd'rop, 'm'agic, or 'r'elease.", "控制一隻怪物，改為操縱牠而不是玩家自身。可執行的動作有移動、原地不動、d 丟棄、m 施法，或 r 解除控制。" },

        /* Paladin/Rogue: Heroism has a different level threshold to Priest. */
        { "Heroism", "Cures 10 points of damage, removes fear and, from character level 15, grants you heroism (resistance to fear, and a bonus of +12 to-hit) for a level-dependent duration.", "恢復 10 點生命值並消除恐懼；角色等級達到 15 後，還會獲得英雄氣概（抵抗恐懼，命中加值提高 12 點），持續時間取決於角色等級。" },
        { "Single Combat", "Encases you and the targeted monster in a stone cell from which only the winner can emerge.", "將你與指定怪物封入石室，只有獲勝的一方能夠離開。" },
        { "Object Detection", "Detects all objects in the immediate area.", "偵測附近的所有物品。" },
        { "Detect Stairs", "Detects all stairs in a wide area.", "偵測較大範圍內的所有樓梯。" },
        { "Hit and Run", "Prepares you to teleport a short distance after stealing from a monster.", "準備在從怪物身上偷竊後，立即短距離傳送。" },

        /* Ranger Herbal Curing differs from Druid: match exact full text. */
        { "Herbal Curing", "Cures all cuts, poison, stunning and black breath.", "治癒所有割傷、中毒、暈眩與黑色吐息。" },
        { "Cover Tracks", "Prevents you from leaving scent for monsters to track you by for a level-dependent duration.", "在隨角色等級變化的一段時間內，不再留下可供怪物追蹤的氣味。" },
        { "Create Arrows", "Allows the player to turn a staff into a stack of arrows, with quality depending on staff level and player level.", "將一根法杖轉化為一疊箭矢，箭矢品質取決於法杖等級及角色等級。" },
        { "Decoy", "Leaves a magical decoy that looks and sounds like the player.", "留下一個外觀與聲音都像玩家本人的魔法誘餌。" },
        { "Brand Ammunition", "Brands one stack of ammunition with fire, cold, or poison (selected at random) and at the same time attempts to improve the to-hit bonus and the to-dam bonus of the same ammunition.  The spell has no effect if the ammunition is already branded, has a slay, is broken, or is cursed.", "為一疊彈藥隨機附加火焰、寒冷或毒素屬性，同時嘗試提高這疊彈藥的命中與傷害加值。若彈藥已有元素附魔、帶有對特定怪物的剋制效果、已損壞或受到詛咒，這個法術就不會生效。" },
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
