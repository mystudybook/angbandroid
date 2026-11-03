#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"
int main(void)
{
    assert(ANGDROID_ZH_TW_MONSTER_LORE == 59);
    assert(strcmp(angdroid_zh_tw_monster_flavor("scrawny cat"),
        "這隻瘦巴巴的毛球有著尖銳的爪子和充滿敵意的眼神。") == 0);
    assert(strcmp(angdroid_zh_tw_monster_flavor("floating eye"),
        "一顆沒有身體的眼球，漂浮在離地幾英呎的空中。") == 0);
    assert(angdroid_zh_tw_monster_flavor("Morgoth, Lord of Darkness") == NULL);
    assert(angdroid_zh_tw_monster_flavor(NULL) == NULL);
    assert(strcmp(angdroid_zh_tw_monster_list_name("scrawny cat"), "瘦弱的貓") == 0);
    assert(angdroid_zh_tw_monster_list_name("Farmer Maggot") == NULL);
    puts("PASS: monster lore flavor, titles, English fallback");
    return 0;
}
