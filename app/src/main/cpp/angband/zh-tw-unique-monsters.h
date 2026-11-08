/* Display-only proper names for all 101 canonical RF_UNIQUE monsters.
 * Source: variant_lib/angband/gamedata/monster.txt (Angband 4.2.4).
 * Do not mutate monster_race.name, save records, or English-language output.
 */
#ifndef ANGDROID_ZH_TW_UNIQUE_MONSTERS_H
#define ANGDROID_ZH_TW_UNIQUE_MONSTERS_H

#include <stddef.h>
#include <string.h>
#include "zh-tw-faangband-monster-flavor.h"

#define ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES 101

static inline const char *angdroid_zh_tw_unique_monster_name(
    const char *english)
{
    static const struct {
        const char *english;
        const char *traditional;
    } names[ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES] = {
        { "Farmer Maggot", "農夫馬戈特" },
        { "Father Christmas", "聖誕老人" },
        { "Grip, Farmer Maggot's Dog", "格里普，農夫馬戈特的狗" },
        { "Fang, Farmer Maggot's Dog", "芳格，農夫馬戈特的狗" },
        { "Sméagol", "史麥戈" },
        { "Bullroarer the Hobbit", "哈比人牛吼" },
        { "Mughash the Kobold Lord", "狗頭人領主穆加什" },
        { "Wormtongue, Agent of Saruman", "巧言，薩魯曼的密探" },
        { "Lagduf, the Snaga", "斯那嘎拉格杜夫" },
        { "Brodda, the Easterling", "東夷人布羅達" },
        { "Orfax, Son of Boldor", "奧法克斯，博爾多之子" },
        { "Grishnákh, the Hill Orc", "山地半獸人格里什納克" },
        { "Golfimbul, the Hill Orc Chief", "山地半獸人首領高爾芬布爾" },
        { "Boldor, King of the Yeeks", "博爾多，伊克族之王" },
        { "Ufthak of Cirith Ungol", "西力斯昂哥的烏夫薩克" },
        { "Ulfast, Son of Ulfang", "烏爾法斯特，烏爾方之子" },
        { "Nár, the Dwarf", "矮人納爾" },
        { "Shagrat, the Orc Captain", "半獸人隊長夏格拉特" },
        { "Gorbag, the Orc Captain", "半獸人隊長戈巴格" },
        { "Old Man Willow", "老柳樹" },
        { "Bolg, Son of Azog", "波爾格，阿索格之子" },
        { "Lugdush, the Uruk", "強獸人盧格杜什" },
        { "Uglúk, the Uruk", "強獸人烏格魯克" },
        { "Azog, Enemy of the Dwarves", "阿索格，矮人之敵" },
        { "Ibun, Son of Mîm", "伊本，密姆之子" },
        { "Khîm, Son of Mîm", "奇米，密姆之子" },
        { "Sangahyando of Umbar", "昂巴的桑加海安多" },
        { "Angamaitë of Umbar", "昂巴的安加麥特" },
        { "Ulwarth, Son of Ulfang", "烏爾瓦斯，烏爾方之子" },
        { "Mîm, Betrayer of Turin", "密姆，圖林的背叛者" },
        { "Uldor the Accursed", "受詛咒的烏爾多" },
        { "Draebor, the Imp", "小惡魔德雷博" },
        { "Beorn, the Shape-Changer", "變形者比翁" },
        { "Beorn, the Mountain Bear", "山熊比翁" },
        { "Bert the Stone Troll", "石食人妖伯特" },
        { "Bill the Stone Troll", "石食人妖比爾" },
        { "Tom the Stone Troll", "石食人妖湯姆" },
        { "Ulfang the Black", "黑烏爾方" },
        { "Lokkak, the Ogre Chieftain", "食人魔酋長洛卡克" },
        { "Lorgan, Chief of the Easterlings", "東夷人首領洛爾甘" },
        { "Castamir the Usurper", "篡位者卡斯塔米爾" },
        { "Gorlim, Betrayer of Barahir", "戈林，巴拉希爾的背叛者" },
        { "Kavlax the Many-Headed", "多頭怪卡夫拉克斯" },
        { "The Queen Ant", "蟻后" },
        { "Rogrog the Black Troll", "黑食人妖羅格羅格" },
        { "Vargo, Tyrant of Fire", "瓦爾戈，烈火暴君" },
        { "Waldern, King of Water", "瓦爾登，水之王" },
        { "Quaker, Master of Earth", "奎克，大地之主" },
        { "Ariel, Queen of Air", "愛麗兒，空氣女王" },
        { "Eöl, the Dark Elf", "黑暗精靈埃歐爾" },
        { "The Balrog of Moria", "摩瑞亞炎魔" },
        { "Harowen the Black Hand", "黑手哈羅溫" },
        { "Itangast the Fire Drake", "火龍伊坦加斯特" },
        { "Scatha the Worm", "巨龍斯卡薩" },
        { "The Phoenix", "鳳凰" },
        { "Shelob, Spider of Darkness", "屍羅，黑暗之蛛" },
        { "Ar-Pharazôn the Golden", "黃金王亞爾法拉松" },
        { "Baphomet the Minotaur Lord", "牛頭人領主巴風特" },
        { "Fundin Bluecloak", "藍斗篷芬丁" },
        { "Gilim, the Giant of Eruman", "吉利姆，埃魯曼巨人" },
        { "Saruman of Many Colours", "多彩薩魯曼" },
        { "Smaug the Golden", "黃金史矛革" },
        { "The Mouth of Sauron", "索倫之口" },
        { "Tevildo, Prince of Cats", "特維爾多，貓之王子" },
        { "Thuringwethil, the Vampire Messenger", "圖林微希爾，吸血鬼信使" },
        { "Tselakus, the Dreadlord", "恐懼領主塞拉庫斯" },
        { "Nan, the Giant", "巨人南" },
        { "Ossë, Herald of Ulmo", "歐希，烏歐牟的使者" },
        { "Omarax, the Eye Tyrant", "奧馬拉克斯，眼魔暴君" },
        { "Ungoliant, the Unlight", "昂哥立安，噬光者" },
        { "Glaurung, Father of the Dragons", "格勞龍，眾龍之父" },
        { "Makar, the Warrior", "戰士馬卡爾" },
        { "Feagwath, the Undead Sorcerer", "費亞格瓦斯，不死術士" },
        { "Uvatha the Horseman", "騎士烏瓦薩" },
        { "Adunaphel the Quiet", "沉默的阿杜娜菲爾" },
        { "Qlzqqlzuup, the Emperor Quylthulg", "奎爾薩格皇帝奎爾茲奎爾祖普" },
        { "Akhorahil the Blind", "盲者阿霍拉希爾" },
        { "Wiruin, the Maelstrom", "維魯因，大漩渦" },
        { "Ren the Unclean", "不潔者仁" },
        { "Maeglin, the Traitor of Gondolin", "梅格林，貢多林的叛徒" },
        { "Ji Indur Dawndeath", "黎明之死吉因杜爾" },
        { "Pazuzu, Lord of Air", "帕祖祖，空氣之王" },
        { "Dwar, Dog Lord of Waw", "德瓦爾，瓦烏犬王" },
        { "Hoarmurath of Dir", "迪爾的霍爾穆拉斯" },
        { "Draugluin, Sire of All Werewolves", "卓古路因，狼人始祖" },
        { "Khamûl, the Black Easterling", "黑東夷人哈姆爾" },
        { "Cantoras, the Skeletal Lord", "骸骨領主坎托拉斯" },
        { "The Witch-King of Angmar", "安格馬巫王" },
        { "Ancalagon the Black", "黑龍安卡拉剛" },
        { "The Tarrasque", "塔拉斯克巨獸" },
        { "Meássë, the Bloody", "血腥梅亞絲" },
        { "Lungorthin, the Balrog of White Fire", "隆戈辛，白焰炎魔" },
        { "Huan, Wolfhound of the Valar", "胡安，維拉獵犬" },
        { "Carcharoth, the Jaws of Thirst", "卡黑洛斯，貪噬之顎" },
        { "Vecna, the Emperor Lich", "巫妖皇帝維克那" },
        { "Gothmog, the High Captain of Balrogs", "勾斯魔格，炎魔大統領" },
        { "Sauron, the Sorcerer", "索倫，黑暗巫師" },
        { "Wolf-Sauron", "狼形索倫" },
        { "Serpent-Sauron", "巨蛇形索倫" },
        { "Vampire-Sauron", "吸血鬼形索倫" },
        { "Morgoth, Lord of Darkness", "魔苟斯，黑暗之王" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES; ++i) {
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    }
    return angdroid_faangband_monster_name(english);
}

#endif
