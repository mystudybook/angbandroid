/* Display-only shop/home help and item-selection prompts.
 * Keystrokes, item IDs, trading rules and savefiles stay unchanged. */
#ifndef ANGDROID_ZH_TW_STORE_TEXT_H
#define ANGDROID_ZH_TW_STORE_TEXT_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_STORE_STRINGS 18
static inline const char *angdroid_store_ui_text(const char *english, int zh_tw)
{
    static const struct {const char *english; const char *traditional;}
        labels[ANGDROID_ZH_TW_STORE_STRINGS] = {
        { " examines", " 檢視" },
        { " and ", "，" },
        { " (or ", "（或 " },
        { ")", "）" },
        { " picks up", " 取出" },
        { " purchases", " 購買" },
        { " an item. ", "物品。" },
        { " gives an item to the store in return for its identification. Some wands and staves will also be recharged. ", " 交出物品供商店鑑定，部分魔杖與法杖也會充能。" },
        { " drops", " 存放" },
        { " sells", " 出售" },
        { " an item from your inventory. ", "背包物品。" },
        { " inspects an item from your inventory. ", " 檢視背包物品。" },
        { " exits the building.", " 離開商店。" },
        { " exits this screen.", " 關閉畫面。" },
        { "You have nothing that I want. ", "沒有商店願意收購的物品。 " },
        { "Give which item? ", "要交出哪件物品？ " },
        { "Sell which item? ", "要出售哪件物品？ " },
        { "Drop which item? ", "要存放哪件物品？ " },
    };
    size_t i;
    if (!zh_tw || !english) return english;
    for (i = 0; i < ANGDROID_ZH_TW_STORE_STRINGS; ++i) {
        if (strcmp(english, labels[i].english) == 0)
            return labels[i].traditional;
    }
    return english;
}
#endif /* ANGDROID_ZH_TW_STORE_TEXT_H */
