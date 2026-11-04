/* Display-only labels for the native Angband targeting help overlay.
 * Targeting commands and color-coded key glyphs remain canonical.
 */
#ifndef ANGDROID_ZH_TW_TARGET_HELP_H
#define ANGDROID_ZH_TW_TARGET_HELP_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_TARGET_HELP_FRAGMENTS 16

static inline const char *angdroid_target_help_text(const char *english,
                                                    int zh_tw)
{
    static const struct {
        const char *english;
        const char *traditional;
    } fragments[ANGDROID_ZH_TW_TARGET_HELP_FRAGMENTS] = {
        { " and ", "／" },
        { " look around. '", " 查看四周；" },
        { "' moves to selection. '", " 移至選取處；" },
        { "' selects player. '", " 選擇玩家；" },
        { "' exits. '", " 離開；" },
        { "' displays details. '", " 檢視詳情；" },
        { "' restricts to interesting places.", " 僅顯示重要地點。" },
        { "' and '", "／" },
        { "' cycle through places. '", " 切換地點；" },
        { "' allows free selection.", " 自由選取。" },
        { " '", " " },
        { "' targets selection.", " 設為目標。" },
        { "' ignores selection.", " 忽略所選物品。" },
        { "', '", "、" },
        { "', and '", "、" },
        { "' select nearest stairs or unexplored area.",
          " 尋找最近樓梯或未探索區域。" }
    };
    size_t i;

    if (!zh_tw || !english) return english;
    for (i = 0; i < ANGDROID_ZH_TW_TARGET_HELP_FRAGMENTS; ++i) {
        if (strcmp(english, fragments[i].english) == 0)
            return fragments[i].traditional;
    }
    return english;
}

#endif /* ANGDROID_ZH_TW_TARGET_HELP_H */
