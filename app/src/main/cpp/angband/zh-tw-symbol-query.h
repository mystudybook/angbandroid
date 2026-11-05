/* Display-only labels for the original Angband symbol-query command.
 * Canonical tval strings from list-tvals.h and gameplay IDs stay English.
 * Unknown keys and English mode preserve the source label verbatim.
 */
#ifndef ANGDROID_ZH_TW_SYMBOL_QUERY_H
#define ANGDROID_ZH_TW_SYMBOL_QUERY_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT 36

static inline const char *angdroid_symbol_tval_name(const char *canonical,
                                                    int zh_tw)
{
    static const struct { const char *en; const char *zh; }
        names[ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT] = {
        { "none", "無" },
        { "chest", "箱子" },
        { "shot", "投石彈" },
        { "arrow", "箭矢" },
        { "bolt", "弩箭" },
        { "bow", "弓" },
        { "digger", "挖掘工具" },
        { "hafted", "鈍擊武器" },
        { "polearm", "長柄武器" },
        { "sword", "劍" },
        { "boots", "靴子" },
        { "gloves", "手套" },
        { "helm", "頭盔" },
        { "crown", "冠冕" },
        { "shield", "盾牌" },
        { "cloak", "披風" },
        { "soft armor", "軟甲" },
        { "hard armor", "硬甲" },
        { "dragon armor", "龍鱗甲" },
        { "light", "光源" },
        { "amulet", "護身符" },
        { "ring", "戒指" },
        { "staff", "法杖" },
        { "wand", "魔杖" },
        { "rod", "魔棒" },
        { "scroll", "卷軸" },
        { "potion", "藥水" },
        { "flask", "油瓶" },
        { "food", "食物" },
        { "mushroom", "蘑菇" },
        { "magic book", "魔法書" },
        { "prayer book", "祭司法術書" },
        { "nature book", "自然法術書" },
        { "shadow book", "暗影法術書" },
        { "other book", "神祕法術書" },
        { "gold", "金錢" },
    };
    size_t i;
    if (!canonical || !zh_tw) return canonical;
    for (i = 0; i < ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT; ++i) {
        if (strcmp(canonical, names[i].en) == 0) return names[i].zh;
    }
    return canonical;
}

/* Only translated labels receive Chinese punctuation. Unknown source labels
 * retain the exact English fallback, including its final full stop.
 * The symbol itself is never translated or substituted.
 */
static inline void angdroid_symbol_query_format(char *out, size_t capacity,
                                                char sym, const char *canonical,
                                                const char *localized, int zh_tw)
{
    const char *label;
    const char *period;
    if (!out || !capacity) return;
    if (!canonical) canonical = "";
    label = (zh_tw && localized) ? localized : canonical;
    period = (zh_tw && strcmp(label, canonical) != 0) ? "。" : ".";
    (void)snprintf(out, capacity, "%c - %s%s", sym, label, period);
}

/* Match both canonical monster base key and description. If upstream data
 * changes, use the original description rather than a stale translation.
 * The original Morgoth exclusion in lookup_symbol() remains unchanged.
 */
#define ANGDROID_ZH_TW_SYMBOL_MONSTER_BASE_COUNT 56

static inline const char *angdroid_symbol_monster_base_text(
    const char *base_name, const char *canonical_text, int zh_tw)
{
    static const struct { const char *base_name; const char *canonical_text;
        const char *traditional; }
        descriptions[ANGDROID_ZH_TW_SYMBOL_MONSTER_BASE_COUNT] = {
        { "ancient dragon", "Ancient Dragon/Wyrm", "遠古巨龍／龍王" },
        { "ainu", "Ainu/maia", "埃努／邁雅" },
        { "ant", "Ant", "螞蟻" },
        { "bat", "Bat", "蝙蝠" },
        { "bird", "Bird", "鳥類" },
        { "canine", "Canine (Dog)", "犬科（狗）" },
        { "centipede", "Centipede", "蜈蚣" },
        { "creeping coins", "Creeping Coins", "蠕行錢幣" },
        { "dragon", "Dragon", "龍" },
        { "dragon fly", "Dragon Fly", "龍蠅" },
        { "eye", "Floating Eye", "浮空之眼" },
        { "elemental", "Elemental", "元素生物" },
        { "feline", "Feline (Cat)", "貓科（貓）" },
        { "ghost", "Ghost", "幽靈" },
        { "giant", "Giant Humanoid", "巨型類人生物" },
        { "golem", "Golem", "魔像" },
        { "humanoid", "Hobbit/Elf/Dwarf", "哈比人／精靈／矮人" },
        { "hybrid", "Hybrid", "混種生物" },
        { "hydra", "Hydra", "多頭蛇" },
        { "icky thing", "Icky Thing", "黏滑怪物" },
        { "insect", "Insect", "昆蟲" },
        { "jelly", "Jelly", "膠質怪物" },
        { "killer beetle", "Killer Beetle", "殺人甲蟲" },
        { "kobold", "Kobold", "狗頭人" },
        { "lich", "Lich", "巫妖" },
        { "lurker", "Lurker", "潛伏怪" },
        { "major demon", "Major Demon", "高階惡魔" },
        { "mimic", "Mimic", "擬態怪" },
        { "minor demon", "Minor Demon", "低階惡魔" },
        { "mold", "Mold", "黴菌" },
        { "mushroom", "Mushroom", "蘑菇" },
        { "naga", "Naga", "娜迦" },
        { "ogre", "Ogre", "食人魔" },
        { "orc", "Orc", "半獸人" },
        { "person", "Person/Human", "人類" },
        { "quadruped", "Quadruped", "四足獸" },
        { "quylthulg", "Quylthulg", "奎爾蘇格" },
        { "reptile", "Reptile/Amphibian", "爬蟲類／兩生類" },
        { "rodent", "Rodent", "齧齒類" },
        { "skeleton", "Skeleton", "骷髏" },
        { "snake", "Snake", "蛇" },
        { "spider", "Spider/Scorpion/Tick", "蜘蛛／蠍子／壁蝨" },
        { "townsfolk", "Townsfolk", "城鎮居民" },
        { "tree", "Tree/Ent", "樹木／樹人" },
        { "troll", "Troll", "巨魔" },
        { "vampire", "Vampire", "吸血鬼" },
        { "vortex", "Vortex", "漩渦" },
        { "worm", "Worm/Worm Mass", "蠕蟲／蟲群" },
        { "wraith", "Wight/Wraith", "屍妖／怨靈" },
        { "xorn", "Xorn/Xaren", "索恩／札倫" },
        { "yeek", "Yeek", "伊克族" },
        { "yeti", "Yeti", "雪人" },
        { "zephyr hound", "Zephyr Hound", "疾風獵犬" },
        { "zombie", "Zombie/Mummy", "殭屍／木乃伊" },
        { "player", "You", "你" },
        { "Morgoth", "Morgoth", "魔苟斯" },
    };
    size_t i;
    if (!base_name || !canonical_text || !zh_tw) return canonical_text;
    for (i = 0; i < ANGDROID_ZH_TW_SYMBOL_MONSTER_BASE_COUNT; ++i) {
        if (strcmp(base_name, descriptions[i].base_name) == 0 &&
            strcmp(canonical_text, descriptions[i].canonical_text) == 0)
            return descriptions[i].traditional;
    }
    return canonical_text;
}

#endif /* ANGDROID_ZH_TW_SYMBOL_QUERY_H */
