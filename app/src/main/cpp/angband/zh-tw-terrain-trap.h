/* Display-only Angband terrain/trap knowledge translations.
 * Canonical terrain.txt and trap.txt records, effects and save IDs stay English.
 */
#ifndef ANGDROID_ZH_TW_TERRAIN_TRAP_H
#define ANGDROID_ZH_TW_TERRAIN_TRAP_H
#include <stddef.h>
#include <string.h>

struct angdroid_terrain_trap_pair { const char *en; const char *zh; };
static inline const char *angdroid_terrain_trap_lookup(
    const char *en, int zh_tw, const struct angdroid_terrain_trap_pair *pairs,
    size_t count)
{
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < count; ++i)
        if (strcmp(en, pairs[i].en) == 0) return pairs[i].zh;
    return en;
}

static inline const char *angdroid_terrain_name(const char *en, int zh_tw)
{
    static const struct angdroid_terrain_trap_pair terrain_names[] = {
        { "unknown grid", "未知地形" },
        { "open floor", "開闊地面" },
        { "closed door", "關閉的門" },
        { "open door", "開啟的門" },
        { "broken door", "損壞的門" },
        { "up staircase", "上行樓梯" },
        { "down staircase", "下行樓梯" },
        { "General Store", "雜貨店" },
        { "Armoury", "防具店" },
        { "Weapon Smiths", "武器店" },
        { "Bookseller", "書店" },
        { "Alchemy Shop", "鍊金商店" },
        { "Magic Shop", "魔法商店" },
        { "Black Market", "黑市" },
        { "Home", "住家" },
        { "secret door", "暗門" },
        { "pile of rubble", "瓦礫堆" },
        { "magma vein", "岩漿礦脈" },
        { "quartz vein", "石英礦脈" },
        { "magma vein with treasure", "藏寶的岩漿礦脈" },
        { "quartz vein with treasure", "藏寶的石英礦脈" },
        { "granite wall", "花崗岩牆" },
        { "permanent wall", "永久牆" },
        { "lava", "熔岩" },
        { "pile of passable rubble", "可通行的瓦礫堆" },
    };
    return angdroid_terrain_trap_lookup(en, zh_tw, terrain_names,
                                       sizeof terrain_names / sizeof terrain_names[0]);
}

/* Terrain lore is keyed by its canonical feature name, not parser-joined text,
 * because original feature descriptions may span several desc: directives. */
static inline const char *angdroid_terrain_lore(
    const char *terrain_name, const char *original, int zh_tw)
{
    static const struct angdroid_terrain_trap_pair terrain_lore[] = {
        { "open floor", "開闊的地面或裸露的土地，任何生物都能通行，也可能放置陷阱或物品，不會阻擋視線。" },
        { "closed door", "門可能上鎖，因此不容易通過。解除陷阱的技能會影響開鎖能力，也可以用十字鎬破門，或施法炸開。門也會拖慢怪物；上鎖能為你爭取寶貴時間。" },
        { "open door", "已經開啟的門，只要保持開啟，玩家、怪物、法術及投射物都能通過。" },
        { "broken door", "這扇門損壞嚴重，已經無法關閉。" },
        { "up staircase", "通往地城較淺樓層的樓梯。" },
        { "down staircase", "通往地城較深樓層的樓梯。" },
        { "General Store", "雜貨店販售食物、彈藥與照明用品。" },
        { "Armoury", "這裡販售的防具能減輕敵人攻擊造成的傷害。" },
        { "Weapon Smiths", "熱氣逼人、瀰漫刺鼻氣味的工坊打造近戰及遠程武器。" },
        { "Bookseller", "這是一處安靜的避風港，書架上擺滿神祕的魔法典籍。" },
        { "Alchemy Shop", "昏暗而散發香氣的商店，專門交易藥水和卷軸。" },
        { "Magic Shop", "這家店販售蘊含魔力的裝置與飾品。" },
        { "Black Market", "踏入這間聲名狼藉的店時，請看好錢包、留意背後；別期待友善服務或划算交易。" },
        { "Home", "這是你在中土世界的一處安全住所；除了隨身攜帶之外，只有這裡能存放物品。" },
        { "secret door", "這扇門在不熟悉的人眼中，看起來就像花崗岩牆。" },
        { "pile of rubble", "會遮擋視線，也能阻擋投射物、魔法箭和射線；可以挖掘或用魔法清除成普通地面。" },
        { "magma vein", "較柔軟的岩層，可挖掘或以魔法清除；沒有實體的怪物能通過，但會阻擋法術、投射物及視線。" },
        { "quartz vein", "較硬的岩層，可挖掘或以魔法清除；沒有實體的怪物能通過，但會阻擋法術、投射物及視線。" },
        { "magma vein with treasure", "較柔軟的岩層，含有可見的寶藏。可挖掘或用魔法清除；無實體怪物能通過，但會阻擋法術、投射物及視線。" },
        { "quartz vein with treasure", "較硬的岩層，含有可見的寶藏。可挖掘或用魔法清除；無實體怪物能通過，但會阻擋法術、投射物及視線。" },
        { "granite wall", "堅硬的岩層，可挖掘或用魔法清除；無實體怪物能通過，但會阻擋法術、投射物及視線。" },
        { "permanent wall", "大多數牆壁都能挖通，唯獨這種牆無法穿透。地城周圍的邊界與部分特殊房間就是由這種牆構成。" },
        { "lava", "一池閃耀火光的滾燙熔岩；踏入其中非常危險！" },
        { "pile of passable rubble", "會遮擋視線，阻擋投射物、魔法箭及射線，也會縮小範圍法術的影響半徑。可挖掘或用魔法清除，玩家與怪物仍可穿過。" },
    };
    const char *translated;
    if (!original || !zh_tw || !terrain_name) return original;
    /* An unrecognized feature name keeps the original descriptive text. */
    translated = angdroid_terrain_trap_lookup(terrain_name, zh_tw,
        terrain_lore, sizeof terrain_lore / sizeof terrain_lore[0]);
    return translated == terrain_name ? original : translated;
}

static inline const char *angdroid_trap_name(const char *en, int zh_tw)
{
    static const struct angdroid_terrain_trap_pair trap_names[] = {
        { "no trap", "沒有陷阱" },
        { "glyph of warding", "防護符文" },
        { "decoy", "魔法誘餌" },
        { "door lock", "門鎖" },
        { "web", "蜘蛛網" },
        { "trap door", "落穴暗門" },
        { "pit", "陷坑" },
        { "spiked pit", "尖刺陷坑" },
        { "poison pit", "毒刺陷坑" },
        { "rune of summon foe", "召敵符文" },
        { "rune of summoning", "召喚符文" },
        { "rune of necromancy", "死靈符文" },
        { "rune of dragonsong", "龍歌符文" },
        { "hellhole", "地獄裂隙" },
        { "teleport rune", "傳送符文" },
        { "fire trap", "火焰陷阱" },
        { "acid trap", "強酸陷阱" },
        { "slow dart", "減速飛鏢" },
        { "strength loss dart", "力量削弱飛鏢" },
        { "dexterity loss dart", "敏捷削弱飛鏢" },
        { "constitution loss dart", "體質削弱飛鏢" },
        { "blinding gas trap", "致盲毒氣陷阱" },
        { "confusion gas trap", "混亂毒氣陷阱" },
        { "poison gas trap", "毒氣陷阱" },
        { "sleep gas trap", "催眠氣體陷阱" },
        { "aggravation trap", "驚動怪物陷阱" },
        { "siren", "警報器" },
        { "mine trap", "地雷陷阱" },
        { "blast trap", "爆破陷阱" },
        { "mind blasting trap", "精神衝擊陷阱" },
        { "brain smashing trap", "腦部粉碎陷阱" },
        { "rock fall trap", "落石陷阱" },
        { "earthquake trap", "地震陷阱" },
        { "block fall trap", "落石塊陷阱" },
        { "area blast trap", "區域爆破陷阱" },
        { "blinding flash trap", "閃光致盲陷阱" },
        { "blinding trap", "黑暗致盲陷阱" },
        { "mana drain trap", "法力吸取陷阱" },
        { "knife trap", "刀刃陷阱" },
        { "petrifying trap", "石化陷阱" },
    };
    return angdroid_terrain_trap_lookup(en, zh_tw, trap_names,
                                       sizeof trap_names / sizeof trap_names[0]);
}

static inline const char *angdroid_trap_lore(const char *en, int zh_tw)
{
    static const struct angdroid_terrain_trap_pair trap_lore[] = {
        { "A magical rune which monsters cannot pass without difficulty.", "魔法符文會讓怪物難以通過。" },
        { "A magical decoy which sounds and looks like the player.", "這個魔法誘餌的外觀和聲音都像玩家。" },
        { "A device which makes a closed door harder to open.", "一種讓關閉的門更難打開的裝置。" },
        { "A tangle of sticky spider webs.", "一團黏稠、糾結的蜘蛛網。" },
        { "It looks like floor, but step incautiously and you will fall through.", "看似普通地面，但一不小心踏上去就會掉下去。" },
        { "A hole dug to snare the unwary.", "挖來困住疏忽者的陷坑。" },
        { "A hole dug to snare the unwary, with spikes.", "設有尖刺、用來困住疏忽者的陷坑。" },
        { "A hole dug to snare the unwary, with poisoned spikes.", "設有毒刺、用來困住疏忽者的陷坑。" },
        { "A magical rune which calls a monster to attack you.", "這道魔法符文會召來一隻怪物攻擊你。" },
        { "A magical rune which calls up monsters to attack you.", "這道魔法符文會召來多隻怪物攻擊你。" },
        { "A magical rune which calls the undead to attack you.", "這道魔法符文會召來不死生物攻擊你。" },
        { "A magical rune which calls dragons to attack you.", "這道魔法符文會召來龍類攻擊你。" },
        { "A magical rune which calls demons to attack you.", "這道魔法符文會召來惡魔攻擊你。" },
        { "A magical rune which teleports you to a random location.", "這道魔法符文會把你傳送到隨機位置。" },
        { "A strange coloured spot on the ground - beware its fiery attack.", "地上有奇怪的斑點；小心它的火焰攻擊。" },
        { "A strange coloured spot on the ground - beware its acidic attack.", "地上有奇怪的斑點；小心它的強酸攻擊。" },
        { "A trap which shoots slowing darts.", "會發射減速飛鏢的陷阱。" },
        { "A trap which shoots damaging darts.", "會發射造成傷害的飛鏢的陷阱。" },
        { "A trap that releases gas to blind you.", "會釋放使你失明的氣體。" },
        { "A trap that releases gas to confuse you.", "會釋放使你混亂的氣體。" },
        { "A trap that releases gas to poison you.", "會釋放使你中毒的氣體。" },
        { "A trap that releases gas to put you to sleep.", "會釋放使你昏睡的氣體。" },
        { "A trap that wakes nearby monsters and impels them toward you.", "會喚醒附近怪物並引誘牠們朝你靠近。" },
        { "A trap that wakes nearby monsters.", "會喚醒附近怪物的陷阱。" },
        { "A trap that blows up the floor.", "會炸毀地面的陷阱。" },
        { "A trap that creates an explosion.", "會引發爆炸的陷阱。" },
        { "A trap that blasts your senses.", "會衝擊你感官的陷阱。" },
        { "A trap that drops a boulder on you.", "會讓巨石掉落砸向你的陷阱。" },
        { "A trap that causes an earthquake.", "會引發地震的陷阱。" },
        { "A pressure plate that drops a stone block behind you.", "踩下壓力板後，石塊會落在你身後。" },
        { "A trap that clears the surrounding area.", "會清除周圍區域的陷阱。" },
        { "A trap that creates a ball of light.", "會產生一團光球的陷阱。" },
        { "A trap that envelops you in darkness.", "會讓你籠罩在黑暗中的陷阱。" },
        { "A trap that drains your mana.", "會吸取你法力的陷阱。" },
        { "A trap designed to bleed you to death.", "設計來讓你流血致死的陷阱。" },
        { "A trap that stuns you and gives you stoneskin.", "會使你暈眩，並讓你獲得石膚效果的陷阱。" },
    };
    return angdroid_terrain_trap_lookup(en, zh_tw, trap_lore,
                                       sizeof trap_lore / sizeof trap_lore[0]);
}
#endif
