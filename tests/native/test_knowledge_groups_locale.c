/* All 97 Angband knowledge-menu group names: zh-TW/English fallback.
 * Layout regressions exercise terminal CELL widths, not UTF-8 byte counts.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-knowledge-groups.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"

int main(void)
{
    assert(strcmp(angdroid_knowledge_group_text("Uniques", 1),
                  "特殊怪物") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Uniques", 0),
                  "Uniques") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ainur", 1),
                  "埃努") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ainur", 0),
                  "Ainur") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ants", 1),
                  "螞蟻") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ants", 0),
                  "Ants") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Bats", 1),
                  "蝙蝠") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Bats", 0),
                  "Bats") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Birds", 1),
                  "鳥類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Birds", 0),
                  "Birds") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Canines", 1),
                  "犬科") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Canines", 0),
                  "Canines") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Centipede", 1),
                  "蜈蚣") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Centipede", 0),
                  "Centipede") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Demons", 1),
                  "惡魔") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Demons", 0),
                  "Demons") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Dragons", 1),
                  "龍類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Dragons", 0),
                  "Dragons") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Elementals/Vortices", 1),
                  "元素生物／漩渦") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Elementals/Vortices", 0),
                  "Elementals/Vortices") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Eyes/Beholders", 1),
                  "眼球怪／眼魔") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Eyes/Beholders", 0),
                  "Eyes/Beholders") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Felines", 1),
                  "貓科") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Felines", 0),
                  "Felines") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ghosts", 1),
                  "幽靈") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ghosts", 0),
                  "Ghosts") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Giants/Ogres", 1),
                  "巨人／食人魔") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Giants/Ogres", 0),
                  "Giants/Ogres") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Golems", 1),
                  "魔像") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Golems", 0),
                  "Golems") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Harpies/Hybrids", 1),
                  "鷹身女妖／混種生物") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Harpies/Hybrids", 0),
                  "Harpies/Hybrids") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hominids (Elves, Dwarves)", 1),
                  "類人族（精靈、矮人）") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hominids (Elves, Dwarves)", 0),
                  "Hominids (Elves, Dwarves)") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hydras", 1),
                  "多頭蛇") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hydras", 0),
                  "Hydras") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Icky Things", 1),
                  "黏滑怪物") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Icky Things", 0),
                  "Icky Things") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Insects", 1),
                  "昆蟲") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Insects", 0),
                  "Insects") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Jellies", 1),
                  "膠質怪物") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Jellies", 0),
                  "Jellies") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Killer Beetles", 1),
                  "殺人甲蟲") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Killer Beetles", 0),
                  "Killer Beetles") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Kobolds", 1),
                  "狗頭人") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Kobolds", 0),
                  "Kobolds") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Lichs", 1),
                  "巫妖") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Lichs", 0),
                  "Lichs") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Men", 1),
                  "人類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Men", 0),
                  "Men") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mimics", 1),
                  "擬態怪") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mimics", 0),
                  "Mimics") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Molds", 1),
                  "黴菌") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Molds", 0),
                  "Molds") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mushroom Patches", 1),
                  "蕈菇群") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mushroom Patches", 0),
                  "Mushroom Patches") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Nagas", 1),
                  "娜迦") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Nagas", 0),
                  "Nagas") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Orcs", 1),
                  "半獸人") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Orcs", 0),
                  "Orcs") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Quadrupeds", 1),
                  "四足獸") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Quadrupeds", 0),
                  "Quadrupeds") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Quylthulgs", 1),
                  "奎爾蘇格") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Quylthulgs", 0),
                  "Quylthulgs") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Reptiles/Amphibians", 1),
                  "爬蟲類／兩生類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Reptiles/Amphibians", 0),
                  "Reptiles/Amphibians") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Rodents", 1),
                  "齧齒類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Rodents", 0),
                  "Rodents") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Scorpions/Spiders", 1),
                  "蠍子／蜘蛛") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Scorpions/Spiders", 0),
                  "Scorpions/Spiders") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Skeletons/Drujs", 1),
                  "骷髏／德魯吉") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Skeletons/Drujs", 0),
                  "Skeletons/Drujs") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Snakes", 1),
                  "蛇類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Snakes", 0),
                  "Snakes") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Trees/Ents", 1),
                  "樹木／樹人") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Trees/Ents", 0),
                  "Trees/Ents") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Trolls", 1),
                  "巨魔") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Trolls", 0),
                  "Trolls") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Vampires", 1),
                  "吸血鬼") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Vampires", 0),
                  "Vampires") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Wights/Wraiths", 1),
                  "屍妖／怨靈") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Wights/Wraiths", 0),
                  "Wights/Wraiths") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Worms/Worm Masses", 1),
                  "蠕蟲／蟲群") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Worms/Worm Masses", 0),
                  "Worms/Worm Masses") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Xorns/Xarens", 1),
                  "索恩／札倫") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Xorns/Xarens", 0),
                  "Xorns/Xarens") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Yeeks", 1),
                  "伊克族") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Yeeks", 0),
                  "Yeeks") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Yeti", 1),
                  "雪人") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Yeti", 0),
                  "Yeti") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Zephyr Hounds", 1),
                  "疾風獵犬") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Zephyr Hounds", 0),
                  "Zephyr Hounds") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Zombies", 1),
                  "殭屍") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Zombies", 0),
                  "Zombies") == 0);
    assert(strcmp(angdroid_knowledge_group_text("***Unclassified***", 1),
                  "未分類") == 0);
    assert(strcmp(angdroid_knowledge_group_text("***Unclassified***", 0),
                  "***Unclassified***") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ring", 1),
                  "戒指") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ring", 0),
                  "Ring") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Amulet", 1),
                  "護身符") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Amulet", 0),
                  "Amulet") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Potion", 1),
                  "藥水") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Potion", 0),
                  "Potion") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Scroll", 1),
                  "卷軸") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Scroll", 0),
                  "Scroll") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Wand", 1),
                  "魔杖") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Wand", 0),
                  "Wand") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Staff", 1),
                  "法杖") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Staff", 0),
                  "Staff") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Rod", 1),
                  "魔棒") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Rod", 0),
                  "Rod") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Food", 1),
                  "食物") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Food", 0),
                  "Food") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mushroom", 1),
                  "蘑菇") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mushroom", 0),
                  "Mushroom") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Priest Book", 1),
                  "祭司法術書") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Priest Book", 0),
                  "Priest Book") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Magic Book", 1),
                  "魔法書") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Magic Book", 0),
                  "Magic Book") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Nature Book", 1),
                  "自然法術書") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Nature Book", 0),
                  "Nature Book") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Shadow Book", 1),
                  "暗影法術書") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Shadow Book", 0),
                  "Shadow Book") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mystery Book", 1),
                  "神祕法術書") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Mystery Book", 0),
                  "Mystery Book") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Light", 1),
                  "光源") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Light", 0),
                  "Light") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Flask", 1),
                  "油瓶") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Flask", 0),
                  "Flask") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Sword", 1),
                  "劍") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Sword", 0),
                  "Sword") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Polearm", 1),
                  "長柄武器") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Polearm", 0),
                  "Polearm") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hafted Weapon", 1),
                  "鈍擊武器") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hafted Weapon", 0),
                  "Hafted Weapon") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Bow", 1),
                  "弓") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Bow", 0),
                  "Bow") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ammunition", 1),
                  "彈藥") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Ammunition", 0),
                  "Ammunition") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Shield", 1),
                  "盾牌") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Shield", 0),
                  "Shield") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Crown", 1),
                  "冠冕") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Crown", 0),
                  "Crown") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Helm", 1),
                  "頭盔") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Helm", 0),
                  "Helm") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Gloves", 1),
                  "手套") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Gloves", 0),
                  "Gloves") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Boots", 1),
                  "靴子") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Boots", 0),
                  "Boots") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Cloak", 1),
                  "斗篷") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Cloak", 0),
                  "Cloak") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Dragon Scale Mail", 1),
                  "龍鱗甲") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Dragon Scale Mail", 0),
                  "Dragon Scale Mail") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hard Armor", 1),
                  "硬甲") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Hard Armor", 0),
                  "Hard Armor") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Soft Armor", 1),
                  "軟甲") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Soft Armor", 0),
                  "Soft Armor") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Digger", 1),
                  "挖掘工具") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Digger", 0),
                  "Digger") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Money", 1),
                  "金錢") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Money", 0),
                  "Money") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Combat", 1),
                  "戰鬥") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Combat", 0),
                  "Combat") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Modifiers", 1),
                  "能力修正") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Modifiers", 0),
                  "Modifiers") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Resists", 1),
                  "抗性") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Resists", 0),
                  "Resists") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Brands", 1),
                  "元素附魔") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Brands", 0),
                  "Brands") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Slays", 1),
                  "剋制效果") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Slays", 0),
                  "Slays") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Curses", 1),
                  "詛咒") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Curses", 0),
                  "Curses") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Other", 1),
                  "其他") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Other", 0),
                  "Other") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Floors", 1),
                  "地板") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Floors", 0),
                  "Floors") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Doors", 1),
                  "門") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Doors", 0),
                  "Doors") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Stairs", 1),
                  "樓梯") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Stairs", 0),
                  "Stairs") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Walls", 1),
                  "牆壁") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Walls", 0),
                  "Walls") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Streamers", 1),
                  "礦脈") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Streamers", 0),
                  "Streamers") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Obstructions", 1),
                  "障礙物") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Obstructions", 0),
                  "Obstructions") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Stores", 1),
                  "商店") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Stores", 0),
                  "Stores") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Runes", 1),
                  "符文") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Runes", 0),
                  "Runes") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Locks", 1),
                  "鎖") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Locks", 0),
                  "Locks") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Traps", 1),
                  "陷阱") == 0);
    assert(strcmp(angdroid_knowledge_group_text("Traps", 0),
                  "Traps") == 0);
    assert(angdroid_knowledge_group_text(NULL, 1) == NULL);
    assert(angdroid_knowledge_group_text(NULL, 0) == NULL);
    {
        const char *unknown = "A previously unseen category";
        assert(angdroid_knowledge_group_text(unknown, 1) == unknown);
        assert(angdroid_knowledge_group_text(unknown, 0) == unknown);
    }
    assert(angdroid_utf8_display_columns("龍類") == 4);
    assert(angdroid_utf8_display_columns("類人族（精靈、矮人）") == 22);
    assert(angdroid_utf8_display_columns("Dragons") == 7);
    puts("PASS: 97 zh-TW knowledge group labels, English fallback, CJK columns");
    return 0;
}
