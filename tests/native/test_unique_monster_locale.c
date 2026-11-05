/* Unique monster names are display-only; never change the canonical keys. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/zh-tw-unique-monsters.h"
#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"

static void expect_unique(const char *english, const char *traditional)
{
    const char *actual = angdroid_zh_tw_unique_monster_name(english);
    assert(actual != NULL);
    assert(strcmp(actual, traditional) == 0);
}

int main(void)
{
    assert(ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES == 101);
    assert(ANGDROID_ZH_TW_MONSTER_NAMES == 522);
    expect_unique("Morgoth, Lord of Darkness", "魔苟斯，黑暗之王");
    expect_unique("Sauron, the Sorcerer", "索倫，黑暗巫師");
    expect_unique("The Witch-King of Angmar", "安格馬巫王");
    expect_unique("Smaug the Golden", "黃金史矛革");
    expect_unique("Shelob, Spider of Darkness", "屍羅，黑暗之蛛");
    expect_unique("Farmer Maggot", "農夫馬戈特");
    expect_unique("Old Man Willow", "老柳樹");

    assert(angdroid_zh_tw_unique_monster_name("kobold") == NULL);
    assert(angdroid_zh_tw_unique_monster_name("Unknown unique") == NULL);
    assert(angdroid_zh_tw_unique_monster_name(NULL) == NULL);
    assert(angdroid_zh_tw_monster_list_name("Morgoth, Lord of Darkness")
           == NULL);
    puts("PASS: all 101 unique monster display-only names and English fallback");
    return 0;
}
