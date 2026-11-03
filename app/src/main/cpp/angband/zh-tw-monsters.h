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

#define ANGDROID_ZH_TW_MONSTER_NAMES 38

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
