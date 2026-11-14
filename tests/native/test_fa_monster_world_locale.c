/*
 * FAangband monster movement and environment messages: real translation
 * header and English fallback. All test cases are display-only.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct ui_pair { const char *en; const char *zh; };
static const struct ui_pair messages[] = {
    {"stumbles", "踉蹌了一下"},
    {"bangs into a door", "撞上門"},
    {"bashes into a wall", "撞上牆壁"},
    {"bumps into some rocks", "撞上岩石"},
    {"The rune of protection is broken!", "防護符文被破壞了！"},
    {"Creatures of the darkness flee from the sunlight!", "黑暗中的生物紛紛逃離陽光！"},
    {"The sun has risen.", "太陽升起了。"},
    {"The sun has fallen.", "太陽落下了。"},
    {"The Black Breath sickens you.", "黑息令你感到病痛。"},
    {"The Black Breath saps your strength.", "黑息正奪走你的力量。"},
    {"The Black Breath dims your life force.", "黑息正在侵蝕你的生命力。"},
    {"You faint from the lack of food.", "你因飢餓而昏倒了。"},
    {"You feel yourself yanked homewards!", "你感到一股力量將自己猛然拉回家園！"},
    {"You feel yourself yanked away!", "你感到一股力量將自己猛然帶走！"},
    {"The floor opens beneath you!", "你腳下的地面突然裂開了！"},
    {"You are thrown back in an explosion!", "爆炸將你猛然震退！"},
    {"Non-existent glyph requested. Please report this bug.", "要求建立不存在的符文，請回報此錯誤。"},
    {"No stairs can be created here!", "這裡無法建立樓梯！"},
    {"The decoy is destroyed!", "誘餌被摧毀了！"},
};

static int checks = 0;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    char output[256];
    const char *unknown = "unmapped terrain action";
    for (i = 0; i < sizeof(messages) / sizeof(messages[0]); ++i) {
        CHECK(strcmp(angdroid_game_ui_text(messages[i].en, 1),
            messages[i].zh) == 0);
        CHECK(angdroid_game_ui_text(messages[i].en, 0) == messages[i].en);
    }
    CHECK(angdroid_game_ui_text(unknown, 1) == unknown);
    CHECK(angdroid_game_ui_text(NULL, 1) == NULL);

    (void)snprintf(output, sizeof(output), "%s%s。",
        "食人妖", angdroid_game_ui_text("bangs into a door", 1));
    CHECK(strcmp(output, "食人妖撞上門。") == 0);
    (void)snprintf(output, sizeof(output), "%s %s.", "Troll",
        angdroid_game_ui_text("bangs into a door", 0));
    CHECK(strcmp(output, "Troll bangs into a door.") == 0);
    (void)snprintf(output, sizeof(output), "%s試著撿起%s，但失敗了。",
        "獸人", "短劍");
    CHECK(strcmp(output, "獸人試著撿起短劍，但失敗了。") == 0);
    (void)snprintf(output, sizeof(output), "%s tries to pick up %s, but fails.",
        "Orc", "Sword");
    CHECK(strcmp(output, "Orc tries to pick up Sword, but fails.") == 0);
    (void)snprintf(output, sizeof(output), "你的一件%s已充能完畢。", "魔棒");
    CHECK(strcmp(output, "你的一件魔棒已充能完畢。") == 0);

    printf("PASS: %d FA monster/world locale checks\n", checks);
    return 0;
}
