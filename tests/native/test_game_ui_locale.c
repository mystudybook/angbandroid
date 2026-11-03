/* This mapping is for presentation only, never game IDs or commands. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"

int main(void)
{
    const char *unknown = "Untranslated data";
    assert(strcmp(angdroid_game_ui_text("New game", 1), "建立新遊戲") == 0);
    assert(strcmp(angdroid_game_ui_text("Point-based", 1), "點數分配") == 0);
    assert(strcmp(angdroid_game_ui_text("['r' to reroll", 1), "[r 重擲") == 0);
    assert(strcmp(angdroid_game_ui_text(", 'p' for previous roll", 1),
                  "，p 上次") == 0);
    assert(strcmp(angdroid_game_ui_text(" or 'Enter' to accept]", 1),
                  "，Enter 確認]") == 0);
    assert(strcmp(angdroid_game_ui_text("Save and quit", 1), "儲存並離開") == 0);
    assert(strcmp(angdroid_game_ui_text("Inscribe an object", 1), "為物品刻字") == 0);
    assert(strcmp(angdroid_game_ui_text("Cast a spell", 1), "施放法術") == 0);
    assert(strcmp(angdroid_game_ui_text("Ignore an item", 1), "忽略物品") == 0);
    assert(strcmp(angdroid_game_ui_text("Walk Towards", 1), "走向此處") == 0);
    assert(strcmp(angdroid_game_ui_text("Use %s: %s", 1), "使用 %s：%s") == 0);
    assert(strcmp(angdroid_game_ui_text("Saving game... failed!", 1),
                  "遊戲儲存失敗！") == 0);
    assert(strcmp(angdroid_game_ui_text("New game", 0), "New game") == 0);
    assert(strcmp(angdroid_game_ui_text(unknown, 1), unknown) == 0);
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    assert(angdroid_utf8_display_columns("繁體中文") == 8);
    puts("PASS: Angband game UI Traditional Chinese labels");
    return 0;
}
