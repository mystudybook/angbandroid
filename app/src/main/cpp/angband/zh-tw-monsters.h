/*
 * Non-unique monster names for the display-only monster list.
 * English spellings are sourced from upstream Angband 4.2.4 monster.txt.
 * Unknown names and named unique monsters keep the original text.
 *
 * Never mutate monster_race.name or monster_race.plural: those fields are
 * consumed by game data, lore, messages and save-related code.
 */
#ifndef ANGDROID_ZH_TW_MONSTERS_H
#define ANGDROID_ZH_TW_MONSTERS_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_MONSTER_NAMES 128

static inline const char *angdroid_zh_tw_monster_list_name(const char *english)
{
    static const struct {
        const char *english;
        const char *traditional;
    } names[ANGDROID_ZH_TW_MONSTER_NAMES] = {
        { "filthy street urchin", "髒兮兮的街童" },
        { "scrawny cat", "瘦弱的貓" },
        { "scruffy little dog", "毛髮蓬亂的小狗" },
        { "blubbering idiot", "哭哭啼啼的傻子" },
        { "boil-covered wretch", "滿身膿瘡的可憐人" },
        { "village idiot", "村裡的傻子" },
        { "pitiful-looking beggar", "可憐的乞丐" },
        { "mangy-looking leper", "形容枯槁的痲瘋病人" },
        { "squint-eyed rogue", "斜眼惡棍" },
        { "singing, happy drunk", "唱歌的快樂醉漢" },
        { "aimless-looking merchant", "漫無目的的商人" },
        { "mean-looking mercenary", "面目凶惡的傭兵" },
        { "battle-scarred veteran", "傷痕累累的老兵" },
        { "red-hatted elf", "戴紅帽的精靈" },
        { "grey mold", "灰色黴菌" },
        { "grey mushroom patch", "灰色蘑菇群" },
        { "giant yellow centipede", "巨型黃蜈蚣" },
        { "giant white centipede", "巨型白蜈蚣" },
        { "white icky thing", "白色黏怪" },
        { "clear icky thing", "透明黏怪" },
        { "giant white mouse", "巨型白鼠" },
        { "large white snake", "大型白蛇" },
        { "small kobold", "小型狗頭人" },
        { "white worm mass", "白色蠕蟲群" },
        { "floating eye", "漂浮之眼" },
        { "rock lizard", "岩蜥" },
        { "wild dog", "野狗" },
        { "soldier ant", "兵蟻" },
        { "fruit bat", "果蝠" },
        { "kobold", "狗頭人" },
        { "shrieker mushroom patch", "尖叫蘑菇群" },
        { "blubbering icky thing", "冒泡黏怪" },
        { "metallic green centipede", "金屬綠蜈蚣" },
        { "soldier", "士兵" },
        { "cutpurse", "扒手" },
        { "acolyte", "見習祭司" },
        { "apprentice", "學徒" },
        { "yellow mushroom patch", "黃色蘑菇群" },
        { "white jelly", "白色果凍怪" },
        { "giant green frog", "巨型綠蛙" },
        { "giant black ant", "巨型黑蟻" },
        { "salamander", "火蜥蜴" },
        { "white harpy", "白色鷹身女妖" },
        { "blue yeek", "藍色耶克人" },
        { "green worm mass", "綠色蠕蟲群" },
        { "cave spider", "洞穴蜘蛛" },
        { "wild cat", "野貓" },
        { "crow", "烏鴉" },
        { "green ooze", "綠色軟泥怪" },
        { "poltergeist", "騷靈" },
        { "metallic blue centipede", "金屬藍蜈蚣" },
        { "giant white louse", "巨型白蝨" },
        { "black naga", "黑色那迦" },
        { "spotted mushroom patch", "斑點蘑菇群" },
        { "silver jelly", "銀色果凍怪" },
        { "yellow jelly", "黃色果凍怪" },
        { "scruffy looking hobbit", "衣衫襤褸的哈比人" },
        { "giant white ant", "巨型白蟻" },
        { "yellow mold", "黃色黴菌" },
        { "metallic red centipede", "金屬紅蜈蚣" },
        { "yellow worm mass", "黃色蠕蟲群" },
        { "clear worm mass", "透明蠕蟲群" },
        { "radiation eye", "輻射之眼" },
        { "kobold shaman", "狗頭人薩滿" },
        { "cave lizard", "洞穴蜥蜴" },
        { "scout", "斥候" },
        { "gallant", "英勇戰士" },
        { "tamer", "馴獸師" },
        { "blue jelly", "藍色果凍怪" },
        { "creeping copper coins", "爬行的銅幣" },
        { "giant white rat", "巨型白大鼠" },
        { "blue worm mass", "藍色蠕蟲群" },
        { "large grey snake", "大型灰蛇" },
        { "kobold archer", "狗頭人弓箭手" },
        { "silver mouse", "銀鼠" },
        { "green naga", "綠色那迦" },
        { "blue ooze", "藍色軟泥怪" },
        { "green glutton ghost", "綠色貪食鬼" },
        { "green jelly", "綠色果凍怪" },
        { "large kobold", "大型狗頭人" },
        { "skeleton kobold", "狗頭人骷髏" },
        { "grey icky thing", "灰色黏怪" },
        { "disenchanter eye", "解除附魔之眼" },
        { "red worm mass", "紅色蠕蟲群" },
        { "copperhead snake", "銅頭蝮蛇" },
        { "giant white dragon fly", "巨型白蜻蜓" },
        { "giant green dragon fly", "巨型綠蜻蜓" },
        { "rot jelly", "腐爛果凍怪" },
        { "purple mushroom patch", "紫色蘑菇群" },
        { "brown mold", "棕色黴菌" },
        { "giant brown bat", "巨型棕蝙蝠" },
        { "creeping silver coins", "爬行的銀幣" },
        { "snaga", "斯納嘎半獸人" },
        { "rattlesnake", "響尾蛇" },
        { "giant black dragon fly", "巨型黑蜻蜓" },
        { "giant gold dragon fly", "巨型金蜻蜓" },
        { "cave orc", "洞穴半獸人" },
        { "wood spider", "樹林蜘蛛" },
        { "bloodshot eye", "充血之眼" },
        { "red naga", "紅色那迦" },
        { "red jelly", "紅色果凍怪" },
        { "giant red frog", "巨型紅蛙" },
        { "green icky thing", "綠色黏怪" },
        { "zombified kobold", "殭屍狗頭人" },
        { "lost soul", "迷失的靈魂" },
        { "eastern dwarf", "東方矮人" },
        { "night lizard", "夜蜥" },
        { "terrified yeek", "驚恐的耶克人" },
        { "giant salamander", "巨型火蜥蜴" },
        { "green mold", "綠色黴菌" },
        { "skeleton orc", "半獸人骷髏" },
        { "orc tracker", "半獸人追蹤者" },
        { "ruffian", "惡棍" },
        { "giant black louse", "巨型黑蝨" },
        { "yeti", "雪人" },
        { "giant grey rat", "巨型灰大鼠" },
        { "black harpy", "黑色鷹身女妖" },
        { "orc shaman", "半獸人薩滿" },
        { "baby blue dragon", "幼年藍龍" },
        { "baby white dragon", "幼年白龍" },
        { "baby green dragon", "幼年綠龍" },
        { "baby black dragon", "幼年黑龍" },
        { "baby red dragon", "幼年紅龍" },
        { "giant red ant", "巨型紅蟻" },
        { "king cobra", "眼鏡王蛇" },
        { "baby gold dragon", "幼年金龍" },
        { "cave bear", "洞穴熊" },
        { "giant spider", "巨型蜘蛛" },
    };
    size_t i;

    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_MONSTER_NAMES; ++i) {
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    }
    return NULL;
}

#endif
