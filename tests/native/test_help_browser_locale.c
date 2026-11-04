/* Native help browser labels are display-only; controls stay English keys. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-help-browser.h"
int main(void)
{
    assert(strcmp(angdroid_help_browser_text("[Press a Letter, or ESC to exit.]", 1), "按字母鍵選擇，或按 Esc 離開。") == 0);
    assert(strcmp(angdroid_help_browser_text("[Press a Letter, or ESC to exit.]", 0), "[Press a Letter, or ESC to exit.]") == 0);
    assert(strcmp(angdroid_help_browser_text("[Press ESC to exit.]", 1), "按 Esc 離開。") == 0);
    assert(strcmp(angdroid_help_browser_text("[Press ESC to exit.]", 0), "[Press ESC to exit.]") == 0);
    assert(strcmp(angdroid_help_browser_text("[Press Space to advance, or ESC to exit.]", 1), "按空白鍵翻頁，或按 Esc 離開。") == 0);
    assert(strcmp(angdroid_help_browser_text("[Press Space to advance, or ESC to exit.]", 0), "[Press Space to advance, or ESC to exit.]") == 0);
    assert(strcmp(angdroid_help_browser_text("Show: ", 1), "標示：") == 0);
    assert(strcmp(angdroid_help_browser_text("Show: ", 0), "Show: ") == 0);
    assert(strcmp(angdroid_help_browser_text("Find: ", 1), "搜尋：") == 0);
    assert(strcmp(angdroid_help_browser_text("Find: ", 0), "Find: ") == 0);
    assert(strcmp(angdroid_help_browser_text("Goto Line: ", 1), "跳至行號：") == 0);
    assert(strcmp(angdroid_help_browser_text("Goto Line: ", 0), "Goto Line: ") == 0);
    assert(strcmp(angdroid_help_browser_text("Goto File: ", 1), "開啟檔案：") == 0);
    assert(strcmp(angdroid_help_browser_text("Goto File: ", 0), "Goto File: ") == 0);
    {
        const char *unknown = "Untranslated help navigation";
        assert(angdroid_help_browser_text(unknown, 1) == unknown);
        assert(angdroid_help_browser_text(NULL, 1) == NULL);
    }
    puts("PASS: 7 native help-browser labels with English fallback");
    return 0;
}
