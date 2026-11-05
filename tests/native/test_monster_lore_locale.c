#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"
int main(void)
{
    assert(ANGDROID_ZH_TW_MONSTER_LORE == 199);
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
    assert(angdroid_zh_tw_monster_flavor("Morgoth, Lord of Darkness") == NULL);
    assert(angdroid_zh_tw_monster_flavor(NULL) == NULL);
    assert(strcmp(angdroid_zh_tw_monster_list_name("scrawny cat"), "瘦弱的貓") == 0);
    assert(angdroid_zh_tw_monster_list_name("Farmer Maggot") == NULL);
    puts("PASS: monster lore flavor, titles, English fallback");
    return 0;
}
