/* Display-only Traditional Chinese group labels for Angband knowledge views.
 * English monster parser data, tval names, sort/group IDs, and savefiles are
 * kept intact. Category names are translated only while being displayed.
 */
#ifndef ANGDROID_ZH_TW_KNOWLEDGE_GROUPS_H
#define ANGDROID_ZH_TW_KNOWLEDGE_GROUPS_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_KNOWLEDGE_GROUPS 97

static inline const char *angdroid_knowledge_group_text(const char *english,
                                                        int zh_tw)
{
    static const struct { const char *en; const char *zh; }
        groups[ANGDROID_ZH_TW_KNOWLEDGE_GROUPS] = {
        { "Uniques", "特殊怪物" },
        { "Ainur", "埃努" },
        { "Ants", "螞蟻" },
        { "Bats", "蝙蝠" },
        { "Birds", "鳥類" },
        { "Canines", "犬科" },
        { "Centipede", "蜈蚣" },
        { "Demons", "惡魔" },
        { "Dragons", "龍類" },
        { "Elementals/Vortices", "元素生物／漩渦" },
        { "Eyes/Beholders", "眼球怪／眼魔" },
        { "Felines", "貓科" },
        { "Ghosts", "幽靈" },
        { "Giants/Ogres", "巨人／食人魔" },
        { "Golems", "魔像" },
        { "Harpies/Hybrids", "鷹身女妖／混種生物" },
        { "Hominids (Elves, Dwarves)", "類人族（精靈、矮人）" },
        { "Hydras", "多頭蛇" },
        { "Icky Things", "黏滑怪物" },
        { "Insects", "昆蟲" },
        { "Jellies", "膠質怪物" },
        { "Killer Beetles", "殺人甲蟲" },
        { "Kobolds", "狗頭人" },
        { "Lichs", "巫妖" },
        { "Men", "人類" },
        { "Mimics", "擬態怪" },
        { "Molds", "黴菌" },
        { "Mushroom Patches", "蕈菇群" },
        { "Nagas", "娜迦" },
        { "Orcs", "半獸人" },
        { "Quadrupeds", "四足獸" },
        { "Quylthulgs", "奎爾蘇格" },
        { "Reptiles/Amphibians", "爬蟲類／兩生類" },
        { "Rodents", "齧齒類" },
        { "Scorpions/Spiders", "蠍子／蜘蛛" },
        { "Skeletons/Drujs", "骷髏／德魯吉" },
        { "Snakes", "蛇類" },
        { "Trees/Ents", "樹木／樹人" },
        { "Trolls", "巨魔" },
        { "Vampires", "吸血鬼" },
        { "Wights/Wraiths", "屍妖／怨靈" },
        { "Worms/Worm Masses", "蠕蟲／蟲群" },
        { "Xorns/Xarens", "索恩／札倫" },
        { "Yeeks", "伊克族" },
        { "Yeti", "雪人" },
        { "Zephyr Hounds", "疾風獵犬" },
        { "Zombies", "殭屍" },
        { "***Unclassified***", "未分類" },
        { "Ring", "戒指" },
        { "Amulet", "護身符" },
        { "Potion", "藥水" },
        { "Scroll", "卷軸" },
        { "Wand", "魔杖" },
        { "Staff", "法杖" },
        { "Rod", "魔棒" },
        { "Food", "食物" },
        { "Mushroom", "蘑菇" },
        { "Priest Book", "祭司法術書" },
        { "Magic Book", "魔法書" },
        { "Nature Book", "自然法術書" },
        { "Shadow Book", "暗影法術書" },
        { "Mystery Book", "神祕法術書" },
        { "Light", "光源" },
        { "Flask", "油瓶" },
        { "Sword", "劍" },
        { "Polearm", "長柄武器" },
        { "Hafted Weapon", "鈍擊武器" },
        { "Bow", "弓" },
        { "Ammunition", "彈藥" },
        { "Shield", "盾牌" },
        { "Crown", "冠冕" },
        { "Helm", "頭盔" },
        { "Gloves", "手套" },
        { "Boots", "靴子" },
        { "Cloak", "斗篷" },
        { "Dragon Scale Mail", "龍鱗甲" },
        { "Hard Armor", "硬甲" },
        { "Soft Armor", "軟甲" },
        { "Digger", "挖掘工具" },
        { "Money", "金錢" },
        { "Combat", "戰鬥" },
        { "Modifiers", "能力修正" },
        { "Resists", "抗性" },
        { "Brands", "元素附魔" },
        { "Slays", "剋制效果" },
        { "Curses", "詛咒" },
        { "Other", "其他" },
        { "Floors", "地板" },
        { "Doors", "門" },
        { "Stairs", "樓梯" },
        { "Walls", "牆壁" },
        { "Streamers", "礦脈" },
        { "Obstructions", "障礙物" },
        { "Stores", "商店" },
        { "Runes", "符文" },
        { "Locks", "鎖" },
        { "Traps", "陷阱" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < ANGDROID_ZH_TW_KNOWLEDGE_GROUPS; ++i) {
        if (strcmp(english, groups[i].en) == 0) return groups[i].zh;
    }
    return english;
}

#endif /* ANGDROID_ZH_TW_KNOWLEDGE_GROUPS_H */
