#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"
int main(void)
{
    assert(ANGDROID_ZH_TW_MONSTER_LORE == 500);
    assert(strcmp(angdroid_zh_tw_monster_flavor("silver jelly"),
        "這是一大團銀色的肉塊，會吸收周遭的光線。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("ruffian"),
        "這個粗野無禮的惡棍帶著棍棒、投石索和鞭子。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("Bullroarer the Hobbit"),
        "這位體格結實的哈比人以非凡的力量與活力聞名，是個難纏的對手。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("scrawny cat"),
        "這隻瘦巴巴的毛球有著尖銳的爪子和充滿敵意的眼神。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("floating eye"),
        "一顆沒有身體的眼球，漂浮在離地幾英呎的空中。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("baby blue dragon"),
        "這隻剛孵化的藍龍身體仍很柔軟，眼睛還不習慣光線，鱗片呈淡藍色。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("giant tarantula"),
        "這隻巨型蜘蛛移動迅速，長著毛茸茸的黑紅色腿。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("gelatinous cube"),
        "這團巨大的明膠狀怪物填滿了整條走廊，形成方形身軀；透過透明的身體，你看得見被它吞下的寶物，還有幾具屍體。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("Angamaitë of Umbar"),
        "這名黑努曼諾爾人憎恨西方的人類。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("banshee"),
        "這個女性模樣的幽靈不斷發出哀傷的嚎哭。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("invisible stalker"),
        "你無法看清這種怪物的形體，但牠的殘暴早已名聞遐邇。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("gravity hound"),
        "這些違反重力法則的獵犬竟能在牆壁和天花板上行走！看著牠們周遭扭曲的重力，你突然覺得腳下的地面也不再牢靠。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("ancient red dragon"),
        "這頭巨大的古紅龍鼻孔冒出縷縷濃煙，周身熾熱得讓你難以呼吸。") == 0);
    assert(angdroid_zh_tw_monster_flavor("Morgoth, Lord of Darkness") == NULL);
    assert(angdroid_zh_tw_monster_flavor(NULL) == NULL);
    assert(strcmp(angdroid_zh_tw_monster_list_name("scrawny cat"), "瘦弱的貓") == 0);
    assert(angdroid_zh_tw_monster_list_name("Farmer Maggot") == NULL);
    puts("PASS: monster lore flavor, titles, English fallback");
    return 0;
}
