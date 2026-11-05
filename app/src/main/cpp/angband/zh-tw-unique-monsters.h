/*
 * Original Angband unique-monster display names, translated to zh-TW.
 * Keys match the 101 RF_UNIQUE records in gamedata/monster.txt exactly.
 * Display-only: never change monster_race.name, savefiles or parser data.
 */
#ifndef ANGDROID_ZH_TW_UNIQUE_MONSTERS_H
#define ANGDROID_ZH_TW_UNIQUE_MONSTERS_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES 101

static inline const char *angdroid_zh_tw_unique_monster_name(
    const char *english)
{
    static const struct {
        const char *english;
        const char *traditional;
    } names[ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES] = {
        { "Farmer Maggot", "瑪格特農夫" },
        { "Father Christmas", "聖誕老人" },
        { "Grip, Farmer Maggot's Dog", "格里普，瑪格特農夫的狗" },
        { "Fang, Farmer Maggot's Dog", "芬格，瑪格特農夫的狗" },
        { "Sméagol", "史麥戈" },
        { "Bullroarer the Hobbit", "哈比人「公牛吼」" },
        { "Mughash the Kobold Lord", "狗頭人領主穆加什" },
        { "Wormtongue, Agent of Saruman", "巧言，薩魯曼的爪牙" },
        { "Lagduf, the Snaga", "斯納嘎拉格杜夫" },
        { "Brodda, the Easterling", "東方人布羅達" },
        { "Orfax, Son of Boldor", "博爾多之子奧法克斯" },
        { "Grishnákh, the Hill Orc", "丘陵半獸人格里什納克" },
        { "Golfimbul, the Hill Orc Chief", "丘陵半獸人首領高爾芬布爾" },
        { "Boldor, King of the Yeeks", "耶克族之王博爾多" },
        { "Ufthak of Cirith Ungol", "奇立斯烏苟的烏夫薩克" },
        { "Ulfast, Son of Ulfang", "烏爾方之子烏爾法斯特" },
        { "Nár, the Dwarf", "矮人納爾" },
        { "Shagrat, the Orc Captain", "半獸人隊長夏格拉特" },
        { "Gorbag, the Orc Captain", "半獸人隊長戈巴格" },
        { "Old Man Willow", "柳樹老翁" },
        { "Bolg, Son of Azog", "阿索格之子波爾格" },
        { "Lugdush, the Uruk", "烏魯克盧格杜什" },
        { "Uglúk, the Uruk", "烏魯克烏格魯克" },
        { "Azog, Enemy of the Dwarves", "矮人之敵阿索格" },
        { "Ibun, Son of Mîm", "米姆之子伊本" },
        { "Khîm, Son of Mîm", "米姆之子奇姆" },
        { "Sangahyando of Umbar", "昂巴的桑加希安多" },
        { "Angamaitë of Umbar", "昂巴的安加麥特" },
        { "Ulwarth, Son of Ulfang", "烏爾方之子烏爾瓦斯" },
        { "Mîm, Betrayer of Turin", "出賣圖林的米姆" },
        { "Uldor the Accursed", "受詛咒的烏爾多" },
        { "Draebor, the Imp", "小惡魔德雷博爾" },
        { "Beorn, the Shape-Changer", "變形者比翁" },
        { "Beorn, the Mountain Bear", "山熊比翁" },
        { "Bert the Stone Troll", "石巨魔伯特" },
        { "Bill the Stone Troll", "石巨魔比爾" },
        { "Tom the Stone Troll", "石巨魔湯姆" },
        { "Ulfang the Black", "黑色烏爾方" },
        { "Lokkak, the Ogre Chieftain", "食人魔首領洛卡克" },
        { "Lorgan, Chief of the Easterlings", "東方人首領洛爾甘" },
        { "Castamir the Usurper", "篡位者卡斯塔米爾" },
        { "Gorlim, Betrayer of Barahir", "出賣巴拉希爾的戈林" },
        { "Kavlax the Many-Headed", "多頭怪卡夫拉克斯" },
        { "The Queen Ant", "蟻后" },
        { "Rogrog the Black Troll", "黑巨魔羅格羅格" },
        { "Vargo, Tyrant of Fire", "烈焰暴君瓦爾戈" },
        { "Waldern, King of Water", "水之王瓦爾登" },
        { "Quaker, Master of Earth", "大地之主奎克" },
        { "Ariel, Queen of Air", "天空女王艾莉爾" },
        { "Eöl, the Dark Elf", "黑暗精靈埃奧爾" },
        { "The Balrog of Moria", "摩瑞亞炎魔" },
        { "Harowen the Black Hand", "黑手哈羅溫" },
        { "Itangast the Fire Drake", "火龍伊坦加斯特" },
        { "Scatha the Worm", "巨龍斯卡薩" },
        { "The Phoenix", "鳳凰" },
        { "Shelob, Spider of Darkness", "黑暗蜘蛛屍羅" },
        { "Ar-Pharazôn the Golden", "黃金王阿爾法拉松" },
        { "Baphomet the Minotaur Lord", "牛頭怪領主巴風特" },
        { "Fundin Bluecloak", "藍斗篷芬丁" },
        { "Gilim, the Giant of Eruman", "埃魯曼巨人吉利姆" },
        { "Saruman of Many Colours", "多彩薩魯曼" },
        { "Smaug the Golden", "黃金巨龍史矛革" },
        { "The Mouth of Sauron", "索倫之口" },
        { "Tevildo, Prince of Cats", "貓王子泰維多" },
        { "Thuringwethil, the Vampire Messenger", "吸血鬼信使瑟林威希爾" },
        { "Tselakus, the Dreadlord", "恐懼領主塞拉庫斯" },
        { "Nan, the Giant", "巨人南恩" },
        { "Ossë, Herald of Ulmo", "烏歐牟的使者歐希" },
        { "Omarax, the Eye Tyrant", "眼魔暴君奧馬拉克斯" },
        { "Ungoliant, the Unlight", "無光者烏苟立安特" },
        { "Glaurung, Father of the Dragons", "群龍之父格勞龍" },
        { "Makar, the Warrior", "戰士馬卡爾" },
        { "Feagwath, the Undead Sorcerer", "不死巫師費亞格瓦斯" },
        { "Uvatha the Horseman", "騎士烏瓦薩" },
        { "Adunaphel the Quiet", "沉默的阿杜納菲爾" },
        { "Qlzqqlzuup, the Emperor Quylthulg", "奎爾薩格怪皇帝庫爾祖普" },
        { "Akhorahil the Blind", "盲者阿霍拉希爾" },
        { "Wiruin, the Maelstrom", "大漩渦威魯因" },
        { "Ren the Unclean", "不潔者任恩" },
        { "Maeglin, the Traitor of Gondolin", "剛多林叛徒梅格林" },
        { "Ji Indur Dawndeath", "黎明死神吉因杜爾" },
        { "Pazuzu, Lord of Air", "風之領主帕祖祖" },
        { "Dwar, Dog Lord of Waw", "瓦烏獵犬之王德瓦爾" },
        { "Hoarmurath of Dir", "迪爾的霍爾穆拉斯" },
        { "Draugluin, Sire of All Werewolves", "狼人始祖卓格路因" },
        { "Khamûl, the Black Easterling", "黑色東方人卡穆爾" },
        { "Cantoras, the Skeletal Lord", "骸骨領主坎托拉斯" },
        { "The Witch-King of Angmar", "安格瑪巫王" },
        { "Ancalagon the Black", "黑龍安卡拉剛" },
        { "The Tarrasque", "泰拉斯奎巨獸" },
        { "Meássë, the Bloody", "嗜血的梅亞瑟" },
        { "Lungorthin, the Balrog of White Fire", "白焰炎魔隆戈辛" },
        { "Huan, Wolfhound of the Valar", "維拉神犬胡安" },
        { "Carcharoth, the Jaws of Thirst", "貪渴巨顎卡黑洛斯" },
        { "Vecna, the Emperor Lich", "巫妖皇帝維克那" },
        { "Gothmog, the High Captain of Balrogs", "炎魔大統帥勾斯魔格" },
        { "Sauron, the Sorcerer", "巫師索倫" },
        { "Wolf-Sauron", "狼形索倫" },
        { "Serpent-Sauron", "巨蛇形索倫" },
        { "Vampire-Sauron", "吸血鬼形索倫" },
        { "Morgoth, Lord of Darkness", "黑暗魔君魔苟斯" },
    };
    size_t i;

    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES; ++i) {
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    }
    return NULL;
}

#endif /* ANGDROID_ZH_TW_UNIQUE_MONSTERS_H */
