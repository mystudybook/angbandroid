/* Display-only localized Esc guidance for native item picker. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"

int main(void)
{
    const char *english = angdroid_game_ui_text(" ESC", 0);
    const char *chinese = angdroid_game_ui_text(" ESC", 1);
    assert(english && strcmp(english, " ESC") == 0);
    assert(chinese && strcmp(chinese, " Esc 取消") == 0);
    assert(strcmp(angdroid_game_ui_text("unmapped label", 1),
        "unmapped label") == 0);
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    assert(angdroid_game_ui_text(NULL, 0) == NULL);
    puts("PASS: item picker bilingual Esc guidance (5 checks)");
    return 0;
}
