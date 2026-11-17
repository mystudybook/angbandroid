/* FAangband batch 171: all 77 remaining exclusive monster names and
 * 76 source-exact lore descriptions.  Test production lookup directly. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-faangband-monster-flavor.h"
#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"
#include "../../app/src/main/cpp/angband/zh-tw-unique-monsters.h"
static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)
int main(void)
{
    static const struct {
        const char *en, *zh, *source, *description;
    } cases[] = {
        { "skeleton lord", "骷髏領主",
          NULL, NULL },
        { "gazing eye", "凝視之眼",
          "A lidless gazing eye, crackling with mana.", "一顆沒有眼瞼、凝視著四周的眼睛，周圍劈啪閃爍著魔力。" },
        { "blue worm", "藍色幼龍",
          "It has a form that legends are made of.  Its still-tender scales are a deep blue in hue.  Sparks crackle along its length.", "牠有著傳說中的龍形，尚未硬化的鱗片呈深藍色，電光沿著身軀劈啪閃爍。" },
        { "white worm", "白色幼龍",
          "It has a form that legends are made of.  Its still-tender scales are a frosty white in hue.  Icy blasts of cold air come from it as it breathes.", "牠有著傳說中的龍形，尚未硬化的鱗片呈霜白色，呼吸時吐出陣陣冰冷寒氣。" },
        { "green worm", "綠色幼龍",
          "It has a form that legends are made of.  Its still-tender scales are a deep green in hue.  Foul gas seeps through its scales.", "牠有著傳說中的龍形，尚未硬化的鱗片呈深綠色，惡臭的毒氣從鱗片間滲出。" },
        { "Othrod, Lord of the orcs", "半獸人領主奧斯羅德",
          "He is an Uruk of power and great cunning.  He doesn't think much of you, but then he doesn't think much at all.", "他是力大無窮又狡詐的強獸人。他不把你放在眼裡，不過他似乎也不大會思考。" },
        { "halfling slinger", "半身人投石手",
          "A strong, wiry hobbit whirling a sling around with enormous speed.", "這名強壯而精瘦的哈比人正以驚人的速度揮舞投石索。" },
        { "black worm", "黑色幼龍",
          "It has a form that legends are made of.  Its still-tender scales are a darkest black hue.  Acid drips from its body.", "牠有著傳說中的龍形，尚未硬化的鱗片漆黑如墨，酸液從身上滴落。" },
        { "gold worm", "金色幼龍",
          "It has a form that legends are made of.  Its still-tender scales are a tarnished gold hue, and light is reflected from its form.", "牠有著傳說中的龍形，尚未硬化的鱗片呈黯淡金色，表面反射著光芒。" },
        { "red worm", "紅色幼龍",
          "It has a form that legends are made of.  Its still-tender scales are a deepest red hue.  Heat radiates from its form.", "牠有著傳說中的龍形，尚未硬化的鱗片呈深紅色，周身散發著熱力。" },
        { "corvid of shadow", "暗影鴉",
          "An ever-wakeful flying eye of Sauron, with a haunting cry that alerts his minions to your presence.", "索倫永不歇息的飛行眼線，牠令人毛骨悚然的叫聲會讓索倫的爪牙知道你的位置。" },
        { "Orcogar, son of Orcobal", "奧科巴爾之子奧科加",
          "A large and powerful orc.  He looks just like his daddy.  He is tall and fast, but fortunately blessed with orcish brains.", "這名半獸人體型高大、力量強勁，長相和父親一模一樣。他又高又快，幸好智力也和半獸人差不多。" },
        { "multi-hued worm", "多彩幼龍",
          "It has a form that legends are made of.  Beautiful scales of shimmering and magical colours cover it.", "牠有著傳說中的龍形，全身覆蓋著閃爍魔法光彩的美麗鱗片。" },
        { "dark naga", "黑暗娜迦",
          "A giant snake-like figure with a woman's torso.  Not as strong as other nagas, but more talented in magic.", "這隻巨蛇般的生物長著女性的上半身。牠不如其他娜迦強壯，卻更擅長魔法。" },
        { "Orcobal the Mighty", "強大的奧科巴爾",
          "He is the champion fighter of the expeditionary orc forces of Angband.  He leers and swaggers as he approaches you.", "他是安格班遠征半獸人軍團的冠軍戰士，一邊獰笑、一邊趾高氣揚地朝你走來。" },
        { "animist", "萬物有靈論者",
          "A believer in the power of animals.", "他信奉動物所擁有的力量。" },
        { "nightstalker", "暗夜潛獵者",
          "A great black cat, hidden from ordinary sight.  Twin scimitars gleam in its yawning mouth, and its hunger is never satisfied.", "這頭巨大的黑貓隱藏在普通視線之外，張開的大嘴裡閃著兩把彎刀般的利齒，飢餓永遠得不到滿足。" },
        { "giant paladin", "巨人聖騎士",
          "A giant both devout and skillful in battle.", "這名巨人既虔誠，又精通戰鬥。" },
        { "Easterling assassin", "東夷刺客",
          "A man of the east clothed in black who moves with blinding speed.", "這名來自東方的男子身穿黑衣，動作快得讓人眼花撩亂。" },
        { "shapeshifter", "變形怪",
          "He exists only to change.", "他的存在只為了不斷改變形體。" },
        { "necromancer of Ered Engrin", "鐵山死靈法師",
          "A necromancer recruited by Morgoth to the cold, bitter mountains of the north.", "這名死靈法師受魔苟斯招募，來到北方苦寒的鐵山。" },
        { "falcon of Ered Engrin", "鐵山獵隼",
          "Falcons, like other creatures of blood, are often used as spies by Morgoth.  Beware this wrathful hunter!  Flee its matchless speed!", "獵隼和其他有血肉的生物一樣，經常被魔苟斯用來刺探情報。小心這隻憤怒的獵手！快逃離牠無與倫比的速度！" },
        { "venom mold", "劇毒黴菌",
          "A highly dangerous mold; its poison is mortal for the unprepared.", "這種黴菌極度危險；毫無準備的人一旦中毒，便可能喪命。" },
        { "Easterling archer", "東夷弓箭手",
          "A man of the east clad in black and brown and carrying a longbow.", "這名東方男子身穿黑褐色衣服，手持長弓。" },
        { "cold drake", "寒冰飛龍",
          "A large dragon, scales gleaming bright white.", "這頭巨龍身形龐大，鱗片閃耀著明亮的白光。" },
        { "lightning drake", "閃電飛龍",
          "A large dragon, scales tinted deep blue.", "這頭巨龍身形龐大，鱗片呈深藍色。" },
        { "venom drake", "毒霧飛龍",
          "A large dragon, scales tinted deep green.", "這頭巨龍身形龐大，鱗片呈深綠色。" },
        { "swamp quylthulg", "沼澤奎爾薩格怪",
          "It is a squishy, stinking mound of flesh.", "牠是一大團濕黏惡臭的血肉。" },
        { "fire drake", "烈焰飛龍",
          "A large dragon, scales tinted deep red.", "這頭巨龍身形龐大，鱗片呈深紅色。" },
        { "clamor drake", "轟鳴飛龍",
          "A large dragon with scales of gleaming gold.", "這頭巨龍身形龐大，鱗片閃耀著金色光輝。" },
        { "corrosive drake", "腐蝕飛龍",
          "A large dragon, with scales of deepest black.", "這頭巨龍身形龐大，鱗片漆黑如墨。" },
        { "gnawing bug", "啃噬蟲",
          "AAAAHHHH!  They're everywhere!  You feel a rising sense of panic as you desperately try to hunt the last bug down.", "啊啊啊！牠們到處都是！你拼命尋找最後一隻蟲子，心中的恐慌也逐漸升高。" },
        { "blade of slaughter", "殺戮之刃",
          "A blade with no wielder you can see, slaughtering where it will.  It is utterly untamable: destroy it, flee, or die.", "一把看不見主人操控的刀刃，任意大開殺戒。牠完全無法馴服；不是摧毀牠，就是逃命，否則便只有死路一條。" },
        { "giant chieftain", "巨人酋長",
          "A great battle-lord of Forodwaith, twenty-five feet tall, who wields a massive two-handed sword that strikes like a bolt of lightning.", "這名來自佛羅德懷斯的偉大戰爭領主身高二十五英呎，揮舞巨大的雙手劍，每一擊都如閃電般猛烈。" },
        { "elemental drake", "元素飛龍",
          "A large dragon, scales shimmering many colours.", "這頭巨龍身形龐大，鱗片閃耀著多種色彩。" },
        { "lord of Ered Engrin", "鐵山邪教領主",
          "An evil priest, dressed all in black.  Deadly spells hit you at an alarming rate as his black spiked mace rains down blow after blow on your pitiful frame.", "這名邪惡的祭司一身黑衣，致命法術以驚人的速度襲來，黑色尖刺釘頭鎚也不斷落在你可憐的身軀上。" },
        { "Eöl, the Dark Elven Smith", "黑暗精靈鐵匠埃歐爾",
          "A lord of the Teleri, Eöl is a mighty metalsmith, the only one to ever forge weapons of meteorite iron.  The mortal enemy of the Eldar, he is currently fleeing their wrath, following the murder of Princess Aredhel of Gondolin.", "埃歐爾是帖勒瑞族領主，也是一位卓越的金屬工匠，世上只有他曾以隕鐵打造武器。他與艾爾達族誓不兩立；在殺害貢多林的亞瑞希爾公主後，正躲避他們的追捕。" },
        { "stone warden", "石之守衛",
          "Angered by the greedy delvings of surface creatures, they seek to rid the dungeon of all who breathe.", "地表生物貪婪的挖掘激怒了牠們，牠們決心把地城裡所有會呼吸的入侵者趕盡殺絕。" },
        { "blue dragon", "藍龍",
          "A huge draconic form.  Lightning crackles along its length.", "這頭巨龍身軀龐大，閃電沿著全身劈啪作響。" },
        { "white dragon", "白龍",
          "A huge draconic form.  Frost covers it from head to tail.", "這頭巨龍身軀龐大，從頭到尾都覆蓋著冰霜。" },
        { "green dragon", "綠龍",
          "A huge draconic form enveloped in clouds of poisonous vapour.", "這頭巨龍身軀龐大，周身籠罩著一團團毒霧。" },
        { "black dragon", "黑龍",
          "A huge draconic form.  Pools of acid melt the floor around it.", "這頭巨龍身軀龐大，牠身邊積聚的酸液正把地面腐蝕融化。" },
        { "red dragon", "紅龍",
          "A huge draconic form.  Wisps of smoke steam from its nostrils and the extreme heat surrounding it makes you gasp for breath.", "這頭巨龍身軀龐大，鼻孔間冒出縷縷煙霧，周圍的酷熱令你喘不過氣。" },
        { "gold dragon", "金龍",
          "A huge draconic form wreathed in a nimbus of light.  Its roar stuns and deafens you.", "這頭巨龍身軀龐大，周圍環繞著光暈，咆哮聲震得你頭暈耳鳴。" },
        { "The Phantom of Eilinel", "艾莉妮爾的幻影",
          "A creature of Sauron in the form of a woman. This phantom was made to appear like Eilinel, the murdered wife of Gorlim the Unhappy, in order to trap him into betraying Barahir and the outlaws of Dorthonion.", "這是索倫以女子形貌製造的生物，外表模仿不幸的戈林被殺害的妻子艾莉妮爾，藉此誘使他背叛巴拉希爾與多索尼安的亡命者。" },
        { "storm troll", "風暴食人妖",
          "The ground shakes as an enormous troll-form thunders into view, hurling ice and lightning in every direction.  It is thirty feet tall, has six heads, and is enraged at your impudence.", "地面震動，一頭龐大的食人妖轟然現身，四面八方投擲冰霜與閃電。牠高達三十英呎、長著六個頭，對你的冒犯憤怒不已。" },
        { "multi-hued dragon", "多彩巨龍",
          "A huge draconic form.  Many colours ripple down its massive frame.  Few live to see another.", "這頭巨龍身軀龐大，繽紛色彩沿著全身流轉，見到牠的人很少還能活著見到下一次。" },
        { "lord of misrule", "亂序之主",
          "A captain of thieves and assassins who has buried all challengers to his control of the underworld. He sharpens his knives as he awaits your attack.", "這名盜賊與刺客的首領埋葬了所有膽敢挑戰其地下世界統治權的人，正磨利刀刃等待你的攻擊。" },
        { "living dungeon", "活體地城",
          "A granitic monstrosity, eating at the very heart of mountains, awoken with rudeness, and now awake in wrath.", "這頭花崗岩巨怪啃噬著山脈的核心，粗暴地被喚醒後，如今正怒不可遏。" },
        { "legendary hobbit rogue", "傳奇哈比盜賊",
          "Cunning and stealthy beyond belief, this infamous rascal could pick the pockets of Smaug himself.", "這名惡名昭彰的無賴狡詐又善於潛行，甚至有本事從史矛革身上偷東西。" },
        { "The Defiler", "褻瀆者",
          "A hulking creature with massive pincers.  The Defiler oozes acidic venom as it smashes the living rock to reach you.", "這頭巨獸身形魁梧，長著巨大鉗爪，一邊滲出酸性毒液，一邊擊碎岩石向你逼近。" },
        { "Errata the Mage", "法師埃拉塔",
          "Errata is a bold and mighty mage.  Her power and guile are legendary.  Hers is the right to face Morgoth in single combat and she will not let you stand in her way.  Defend yourself!", "埃拉塔是勇敢而強大的法師，她的力量與機智皆已成為傳說。她自認有權與魔苟斯單獨決鬥，絕不容許你擋路。保護好自己！" },
        { "winged dragon of bile", "腐液翼龍",
          "An acrid stench fills the air as this immense dragon spreads its wings.  Your courage seems to melt away as you feel you fear and nausea rising.", "這頭巨龍張開雙翼時，刺鼻惡臭瀰漫空中。隨著恐懼與噁心湧上心頭，你感覺勇氣正逐漸消融。" },
        { "Krotus, the Dark Lord", "黑暗之主克羅圖斯",
          "This mighty Balrog commands awesome power, his visage grave enough to shrivel your soul.  You shriek with disbelief as his mastery of death draws you to your grave.  It is truly beyond all but the mightiest of warriors to stand against him and live.", "這名強大的炎魔擁有驚人力量，光是可怖的面容就足以使你靈魂萎縮。牠駕馭死亡的能力將你拖向墳墓，令你難以置信地尖叫。除非是最強大的戰士，否則沒有人能與牠對峙而生還。" },
        { "winged dragon of ice", "寒冰翼龍",
          "An immense dragon capable of awesome destruction.  You have never felt such extreme cold, or witnessed such an icy stare.  Begone quickly or feel its wrath!", "這頭巨龍體型驚人，破壞力極其可怕。你從未感受過如此極寒，也從未見過如此冰冷的目光；快逃，否則便要承受牠的怒火！" },
        { "winged dragon of storms", "風暴翼龍",
          "A vast dragon of power.  Storms and lightning crash around its titanic form.  Deep blue scales reflect the flashes and highlight the creature's great muscles.  It regards you with contempt.", "這頭巨龍力量驚人，暴風與閃電不斷轟擊著龐大身軀四周。深藍鱗片映照著電光，更襯托出牠發達的肌肉。牠輕蔑地注視你。" },
        { "winged dragon of fire", "烈焰翼龍",
          "A vast dragon of immense power.  Fire leaps continuously from its huge form.  The air around it scalds you.  Its slightest glance burns you, and you truly realize how insignificant you are.", "這頭巨龍具有無比強大的力量，巨大的身軀不斷迸出火焰，周圍的空氣燙得令人難受。牠只消瞥你一眼，就像要把你燒焦，讓你深刻明白自己的渺小。" },
        { "mimic (large jeweled chest)", "擬態怪（大型珠寶箱）",
          "A deadly creature that disguises itself as a chest to lure unsuspecting adventurers within reach of its venomous claws and deadly touch.", "這種致命生物偽裝成寶箱，誘使毫無戒心的冒險者靠近，再以帶毒利爪與致命碰觸發動攻擊。" },
        { "winged dragon of chaos", "混沌翼龍",
          "A massive dragon of changing form.  As you watch, it appears first fair and then foul.  Its body is twisted by chaotic forces as it strives to stay real.  Its very existence distorts the universe around it.", "這頭巨龍形態不斷變化，你看著牠忽而華美、忽而醜惡。混沌力量扭曲牠的身體，牠竭力維持實體，光是存在就令周圍的宇宙失去常態。" },
        { "winged dragon of law", "秩序翼龍",
          "A massive dragon of powerful intellect.  It seeks to dominate the universe and despises all other life.  It sees all who do not obey it as mere insects to be crushed underfoot.", "這頭巨龍具有強大智慧，企圖支配整個宇宙，蔑視所有其他生命。在牠眼中，不服從者都只是可以一腳踩扁的蟲子。" },
        { "winged dragon of Ilmen", "伊爾門天穹翼龍",
          "A massive ethereal dragon bred by Morgoth for battle in the region of the stars above Arda.  If it can take on the Sun and the Moon, what chance do you have?", "這頭巨大的靈體龍由魔苟斯培育，準備在阿爾達上方的群星領域作戰。牠若能與太陽和月亮對抗，你又有多少勝算？" },
        { "balrog", "炎魔",
          "Originally of the semi-divine Maiar, this evil spirit swore allegiance to Morgoth at the beginning of time and is now one of his most terrible demonic servants.  With its flaming whip and sword it seeks to destroy you.", "這個邪惡靈體原本是近乎神聖的邁雅，卻在時間之初宣誓效忠魔苟斯，成為他最可怕的惡魔僕從之一。牠揮舞燃燒的長鞭與利劍，企圖摧毀你。" },
        { "winged dragon of annihilation", "湮滅翼龍",
          "A massive dragon of hideous form.  From its shadowy aura comes the stench of the underworld.  Its body is twisted by chaotic forces as it strives to stay real.  Its very existence distorts the universe around it.", "這頭巨龍形貌可怖，陰影般的氣息散發冥界的惡臭。混沌力量扭曲牠的身體，牠竭力維持實體，光是存在就令周圍的宇宙失去常態。" },
        { "winged dragon of balance", "平衡翼龍",
          "A massive dragon, one of the mightiest of dragonkind.  It is thousands of years old and seeks to maintain the Cosmic Balance.  It sees you as an upstart troublemaker without the wisdom to control your actions.  It will destroy you.", "這頭巨龍是龍族中最強大的存在之一，已活了數千年，力求維持宇宙平衡。牠認為你是沒有智慧約束自身行為的自大搗亂者，決心將你毀滅。" },
        { "winged dragon of many colours", "多彩翼龍",
          "A gigantic dragon whose scales shimmer in myriad hues.", "這頭巨龍體型驚人，鱗片閃耀著千變萬化的色彩。" },
        { "winged dragon of power", "威能翼龍",
          "A gigantic wyrm, in command of the forces of Time itself.  You shudder as you turn to face an emperor of Dragons, a despot of Life and Death, and slowly realize that you have just encountered what might quite possibly be your doom.", "這頭巨大的古龍掌控著時間本身的力量。面對龍族皇帝、掌管生死的暴君，你不禁顫抖，逐漸意識到自己或許已遇上末日。" },
        { "Skeleton", "骷髏亡魂",
          "An animated skeleton, creaking as it advances.  You feel you know it and it knows you.", "一具復活的骷髏吱嘎作響地向你走來。你感覺自己認識牠，而牠也認識你。" },
        { "Zombie", "殭屍亡魂",
          "A rotting corpse animated by dark magics.  Flesh falls off in large chunks as it shambles forward.  You feel you know it and it knows you.", "一具被黑暗魔法喚起的腐屍蹣跚前進，大片腐肉不斷剝落。你感覺自己認識牠，而牠也認識你。" },
        { "Spirit", "幽靈亡魂",
          "A ghostly apparition that shrieks horribly.  You feel you know it and it knows you.", "這道幽魂發出可怖的尖叫。你感覺自己認識牠，而牠也認識你。" },
        { "Mummy", "木乃伊亡魂",
          "A larger-than life figure covered in wrappings.  Twin flames burn in its head.  You feel you know it and it knows you.", "這具身形異常高大的木乃伊全身纏滿布條，頭部燃燒著兩團火焰。你感覺自己認識牠，而牠也認識你。" },
        { "Wight", "屍妖亡魂",
          "This eldritch form rises from a grave shrouded in dust.  As it reaches out towards you, your panicked soul cowers away.  You feel you know it and it knows you.", "這個詭異身影從塵封的墳墓升起，朝你伸出手時，你驚恐的靈魂不禁退縮。你感覺自己認識牠，而牠也認識你。" },
        { "Ghost", "鬼魂亡魂",
          "A phantasm, a shadow given life.  The nimbus of corpse-light around it illuminates nothing.  You feel you know it and it knows you.", "一道被賦予生命的陰影幻影，周圍籠罩的屍火毫無照明之效。你感覺自己認識牠，而牠也認識你。" },
        { "Vampire", "吸血鬼亡魂",
          "A humanoid with an aura of power.  You notice a sharp set of front teeth.  You feel you know it and it knows you.", "這名人形生物散發強大氣息，口中露出一對尖銳門牙。你感覺自己認識牠，而牠也認識你。" },
        { "Phantom", "幻影亡魂",
          "An unholy creature of darkness; the aura emanating from this evil being saps your very soul.  You feel you know it and it knows you.", "這個褻瀆神聖的黑暗生物散發邪惡氣息，正在抽乾你的靈魂。你感覺自己認識牠，而牠也認識你。" },
        { "Lich", "巫妖亡魂",
          "A skeletal form dressed in robes, radiating vastly evil power.  You feel you know it and it knows you.", "一具穿著長袍的骷髏，散發著極其邪惡的強大力量。你感覺自己認識牠，而牠也認識你。" },
        { "Nether Wraith", "冥界怨靈亡魂",
          "As it advances, your soul wilts under the threat of death and a dreadful chill invades your bones.  You feel you know it and it knows you.", "牠逐漸逼近，死亡威脅令你的靈魂萎縮，徹骨寒意侵入骨髓。你感覺自己認識牠，而牠也認識你。" },
        { "Deathlord", "死亡領主亡魂",
          "Cloaked in midnight, and supported by minions of death, this lord of the undead comes to claim your soul.  You feel you know it and it knows you.", "這名不死領主身披午夜般的黑袍，率領死亡僕從前來索取你的靈魂。你感覺自己認識牠，而牠也認識你。" },
    };
    size_t i;
    unsigned descriptions = 0;
    CHECK(sizeof(cases) / sizeof(cases[0]) == 77);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const char *name = cases[i].en;
        CHECK(strcmp(angdroid_faangband_monster_name(name), cases[i].zh) == 0);
        CHECK(strcmp(angdroid_zh_tw_monster_list_name(name), cases[i].zh) == 0);
        CHECK(strcmp(angdroid_zh_tw_unique_monster_name(name), cases[i].zh) == 0);
        if (cases[i].source) {
            const char *shown = angdroid_faangband_monster_flavor(
                name, cases[i].source);
            CHECK(shown != NULL);
            CHECK(strcmp(shown, cases[i].description) == 0);
            CHECK(strcmp(angdroid_faangband_monster_flavor(
                name, "Changed upstream description"),
                "Changed upstream description") == 0);
            ++descriptions;
        } else {
            CHECK(strcmp(name, "skeleton lord") == 0);
            CHECK(angdroid_faangband_monster_flavor(name, "") == NULL);
        }
    }
    CHECK(descriptions == 76);
    CHECK(angdroid_faangband_monster_name(NULL) == NULL);
    CHECK(angdroid_faangband_monster_flavor(NULL, "Unknown") == NULL);
    CHECK(angdroid_faangband_monster_flavor("blue dragon", NULL) == NULL);
    CHECK(angdroid_faangband_monster_flavor("Unknown", "Unknown") == NULL);
    CHECK(angdroid_zh_tw_monster_list_name("Unknown") == NULL);
    CHECK(angdroid_zh_tw_unique_monster_name("Unknown") == NULL);
    printf("PASS: 77 FAangband monster names, 76 lore (assertions=%u)\n", checks);
    return 0;
}
