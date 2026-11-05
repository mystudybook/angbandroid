/* Original 56 monster-base descriptions and bilingual C99 fallback. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-symbol-query.h"

static void check(const char *base, const char *desc, const char *traditional)
{
    const char *value = angdroid_symbol_monster_base_text(base, desc, 1);
    char buf[128];
    assert(value != NULL && strcmp(value, traditional) == 0);
    assert(strcmp(angdroid_symbol_monster_base_text(base, desc, 0), desc) == 0);
    angdroid_symbol_query_format(buf, sizeof(buf), 'D', desc, value, 1);
    assert(strstr(buf, traditional) != NULL);
    assert(strcmp(buf + strlen(buf) - strlen("。"), "。") == 0);
}

int main(void)
{
    char out[128];
    const char *changed_desc = "Unknown new source description";
    assert(ANGDROID_ZH_TW_SYMBOL_MONSTER_BASE_COUNT == 56);
    check("ancient dragon", "Ancient Dragon/Wyrm", "遠古巨龍／龍王");
    check("ainu", "Ainu/maia", "埃努／邁雅");
    check("ant", "Ant", "螞蟻");
    check("bat", "Bat", "蝙蝠");
    check("bird", "Bird", "鳥類");
    check("canine", "Canine (Dog)", "犬科（狗）");
    check("centipede", "Centipede", "蜈蚣");
    check("creeping coins", "Creeping Coins", "蠕行錢幣");
    check("dragon", "Dragon", "龍");
    check("dragon fly", "Dragon Fly", "龍蠅");
    check("eye", "Floating Eye", "浮空之眼");
    check("elemental", "Elemental", "元素生物");
    check("feline", "Feline (Cat)", "貓科（貓）");
    check("ghost", "Ghost", "幽靈");
    check("giant", "Giant Humanoid", "巨型類人生物");
    check("golem", "Golem", "魔像");
    check("humanoid", "Hobbit/Elf/Dwarf", "哈比人／精靈／矮人");
    check("hybrid", "Hybrid", "混種生物");
    check("hydra", "Hydra", "多頭蛇");
    check("icky thing", "Icky Thing", "黏滑怪物");
    check("insect", "Insect", "昆蟲");
    check("jelly", "Jelly", "膠質怪物");
    check("killer beetle", "Killer Beetle", "殺人甲蟲");
    check("kobold", "Kobold", "狗頭人");
    check("lich", "Lich", "巫妖");
    check("lurker", "Lurker", "潛伏怪");
    check("major demon", "Major Demon", "高階惡魔");
    check("mimic", "Mimic", "擬態怪");
    check("minor demon", "Minor Demon", "低階惡魔");
    check("mold", "Mold", "黴菌");
    check("mushroom", "Mushroom", "蘑菇");
    check("naga", "Naga", "娜迦");
    check("ogre", "Ogre", "食人魔");
    check("orc", "Orc", "半獸人");
    check("person", "Person/Human", "人類");
    check("quadruped", "Quadruped", "四足獸");
    check("quylthulg", "Quylthulg", "奎爾蘇格");
    check("reptile", "Reptile/Amphibian", "爬蟲類／兩生類");
    check("rodent", "Rodent", "齧齒類");
    check("skeleton", "Skeleton", "骷髏");
    check("snake", "Snake", "蛇");
    check("spider", "Spider/Scorpion/Tick", "蜘蛛／蠍子／壁蝨");
    check("townsfolk", "Townsfolk", "城鎮居民");
    check("tree", "Tree/Ent", "樹木／樹人");
    check("troll", "Troll", "巨魔");
    check("vampire", "Vampire", "吸血鬼");
    check("vortex", "Vortex", "漩渦");
    check("worm", "Worm/Worm Mass", "蠕蟲／蟲群");
    check("wraith", "Wight/Wraith", "屍妖／怨靈");
    check("xorn", "Xorn/Xaren", "索恩／札倫");
    check("yeek", "Yeek", "伊克族");
    check("yeti", "Yeti", "雪人");
    check("zephyr hound", "Zephyr Hound", "疾風獵犬");
    check("zombie", "Zombie/Mummy", "殭屍／木乃伊");
    check("player", "You", "你");
    check("Morgoth", "Morgoth", "魔苟斯");
    assert(strcmp(angdroid_symbol_monster_base_text("dragon", changed_desc, 1),
                  changed_desc) == 0);
    assert(strcmp(angdroid_symbol_monster_base_text("missing", "Dragon", 1),
                  "Dragon") == 0);
    assert(strcmp(angdroid_symbol_monster_base_text(NULL, "Dragon", 1),
                  "Dragon") == 0);
    assert(angdroid_symbol_monster_base_text("dragon", NULL, 1) == NULL);
    angdroid_symbol_query_format(out, sizeof(out), 'D', changed_desc,
        angdroid_symbol_monster_base_text("dragon", changed_desc, 1), 1);
    assert(strcmp(out, "D - Unknown new source description.") == 0);
    puts("PASS: all 56 source exact monster bases and bilingual fallback");
    return 0;
}
