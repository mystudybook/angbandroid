#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"

static void expect_translated(const char *english, const char *chinese)
{
    const char *result = angdroid_zh_tw_monster_list_name(english);
    assert(result != NULL);
    assert(strcmp(result, chinese) == 0);
}

int main(void)
{
    assert(ANGDROID_ZH_TW_MONSTER_NAMES == 258);
    expect_translated("kobold", "狗頭人");
    expect_translated("wolf", "狼");
    expect_translated("vampire bat", "吸血蝙蝠");
    expect_translated("Mirkwood spider", "幽暗密林蜘蛛");
    expect_translated("stone troll", "石食人妖");
    expect_translated("small kobold", "小型狗頭人");
    expect_translated("floating eye", "漂浮之眼");
    expect_translated("giant white mouse", "巨型白鼠");
    expect_translated("fruit bat", "果蝠");
    expect_translated("grey mold", "灰色黴菌");
    expect_translated("wild dog", "野狗");
    expect_translated("yellow mushroom patch", "黃色蘑菇群");
    expect_translated("cave spider", "洞穴蜘蛛");
    expect_translated("green ooze", "綠色軟泥怪");
    expect_translated("large kobold", "大型狗頭人");
    expect_translated("kobold shaman", "狗頭人薩滿");
    expect_translated("orc shaman", "半獸人薩滿");
    expect_translated("baby red dragon", "幼年紅龍");
    expect_translated("cave bear", "洞穴熊");
    expect_translated("giant spider", "巨型蜘蛛");

    /* Fallback is deliberately left to the unchanged original list path. */
    assert(angdroid_zh_tw_monster_list_name("unknown monster") == NULL);
    assert(angdroid_zh_tw_monster_list_name("Kobold") == NULL);
    assert(angdroid_zh_tw_monster_list_name(NULL) == NULL);
    puts("PASS: 258 CJK monster list entries, exact matching and English fallback");
    return 0;
}
