/* Display-only FAangband monster additions and divergent English descriptions.
 * Match the exact monster name AND source text before translating the lore.
 * NULL means defer to the original Angband locale table; a non-NULL English
 * fallback means the known name has changed upstream and must not use stale lore.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_MONSTER_FLAVOR_H
#define ANGDROID_ZH_TW_FAANGBAND_MONSTER_FLAVOR_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_faangband_monster_name(const char *english)
{
    static const struct { const char *en, *zh; } names[] = {
        { "The Complainer", "抱怨者" },
        { "maddened weasel", "發狂的鼬鼠" },
        { "Pongo the Devious", "狡詐的龐戈" },
        { "The Thief of the Mountains", "山中大盜" },
        { "crow of Udûn", "烏頓之鴉" },
        { "Mughâsh the Kobold Lord", "狗頭人領主穆加什" },
        { "The Ickiness", "黏穢怪" },
        { "Ulfast, son of Ulfang", "烏爾方之子烏爾法斯特" },
        { "Balcmeg, the cave orc", "洞穴半獸人巴爾克梅格" },
        { "petty-dwarven rogue", "小矮人惡棍" },
        { "Lug, the hill orc", "山地半獸人盧格" },
        { "beastmaster", "馴獸大師" },
        { "skeleton serpent", "骷髏巨蛇" },
        { "floating skull", "漂浮骷髏頭" },
        { "apparition of hunger", "飢餓幽影" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < sizeof names / sizeof names[0]; ++i)
        if (strcmp(english, names[i].en) == 0) return names[i].zh;
    return NULL;
}
static inline const char *angdroid_faangband_monster_flavor(
    const char *name, const char *canonical_text)
{
    static const struct {const char *name, *source, *zh;} flavors[] = {
        { "The Complainer", "He has lots of problems, and seems to think you can fix them. ", "他有數不完的煩惱，而且似乎覺得你能替他一一解決。" },
        { "maddened weasel", "An utterly silent whirlwind of fangs and claws aiming straight for your jugular.", "牠無聲無息地化成利齒與利爪的旋風，直撲你的頸部。" },
        { "Pongo the Devious", "A wicked, power-hungry hobbit outlawed from the Gladden Fields settlement.", "這名邪惡又渴求權力的哈比人，已被逐出格拉頓平原的聚落。" },
        { "The Thief of the Mountains", "He is a sturdy hobbit who is renowned for his unusual strength and vigour.  He can prove a troublesome opponent.", "這名結實的哈比人以過人的力氣與精力聞名，是個難纏的對手。" },
        { "crow of Udûn", "A crook-beaked bird with bedraggled black feathers.  It mocks you, and you realise that its scorn can kill.", "牠有彎曲的喙與蓬亂的黑羽，正嘲弄著你；你意識到牠的輕蔑足以致命。" },
        { "Mughâsh the Kobold Lord", "Strong and powerful, for a kobold.", "以狗頭人而言，牠格外強壯有力。" },
        { "The Ickiness", "You prepare to avoid this foul mess on the ground, then you realise it's alive...", "你正準備繞過地上這團噁心的東西，才驚覺它竟然是活的……" },
        { "Ulfast, son of Ulfang", "He's been spying for his brother Uldor, and doesn't mind making a profit for himself.", "他替兄弟烏爾多刺探情報，也不介意趁機為自己撈一筆。" },
        { "Balcmeg, the cave orc", "A captain of a regiment of weaker orcs, Balcmeg keeps his troop in order with displays of excessive violence.", "巴爾克梅格率領一支較弱小的半獸人部隊，靠著過度殘暴的手段維持紀律。" },
        { "petty-dwarven rogue", "His eyes light up around gold, and he uses all his sorcerous tricks to get it.", "他見到黃金便兩眼發亮，還會施展各種巫術來奪取它。" },
        { "Lug, the hill orc", "He is a cunning and devious orc with a chaotic nature.", "這名半獸人狡詐多端，性情反覆無常。" },
        { "beastmaster", "A short hairy man with a bullwhip, who is always surrounded by animals.", "這個矮小多毛的男人手持牛皮鞭，身邊總有動物相伴。" },
        { "skeleton serpent", "The skeleton of a great snake, driven back to life by necromantic magics.  Its bite is cold and deadly.", "一條巨蛇的骸骨被死靈法術喚回人世，牠的咬擊冰冷而致命。" },
        { "floating skull", "A glowing human skull inhabited by evil spirits.", "一顆發光的人類骷髏頭，裡頭寄宿著邪惡靈體。" },
        { "apparition of hunger", "A deathly thin spirit, eternally ravenous and forever insatiable.", "一個瘦骨嶙峋的亡靈，永遠飢餓，永遠無法滿足。" },
        { "half-orc", "He is a hideous deformed cross-breed with man and orc, combining man's strength and cunning with orcish evil.", "這個醜陋畸形的人獸混血兒結合了人類的力量與狡詐，以及半獸人的邪惡。" },
        { "wolf chieftain", "She commands a pack of wargs in the service of her dark master, and her eyes gleam redly with dreadful knowledge.", "她替黑暗主人統領一群座狼，雙眼閃耀著充滿可怖知識的紅光。" },
        { "greater basilisk", "A large basilisk, whose shape resembles that of a dragon.", "這隻巨型蛇怪的身形與巨龍十分相似。" },
        { "winged horror", "A terrifying sight: a winged creature greater than any bird you have ever seen, and with no feathers on its horrid black leathery wings.", "這隻長翅膀的恐怖怪物比你見過的任何鳥類都巨大，牠可怕的黑色皮翼上沒有一根羽毛。" },
        { "Itangast the Fire Drake", "A mighty dragon, Itangast's form scorches your flesh.  Wisps of smoke curl up from his nostrils as he regards you with disdain.", "伊坦加斯特是一頭強大的巨龍，身上散發的熱力灼傷你的皮膚。牠鼻孔裡飄出縷縷輕煙，輕蔑地打量著你。" },
        { "Ungoliant, the Unlight", "This enormous, hideous spirit of void is in the form of a spider of immense proportions.  She is surrounded by a cloud of Unlight as she sucks in all living light into her bloated body, and breathes out the blackest of darkness. She is always ravenously hungry and would even eat herself to avoid starvation.", "昂哥立安是巨大而醜惡的虛無之靈，化成蜘蛛形體，周圍籠罩著不明之光。牠吞噬一切活生生的光明，再吐出最深沉的黑暗；永無止境的飢餓甚至會讓牠吞食自己。" },
        { "Sauron, the Sorcerer", "Greatest and most terrible of the servants of Morgoth.  A sorcerer of dreadful powers, master of shadows and of phantoms, foul in wisdom, cruel in strength, misshaping what he rules, lord of werewolves; his dominion is torment.  He has many servants, and rarely fights without them.", "索倫是魔苟斯最強大也最可怕的僕從。他精通黑暗與幻影，智慧邪惡、力量殘酷，擅長扭曲所統治的一切，也是魔狼之主；他擁有許多僕從，極少獨自作戰。" },
        { "Smaug the Golden", "Smaug is one of the Urulóki, a flying fire-drake of immense cunning and intelligence.  His speed through air is matched by few other dragons and his dragonfire is what legends are made of.", "史矛革是烏魯洛奇火龍之一，能夠飛行，極其狡詐聰明。牠在空中的速度鮮有其他巨龍能及，傳說中的龍焰更是威力驚人。" },
    };
    static const struct {const char *name, *source;} standard[] = {
        { "half-orc", "He is a hideous deformed cross-breed with man and orc, combining man's strength and cunning with orcish evil.  The traitorous wizard Saruman is generally believed to be responsible for this abomination." },
        { "wolf chieftain", "A great wolf-chieftain whose pack is in the service of the Dark Lord, and whose howls strike fear into even the boldest heart." },
        { "greater basilisk", "A large basilisk, whose shape resembles that of a great wyrm." },
        { "winged horror", "A terrifying sight: a winged creature greater than any bird you have ever seen, and with no feathers on its horrid black leathery wings.  Descended from a creature of an older world perhaps, bred by Sauron to be a winged steed for his Ringwraiths." },
        { "Itangast the Fire Drake", "A mighty ancient dragon, Itangast's form scorches your flesh.  Wisps of smoke curl up from his nostrils as he regards you with disdain." },
        { "Ungoliant, the Unlight", "This enormous, hideous spirit of void is in the form of a spider of immense proportions.  She is surrounded by a cloud of Unlight as she sucks in all living light into her bloated body, and breathes out the blackest of darkness.  She is always ravenously hungry and would even eat herself to avoid starvation." },
        { "Sauron, the Sorcerer", "Greatest and most terrible of the servants of Morgoth, creator of the One Ring.  A sorcerer of dreadful powers, master of shadows and of phantoms, foul in wisdom, cruel in strength, misshaping what he rules, lord of werewolves; his dominion is torment.  He has many servants, and rarely fights without them." },
        { "Smaug the Golden", "Smaug is one of the Urulóki that still survive, a fire-drake of immense cunning and intelligence.  His speed through air is matched by few other dragons and his dragonfire is what legends are made of; he is believed to be the greatest dragon still surviving into the Third Age." },
    };
    size_t i;
    if (!name || !canonical_text) return NULL;
    for (i = 0; i < sizeof flavors / sizeof flavors[0]; ++i) {
        if (strcmp(name, flavors[i].name) == 0) {
            if (strcmp(canonical_text, flavors[i].source) == 0)
                return flavors[i].zh;
            /* Keep the standard version's original translation. */
            for (size_t j = 0; j < sizeof standard / sizeof standard[0]; ++j)
                if (strcmp(name, standard[j].name) == 0 &&
                    strcmp(canonical_text, standard[j].source) == 0)
                    return NULL;
            /* Prevent applying Angband's stale text to a changed FA monster. */
            return canonical_text;
        }
    }
    return NULL;
}
#endif
