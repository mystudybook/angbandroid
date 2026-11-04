/* Targeting instructions are localized without translating key commands. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-target-help.h"

static void verify(const char *en, const char *zh)
{
    assert(strcmp(angdroid_target_help_text(en, 1), zh) == 0);
    assert(strcmp(angdroid_target_help_text(en, 0), en) == 0);
}

int main(void)
{
    verify(" and ", "／");
    verify(" look around. '", " 查看四周；");
    verify("' moves to selection. '", " 移至選取處；");
    verify("' selects player. '", " 選擇玩家；");
    verify("' exits. '", " 離開；");
    verify("' displays details. '", " 檢視詳情；");
    verify("' restricts to interesting places.", " 僅顯示重要地點。");
    verify("' and '", "／");
    verify("' cycle through places. '", " 切換地點；");
    verify("' allows free selection.", " 自由選取。");
    verify(" '", " ");
    verify("' targets selection.", " 設為目標。");
    verify("' ignores selection.", " 忽略所選物品。");
    verify("', '", "、");
    verify("', and '", "、");
    verify("' select nearest stairs or unexplored area.",
           " 尋找最近樓梯或未探索區域。");

    {
        const char *unknown = "unmapped target help";
        assert(angdroid_target_help_text(unknown, 1) == unknown);
        assert(angdroid_target_help_text(NULL, 1) == NULL);
    }
    puts("PASS: 16 targeting help fragments, English fallback and unknown keys");
    return 0;
}
