/* Presentation-only store and home text with English fallback. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-store-text.h"
static void check(const char *en, const char *zh)
{
    assert(strcmp(angdroid_store_ui_text(en, 1), zh) == 0);
    assert(strcmp(angdroid_store_ui_text(en, 0), en) == 0);
}
int main(void)
{
    check(" examines", " 檢視");
    check(" and ", "，");
    check(" (or ", "（或 ");
    check(")", "）");
    check(" picks up", " 取出");
    check(" purchases", " 購買");
    check(" an item. ", "物品。");
    check(" gives an item to the store in return for its identification. Some wands and staves will also be recharged. ", " 交出物品供商店鑑定，部分魔杖與法杖也會充能。");
    check(" drops", " 存放");
    check(" sells", " 出售");
    check(" an item from your inventory. ", "背包物品。");
    check(" inspects an item from your inventory. ", " 檢視背包物品。");
    check(" exits the building.", " 離開商店。");
    check(" exits this screen.", " 關閉畫面。");
    check("You have nothing that I want. ", "沒有商店願意收購的物品。 ");
    check("Give which item? ", "要交出哪件物品？ ");
    check("Sell which item? ", "要出售哪件物品？ ");
    check("Drop which item? ", "要存放哪件物品？ ");
    {
        const char *unknown = "unchanged item";
        assert(angdroid_store_ui_text(unknown, 1) == unknown);
        assert(angdroid_store_ui_text(NULL, 1) == NULL);
    }
    puts("PASS: 18 store strings, English fallback and unknown entries");
    return 0;
}
