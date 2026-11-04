/* Presentation-only native help-browser labels in Traditional Chinese.
 * Navigation key codes, file names and RST markup remain untouched.
 */
#ifndef ANGDROID_ZH_TW_HELP_BROWSER_H
#define ANGDROID_ZH_TW_HELP_BROWSER_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_HELP_BROWSER_LABELS 7
static inline const char *angdroid_help_browser_text(const char *english,
                                                    int zh_tw)
{
    static const struct { const char *en; const char *zh; }
        labels[ANGDROID_ZH_TW_HELP_BROWSER_LABELS] = {
        { "[Press a Letter, or ESC to exit.]", "按字母鍵選擇，或按 Esc 離開。" },
        { "[Press ESC to exit.]", "按 Esc 離開。" },
        { "[Press Space to advance, or ESC to exit.]", "按空白鍵翻頁，或按 Esc 離開。" },
        { "Show: ", "標示：" },
        { "Find: ", "搜尋：" },
        { "Goto Line: ", "跳至行號：" },
        { "Goto File: ", "開啟檔案：" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < ANGDROID_ZH_TW_HELP_BROWSER_LABELS; ++i) {
        if (strcmp(english, labels[i].en) == 0) return labels[i].zh;
    }
    return english;
}
#endif /* ANGDROID_ZH_TW_HELP_BROWSER_H */
