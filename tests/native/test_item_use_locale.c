/* Host C99: Angband item-use picker prompts, exact bilingual fallback.
 * Only display text is translated; the native command filters stay intact.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"

static int checks;

static void mapped(const char *english, const char *traditional)
{
    const char *en = angdroid_game_ui_text(english, 0);
    const char *zh = angdroid_game_ui_text(english, 1);
    assert(en && strcmp(en, english) == 0);
    assert(zh && strcmp(zh, traditional) == 0);
    assert(angdroid_game_ui_text(english, 2) == zh);
    checks += 3;
}

int main(void)
{
    mapped("Read which scroll? ", "要閱讀哪張卷軸？ ");
    mapped("You have no scrolls to read.", "你沒有可以閱讀的卷軸。");
    mapped("Eat which food? ", "要吃哪種食物？ ");
    mapped("You have no food to eat.", "你沒有可以吃的食物。");
    mapped("Quaff which potion? ", "要喝哪瓶藥水？ ");
    mapped("You have no potions from which to quaff.",
        "你沒有可以喝的藥水。");
    mapped("Use which item? ", "要使用哪件物品？ ");
    mapped("You have no items to use.", "你沒有可使用的物品。");
    mapped("Refuel with which fuel source? ",
        "要使用哪種燃料補充光源？ ");

    assert(angdroid_game_ui_text(NULL, 0) == NULL);
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    assert(strcmp(angdroid_game_ui_text("Unknown item prompt", 1),
        "Unknown item prompt") == 0);
    assert(strcmp(angdroid_game_ui_text("Drop which item?", 0),
        "Drop which item?") == 0);
    checks += 4;

    printf("PASS: item-use picker bilingual prompts (%d checks)\n", checks);
    return 0;
}
