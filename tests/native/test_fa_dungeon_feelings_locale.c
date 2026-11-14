/*
 * Compile the real shared UI translation table to verify FAangband
 * treasure and monster-danger feeling strings. English remains exact.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct feeling_pair {
    const char *english;
    const char *zh_tw;
};

static const struct feeling_pair object_feelings[] = {
    {"Looks like any other level.", "這一層看起來與其他樓層沒有太大差別"},
    {"you sense an item of wondrous power!", "你感應到一件蘊含驚人力量的物品"},
    {"there are superb treasures here.", "這裡藏有極為珍貴的寶物"},
    {"there are excellent treasures here.", "這裡藏有上等寶物"},
    {"there are very good treasures here.", "這裡藏有相當不錯的寶物"},
    {"there are good treasures here.", "這裡藏有不錯的寶物"},
    {"there may be something worthwhile here.", "這裡或許有值得尋找的東西"},
    {"there may not be much interesting here.", "這裡可能沒有太多有價值的東西"},
    {"there aren't many treasures here.", "這裡的寶物似乎不多"},
    {"there are only scraps of junk here.", "這裡似乎只有一些破爛雜物"},
    {"there is naught but cobwebs here.", "這裡似乎只剩下蜘蛛網"}
};
static const struct feeling_pair monster_feelings[] = {
    {"You are still uncertain about this place", "你仍無法判斷此地的危險程度"},
    {"Omens of death haunt this place", "死亡的預兆籠罩著此地"},
    {"This place seems murderous", "這裡殺機四伏"},
    {"This place seems terribly dangerous", "這裡似乎極度危險"},
    {"You feel anxious about this place", "你對這裡感到不安"},
    {"You feel nervous about this place", "你對這裡感到緊張"},
    {"This place does not seem too risky", "這裡似乎不算太危險"},
    {"This place seems reasonably safe", "這裡看起來還算安全"},
    {"This seems a tame, sheltered place", "這裡似乎平靜而且隱蔽"},
    {"This seems a quiet, peaceful place", "這裡看起來安靜又祥和"}
};
static unsigned long checks;
/* Canonical themed.txt message values must be translated only at display. */
static const struct feeling_pair themed_messages[] = {
    {"You find yourself in a battlefield of the Elements!", "你發現自己置身於元素交戰的戰場！"},
    {"Beware, for there be dragons here!", "小心，此地有巨龍出沒！"},
    {"Dire wolves guard crumbling ruins in this wild land; find shelter quick!", "兇狼守衛著荒野中的殘破遺跡；快找地方躲藏！"},
    {"The Fires of Udun rage here, and demons laugh to see new sport!", "烏頓的烈焰在此肆虐，惡魔正為新的獵物而狂笑！"},
    {"Goblin-drums sound: Doom... Doom...", "哥布林的戰鼓響起：咚……咚……"},
    {"Proud warlords struggle in this land; here, there are no neutrals.", "驕傲的軍閥在此爭戰；在這片土地上，沒有人能置身事外。"},
    {"Here Barahir and his outlaws were murdered.", "巴拉希爾與他的亡命之徒在此遭到殺害。"},
    {"Here the Edain camped when they first came to Beleriand.", "伊甸人初抵貝烈瑞安德時，曾在此紮營。"},
    {"\"Haudh-en-Ndengin the Elves named it, the Hill of Slain...\"", "「精靈稱它為 Haudh-en-Ndengin，意為『陣亡者之丘』……」"},
};

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

static int check_set(const struct feeling_pair *items, size_t n)
{
    size_t i;
    for (i = 0; i < n; ++i) {
        const char *actual = angdroid_game_ui_text(items[i].english, 1);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, items[i].zh_tw) == 0);
        CHECK(angdroid_game_ui_text(items[i].english, 0) == items[i].english);
    }
    return 0;
}
int main(void)
{
    char rendered[300];
    const char *monster;
    const char *treasure;

    if (check_set(object_feelings,
            sizeof(object_feelings) / sizeof(object_feelings[0])))
        return 1;
    if (check_set(monster_feelings,
            sizeof(monster_feelings) / sizeof(monster_feelings[0])))
        return 1;
    if (check_set(themed_messages,
            sizeof(themed_messages) / sizeof(themed_messages[0])))
        return 1;

    monster = angdroid_game_ui_text("This place seems murderous", 1);
    treasure = angdroid_game_ui_text("there are excellent treasures here.", 1);
    (void)snprintf(rendered, sizeof(rendered), "%s，而且%s。",
        monster, treasure);
    CHECK(strcmp(rendered, "這裡殺機四伏，而且這裡藏有上等寶物。") == 0);

    monster = angdroid_game_ui_text("This place seems reasonably safe", 1);
    treasure = angdroid_game_ui_text("there are superb treasures here.", 1);
    (void)snprintf(rendered, sizeof(rendered), "%s，但%s。",
        monster, treasure);
    CHECK(strcmp(rendered, "這裡看起來還算安全，但這裡藏有極為珍貴的寶物。") == 0);

    (void)snprintf(rendered, sizeof(rendered), "你感覺到：%s。",
        angdroid_game_ui_text("there may be something worthwhile here.", 1));
    CHECK(strcmp(rendered, "你感覺到：這裡或許有值得尋找的東西。") == 0);

    (void)snprintf(rendered, sizeof(rendered), "%s.",
        angdroid_game_ui_text("This place seems murderous", 0));
    CHECK(strcmp(rendered, "This place seems murderous.") == 0);

    CHECK(angdroid_game_ui_text("FA unseen feeling", 1)[0] == 'F');
    printf("PASS: %lu native FA dungeon feeling translation checks\n", checks);
    return 0;
}
