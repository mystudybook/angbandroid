/* FAangband-only player ability descriptions, UI translation only.
 * Exact English source strings; never change property codes or gameplay data.
 * The original Angband ability dictionary retains precedence.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_ABILITIES_H
#define ANGDROID_ZH_TW_FAANGBAND_ABILITIES_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_FAANGBAND_ABILITY_COUNT 64
struct angdroid_fa_ability_pair { const char *english; const char *traditional; };
static inline const char *angdroid_faangband_ability_lookup(
    const char *english, int zh_tw, const struct angdroid_fa_ability_pair *pairs, size_t count)
{
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < count; ++i)
        if (strcmp(english, pairs[i].english) == 0) return pairs[i].traditional;
    return english;
}
static inline const char *angdroid_faangband_ability_name(const char *english, int zh_tw)
{
    static const struct angdroid_fa_ability_pair map[ANGDROID_FAANGBAND_ABILITY_COUNT] = {
        { "Sword Preference", "刀劍專精" },
        { "Polearm Preference", "斧槍專精" },
        { "Hafted Preference", "鈍器專精" },
        { "Sling Preference", "投石索專精" },
        { "Bow Preference", "弓箭專精" },
        { "Crossbow Preference", "弩箭專精" },
        { "Sword Distaste", "不擅刀劍" },
        { "Polearm Distaste", "不擅斧槍" },
        { "Hafted Distaste", "不擅鈍器" },
        { "Sling Distaste", "不擅投石索" },
        { "Bow Distaste", "不擅弓箭" },
        { "Crossbow Distaste", "不擅弩箭" },
        { "Hardy", "堅韌" },
        { "Flying", "飛行" },
        { "Shadow", "暗影體質" },
        { "Treeish", "樹木之軀" },
        { "Shapeshifter", "熊形變身" },
        { "Plains Fighter", "平原戰士" },
        { "Elven", "精靈天賦" },
        { "Dwarven", "矮人天賦" },
        { "Forest Fighter", "森林戰士" },
        { "Fast Bow Shots", "快速弓射" },
        { "Very Fast Bow Shots", "極速弓射" },
        { "Fast Sling Shots", "快速投石" },
        { "Very Fast Sling Shots", "極速投石" },
        { "Fast Crossbow Shots", "快速弩射" },
        { "Very Fast Crossbow Shots", "極速弩射" },
        { "Backstab", "背刺" },
        { "Unarmed Combat", "徒手格鬥" },
        { "Magic Device Expertise", "魔法裝置專家" },
        { "Full Spellcaster", "完美施法者" },
        { "Pious", "虔誠" },
        { "Trap Setting", "陷阱設置" },
        { "Woodsmanship", "林地技巧" },
        { "Specialization [1]", "額外專精 [1]" },
        { "Armor Mastery", "護甲精通" },
        { "Shield Mastery", "盾牌精通" },
        { "Armor Proficiency", "護甲熟練" },
        { "Evasion", "閃避技巧" },
        { "Magic Resistance", "魔法抗性" },
        { "Phasewalking", "相位行走" },
        { "Armsman", "武器大師" },
        { "Fast Attacking", "快速攻擊" },
        { "Marksman", "神射手" },
        { "Piercing Shot", "穿透射擊" },
        { "Mighty Throw", "強力投擲" },
        { "Power Strike", "強力打擊" },
        { "Martial Arts", "武術" },
        { "Mana Burn", "法力燃燒" },
        { "Rapid Fire", "速射" },
        { "Beguile", "迷惑術" },
        { "Enhance Magic", "增幅魔法" },
        { "Power Siphon", "力量汲取" },
        { "Heighten Magic", "施法強化" },
        { "Soul Siphon", "靈魂汲取" },
        { "Harmony", "調和" },
        { "Channeling", "法力引導" },
        { "Athletics", "強健體魄" },
        { "Clarity", "清晰思緒" },
        { "Fury", "狂怒" },
        { "Meditation", "冥想" },
        { "Regeneration", "生命再生" },
        { "Extra Trap", "額外陷阱" },
        { "Holy Light", "神聖之光" },
    };
    return angdroid_faangband_ability_lookup(english, zh_tw, map,
        ANGDROID_FAANGBAND_ABILITY_COUNT);
}
static inline const char *angdroid_faangband_ability_description(const char *english, int zh_tw)
{
    static const struct angdroid_fa_ability_pair map[ANGDROID_FAANGBAND_ABILITY_COUNT] = {
        { "You are slightly more skilled with swords than other melee weapons.", "使用刀劍類武器時，近戰技巧略高於使用其他近戰武器。" },
        { "You are slightly more skilled with axes and polearms than other melee weapons.", "使用斧頭及長柄武器時，近戰技巧略高於使用其他近戰武器。" },
        { "You are slightly more skilled with blunt weapons than other melee weapons.", "使用鈍擊武器時，近戰技巧略高於使用其他近戰武器。" },
        { "You are slightly more skilled with slings than other ranged weapons.", "使用投石索時，遠程技巧略高於使用其他遠程武器。" },
        { "You are slightly more skilled with bows than other ranged weapons.", "使用弓時，遠程技巧略高於使用其他遠程武器。" },
        { "You are slightly more skilled with crossbows than other ranged weapons.", "使用弩時，遠程技巧略高於使用其他遠程武器。" },
        { "You are slightly less skilled with swords than other melee weapons.", "使用刀劍類武器時，近戰技巧略低於使用其他近戰武器。" },
        { "You are slightly less skilled with axes and polearms than other melee weapons.", "使用斧頭及長柄武器時，近戰技巧略低於使用其他近戰武器。" },
        { "You are slightly less skilled with blunt weapons than other melee weapons.", "使用鈍擊武器時，近戰技巧略低於使用其他近戰武器。" },
        { "You are slightly less skilled with slings than other ranged weapons.", "使用投石索時，遠程技巧略低於使用其他遠程武器。" },
        { "You are slightly less skilled with bows than other ranged weapons.", "使用弓時，遠程技巧略低於使用其他遠程武器。" },
        { "You are slightly less skilled with crossbows than other ranged weapons.", "使用弩時，遠程技巧略低於使用其他遠程武器。" },
        { "You recover quickly from poison and cuts, and succumb slowly to the Black Breath.", "你能較快從中毒與割傷中恢復，且黑之吐息對你的侵蝕較慢。" },
        { "You can fly.", "你能飛行。" },
        { "You are incredibly stealthy.", "你的潛行能力極為出色。" },
        { "Your bare hands tear through stone and you are almost impossible to detect in forests, but you are vulnerable to fire and falling.", "你的徒手攻擊能穿透岩石，在森林中幾乎不會被察覺，但對火焰與墜落傷害較為脆弱。" },
        { "You can take the form of a bear (use the '!' command).", "你可以變成熊的形態（使用「!」指令）。" },
        { "You are more able to hit monsters while in plain regions.", "在平原地區，你更容易擊中怪物。" },
        { "You have an inborn ability to move freely through forests.", "你天生能在森林中自由行動。" },
        { "You are able to move freely through rubble, and to evade attacks while in mountain regions.", "你能在瓦礫間自由行動，並在山區更容易閃避攻擊。" },
        { "You are more able to hit monsters while in forests.", "在森林地區，你更容易擊中怪物。" },
        { "Your shooting speed with a bow increases quickly with Dexterity.", "使用弓時，你的射擊速度會隨敏捷提高而較快增加。" },
        { "Your shooting speed with a bow increases very quickly with Dexterity.", "使用弓時，你的射擊速度會隨敏捷提高而非常快速地增加。" },
        { "Your shooting speed with a sling increases quickly with Dexterity.", "使用投石索時，你的射擊速度會隨敏捷提高而較快增加。" },
        { "Your shooting speed with a sling increases very quickly with Dexterity.", "使用投石索時，你的射擊速度會隨敏捷提高而非常快速地增加。" },
        { "Your shooting speed with a crossbow increases quickly with Dexterity.", "使用弩時，你的射擊速度會隨敏捷提高而較快增加。" },
        { "Your shooting speed with a crossbow increases very quickly with Dexterity.", "使用弩時，你的射擊速度會隨敏捷提高而非常快速地增加。" },
        { "You effectively backstab unsuspecting creatures in melee.", "在近戰中，你能有效偷襲毫無防備的生物。" },
        { "You are skilled in hand to hand combat, and are somewhat talented with slings.", "你精通徒手格鬥，也相當擅長使用投石索。" },
        { "You have a great understanding of magical devices and can recharge them very reliably.", "你對魔法裝置有深入了解，能以極高可靠度為它們重新充能。" },
        { "You are an expert spellcaster capable of a perfect success rate and have a large reserve of mana.", "你是精通施法的專家，可以達到完全成功的施法機率，並擁有充沛的法力儲量。" },
        { "You carry a pious authority.  You glow with an inner light at level 35 (Priest) or level 45 (Paladin).", "你具有虔誠的威儀。祭司達到 35 級，或聖騎士達到 45 級時，會從內在散發光芒。" },
        { "You can set monster traps.", "你可以設置對付怪物的陷阱。" },
        { "You are attuned to nature.  You easily move and hide in trees.", "你與大自然十分契合，能輕鬆地在樹林中移動與藏身。" },
        { "You can learn an extra specialty abilty at level 1.", "你可以在第 1 級時額外學會一項專精能力。" },
        { "Increases the armor class of body armor by 2/3.", "軀幹護甲的防禦等級增加三分之二。" },
        { "Doubles shield AC, increases chance of shield deflection of missiles, and increases damage from shield bashes (more so for heavier shields).", "盾牌防禦等級加倍，提高用盾牌偏轉飛射武器的機率，並增加盾擊傷害（盾牌越重，提升越大）。" },
        { "Allows an extra 5 pounds of armor before mana penalties for armor use take effect.", "在護甲重量造成施法法力懲罰之前，你可以額外承受 5 磅護甲重量。" },
        { "Gives up to 40% chance to avoid melee attacks and up to 50% chance to avoid missile weapons when lightly armored (15 + .3 per level pounds of armor).", "穿著輕甲（護甲重量以磅計，不超過 15 + 每級 0.3 磅）時，最多有 40% 機率避開近戰攻擊、50% 機率避開遠程武器。" },
        { "Halves the distance between your saving throw and perfect, or if close enough makes it perfect.", "使你的豁免判定與完美豁免之間的差距減半；若已足夠接近，便能達到完美豁免。" },
        { "Causes you to speed up after teleporting.  Gives resistance to hostile teleportation.", "傳送後會獲得速度加成，並能抵抗敵對傳送效果。" },
        { "Grants one in 6 chance of a critical hit with melee weapons against visible monsters.", "使用近戰武器對付可見怪物時，有六分之一機率造成致命一擊。" },
        { "Gives you a (number of blows) in 8 chance of an extra melee attack each round.  Works armed and unarmed.", "每回合有「攻擊次數／8」的機率額外發動一次近戰攻擊；持武器與徒手時皆有效。" },
        { "Grants one in 6 chance of a critical hit in ranged combat against visible monsters.", "使用遠程攻擊對付可見怪物時，有六分之一機率造成致命一擊。" },
        { "Gives your successful bow and crossbow attacks a chance of continuing to additional targets (up to a maximum of the bow multiplier).", "弓與弩的攻擊命中時，有機會繼續貫穿並擊中其他目標，最多不超過弓的倍率數量。" },
        { "Allows you to hurl throwing weapons and other objects twice as far.", "投擲武器與其他物品的投擲距離加倍。" },
        { "Gives you a one in 8 chance of a better-than-usual barehanded attack, with maximised damage dice and a 2 in 3 chance to attempt to confuse the struck monster.", "徒手攻擊有八分之一機率觸發更強的攻擊，傷害骰固定取最大值，並有三分之二機率嘗試使被擊中的怪物混亂。" },
        { "Gives you a one in 6 chance of a better-than-usual barehanded attack, with maximised damage dice and a 2 in 3 chance to attempt to confuse the struck monster.", "徒手攻擊有六分之一機率觸發更強的攻擊，傷害骰固定取最大值，並有三分之二機率嘗試使被擊中的怪物混亂。" },
        { "Adds a damage die to critical hits against spellcasting monsters.", "對會施法的怪物造成致命一擊時，額外增加一顆傷害骰。" },
        { "Trades some shooting accuracy for faster shooting with a short or long bow, even faster with increasing dexterity.", "使用短弓或長弓時，可犧牲部分射擊命中率換取更快射速；敏捷越高，射速還會進一步提升。" },
        { "Enhances your power to slow, sleep, confuse, stun, hold or polymorph your enemies by 50%.", "對敵人施加緩速、睡眠、混亂、震懾、定身或變形效果的能力提升 50%。" },
        { "Increases the effects (by 25-100%) and durations (by one third) of beneficial magical effects.", "有益魔法效果增強 25% 至 100%，持續時間延長三分之一。" },
        { "When a monster casts a spell, gives you mana equal to a fifth of the monster's spell power.", "怪物施放法術時，你會獲得相當於該怪物法術威力五分之一的法力。" },
        { "Gives a boost to the effective player level for spells when a spell is cast.  This boost is at least 1, increases with every spell cast up to a maximum boost of 20, and decreases every turn with no spell cast.", "施法時提高法術的有效玩家等級。加成至少為 1，每次施法都會增加，最高為 20；每回合若沒有施法，加成便會降低。" },
        { "When you slay a living, non-stupid, non-demon monster, gives you mana equal to a thirtieth of the monster's maximum HP.  Causes damage when it makes your mana exceed your maximum mana by more than your level.", "擊殺具有生命、並非愚笨且不是惡魔的怪物時，獲得相當於其最大生命值三十分之一的法力。若法力超出上限的數量超過自身等級，則會受到傷害。" },
        { "When you cast a spell, regain a percentage of your hitpoints equal to a third of that spell's mana (capped at 10%).", "每次施法可恢復一定比例的生命值，恢復百分比等於該法術法力消耗的三分之一，最高為 10%。" },
        { "Gives a boost to the effective player level for spells when your mana level is high.  This boost is 5 plus 1 for every 5 levels at full mana, reduced by the square of your mana as a fraction of maximum mana.", "法力充沛時，提高法術的有效玩家等級；滿法力時加成為 5，每 5 級再增加 1，並依目前法力與最大法力之比的平方調整。" },
        { "Increases Dexterity and Constitution by 2 points and sustains them half the time.  Adds 5% to maximum hitpoints.", "敏捷與體質各增加 2 點，並有一半時間受到維持屬性效果保護；最大生命值增加 5%。" },
        { "Increases Intelligence and Wisdom by 2 points and sustains them half the time.  Adds 5% to maximum mana.", "智力與智慧各增加 2 點，並有一半時間受到維持屬性效果保護；最大法力增加 5%。" },
        { "Gives you a speed boost after melee attacks or being damaged.  Maximum boost varies from 3 up to 10 with player level.", "近戰攻擊或受到傷害後會獲得速度加成；最高加成隨等級變化，介於 +3 至 +10。" },
        { "Doubles mana regeneration; cumulative with other forms of regeneration.", "法力恢復速度加倍，且能與其他恢復加成累積。" },
        { "Doubles hit point regeneration; cumulative with other forms of regeneration.", "生命值恢復速度加倍，且能與其他恢復加成累積。" },
        { "Allows you to set an additional monster trap, and to learn to set more advanced types of traps.", "可額外設置一個怪物陷阱，並學會設置更進階的陷阱類型。" },
        { "Increases light radius.  Causes undead, evil, and light-sensitive creatures to take extra melee damage and makes them fear your light-based spells.  Grants 40% resistance to light.", "增加照明範圍。對不死生物、邪惡生物與畏光生物造成額外近戰傷害，也使牠們畏懼你的光系法術；並提供 40% 的光系抗性。" },
    };
    return angdroid_faangband_ability_lookup(english, zh_tw, map,
        ANGDROID_FAANGBAND_ABILITY_COUNT);
}
#endif /* ANGDROID_ZH_TW_FAANGBAND_ABILITIES_H */
