/* Knowledge menu labels stay bilingual without changing selection keys. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"
#include "../../app/src/main/cpp/angband/zh-tw-knowledge-menu.h"

static int checks;
static void store(const char *name, int index, const char *shortcut,
                  int chinese, const char *expected)
{
    char out[256];
    const char *label = angdroid_knowledge_store_label(out, sizeof(out),
        name, index, shortcut, chinese);
    assert(label == out);
    assert(strcmp(label, expected) == 0);
    ++checks;
}
static void action(const char *english, const char *chinese)
{
    assert(strcmp(angdroid_game_ui_text(english, 0), english) == 0);
    assert(strcmp(angdroid_game_ui_text(english, 1), chinese) == 0);
    checks += 2;
}
int main(void)
{
    char tiny[8] = "before";
    char empty[1] = "";
    char medium[64];
    const char *long_name = "A shop with an unusually long name that cannot fit";
    action("Display object knowledge", "查看物品知識");
    action("Display rune knowledge", "查看符文知識");
    action("Display artifact knowledge", "查看神器知識");
    action("Display ego item knowledge", "查看特殊詞綴物品");
    action("Display monster knowledge", "查看怪物知識");
    action("Display feature knowledge", "查看地形知識");
    action("Display trap knowledge", "查看陷阱知識");
    action("Display shapechange effects", "查看變形效果");
    action("Display hall of fame", "查看名人堂");
    action("Display character history", "查看角色歷史");
    action("Display equippable comparison", "比較可裝備物品");
    store("General Store", 1, " (1)", 1, "查看雜貨店的商品 (1)");
    store("General Store", 1, " (1)", 0,
          "Display General Store's contents (1)");
    store("Weapon Smiths", 3, " (3)", 1, "查看武器店的商品 (3)");
    store("Weapon Smiths", 3, " (3)", 0,
          "Display Weapon Smiths' contents (3)");
    store("Home", 8, " (8)", 1, "查看住家的商品 (8)");
    store("Home", 8, " (8)", 0, "Display Home's contents (8)");
    store(NULL, 10, "", 1, "查看第 10 間商店的商品");
    store(NULL, 10, "", 0, "Display store 10's contents");
    store("Custom Store", 11, NULL, 1, "查看Custom Store的商品");
    store("Custom Store", 11, NULL, 0,
          "Display Custom Store's contents");
    store("", 2, " (2)", 1, "查看第 2 間商店的商品 (2)");
    store("", 2, " (2)", 0, "Display store 2's contents (2)");
    store("General Store", 1, NULL, 1, "查看雜貨店的商品");
    assert(angdroid_knowledge_store_label(tiny, sizeof(tiny),
        long_name, 1, " (1)", 1) == NULL);
    assert(tiny[0] == '\0');
    ++checks;
    assert(angdroid_knowledge_store_label(empty, sizeof(empty),
        "General Store", 1, "", 1) == NULL);
    assert(empty[0] == '\0');
    ++checks;
    /* Long Chinese custom names use a numbered Chinese fallback, not
     * an English sentence; the same long English name keeps the original
     * NULL / caller fallback behavior. */
    assert(angdroid_knowledge_store_label_for_menu(medium, sizeof(medium),
        long_name, 11, " (9)", 1) == medium);
    assert(strcmp(medium, "查看第 11 間商店的商品 (9)") == 0);
    ++checks;
    assert(angdroid_knowledge_store_label_for_menu(medium, sizeof(medium),
        long_name, 11, " (9)", 0) == NULL);
    assert(medium[0] == '\0');
    ++checks;
    assert(angdroid_knowledge_store_label_for_menu(medium, sizeof(medium),
        "General Store", 1, " (1)", 1) == medium);
    assert(strcmp(medium, "查看雜貨店的商品 (1)") == 0);
    ++checks;
    assert(angdroid_knowledge_store_label(NULL, 0,
        "General Store", 1, "", 1) == NULL);
    ++checks;
    assert(angdroid_knowledge_store_label(tiny, sizeof(tiny),
        "General Store", 0, "", 1) == NULL);
    assert(tiny[0] == '\0');
    ++checks;
    printf("PASS: knowledge menu fixed entries and store labels (%d checks)\n", checks);
    return 0;
}
