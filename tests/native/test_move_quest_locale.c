#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-move-quest.h"
static int checks;
static void test(const char *en, const char *zh)
{
    assert(strcmp(angdroid_move_quest_text(en, 1), zh) == 0);
    assert(strcmp(angdroid_move_quest_text(en, 0), en) == 0);
    checks += 2;
}
int main(void)
{
    test("stumbles", "踉蹌了一下");
    test("bangs into a door", "撞上了門");
    test("bashes into a wall", "撞上了牆壁");
    test("bumps into some rocks", "撞上了岩石");
    test("tramples over", "踐踏了");
    test("pushes past", "擠過了");
    test("You hear a door burst open!", "你聽見門被撞開的聲音！");
    test("The rune of protection is broken!", "防護符文被破壞了！");
    test("A magical staircase appears...", "一道魔法階梯出現了……");
    test("*** CONGRATULATIONS ***", "＊＊＊ 恭喜你 ＊＊＊");
    test("You have won the game!", "你贏得了這場遊戲！");
    test("You may retire (commit suicide) when you are ready.", "準備好之後，你可以選擇退隱（結束角色生命）以完成冒險。");
    assert(strcmp(angdroid_move_quest_text("future text", 1), "future text") == 0);
    assert(angdroid_move_quest_text(NULL, 1) == NULL);
    checks += 2;
    printf("PASS: %d movement and quest locale checks\n", checks);
    return 0;
}
