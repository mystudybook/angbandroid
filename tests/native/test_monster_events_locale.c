#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-monster-events.h"
int main(void)
{
    assert(strcmp(angdroid_monster_event("You vanish into the shadows!", 1), "你消失在陰影之中！") == 0);
    assert(strcmp(angdroid_monster_event("You vanish into the shadows!", 0), "You vanish into the shadows!") == 0);
    assert(strcmp(angdroid_monster_event("treasure", 1), "財寶") == 0);
    assert(strcmp(angdroid_monster_event("treasure", 0), "treasure") == 0);
    assert(strcmp(angdroid_monster_event("Compacting monsters...", 1), "正在整理怪物資料……") == 0);
    assert(strcmp(angdroid_monster_event("Compacting monsters...", 0), "Compacting monsters...") == 0);
    assert(strcmp(angdroid_monster_event("Too many monsters!", 1), "怪物數量過多！") == 0);
    assert(strcmp(angdroid_monster_event("Too many monsters!", 0), "Too many monsters!") == 0);
    assert(strcmp(angdroid_monster_event("Warning! Could not allocate a new monster.", 1), "警告！無法配置新的怪物。") == 0);
    assert(strcmp(angdroid_monster_event("Warning! Could not allocate a new monster.", 0), "Warning! Could not allocate a new monster.") == 0);
    assert(strcmp(angdroid_monster_event("future event", 1), "future event") == 0);
    assert(angdroid_monster_event(NULL, 1) == NULL);
    puts("PASS: 5 monster theft/allocation message pairs and fallbacks");
    return 0;
}
