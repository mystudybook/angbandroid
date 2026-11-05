/* Display-only labels for the original Angband equipment comparison UI.
 * Command keys, comparison states, filter codes and exported files are not
 * translated or modified.
 */
#ifndef ANGDROID_ZH_TW_EQUIP_CMP_H
#define ANGDROID_ZH_TW_EQUIP_CMP_H
#include <stddef.h>
#include <string.h>
struct angdroid_equip_cmp_pair { const char *en; const char *zh; };
static inline const char *angdroid_equip_cmp_text(const char *english,
    int zh_tw)
{
    static const struct angdroid_equip_cmp_pair labels[] = {
        { "Sorry, could not display.  Press any key.", "無法顯示，請按任意鍵。" },
        { "[k/up, j/down, p/PgUp, n/PgDn to move; ? for help; ESC to exit]", "k／↑、j／↓、p／PgUp、n／PgDn 移動；? 說明；Esc 離開" },
        { "[k/up, j/down, p/PgUp, n/PgDn to move; return to accept]", "k／↑、j／↓、p／PgUp、n／PgDn 移動；Enter 確認" },
        { "No items; use q, !, c, or R to change filter", "沒有符合的物品；按 q、!、c 或 R 調整篩選" },
        { "Movement/scrolling ---------------------------------", "移動／捲動" },
        { "j, down  one line down    k, up    one line up", "j／↓ 下一列；k／↑ 上一列" },
        { "n, PgDn  one page down    p, PgUp  one page up", "n／PgDn 下一頁；p／PgUp 上一頁" },
        { "space    one page down", "Space 下一頁" },
        { "Filtering/searching/sorting ------------------------", "篩選／搜尋／排序" },
        { "q        quick filter     !        use opposite quick", "q 快速篩選；! 反向快速篩選" },
        { "c        cycle through sources of items", "c 切換物品來源" },
        { "r        reverse", "r 反轉排序" },
        { "Information ----------------------------------------", "資訊" },
        { "v        cycle through attribute views", "v 切換屬性檢視" },
        { "I, x     select one or two items for details", "I／x 選取一至兩件物品查看詳情" },
        { "Other ----------------------------------------------", "其他" },
        { "d        dump to file     R        reset display", "d 匯出檔案；R 重設顯示" },
        { "ESC      exit", "Esc 離開" },
        { "Press any key to continue", "按任意鍵繼續" },
        { "j, down   move selection one line down", "j／↓ 選取下一列" },
        { "k, up     move selection one line up", "k／↑ 選取上一列" },
        { "n, PgDn   move selection one page up", "n／PgDn 下一頁" },
        { "p, PgUp   move selection one page up", "p／PgUp 上一頁" },
        { "x         stop selection; if first item, escapes", "x 停止選取；若為第一件則離開" },
        { "return    select current item", "Enter 選取目前物品" },
        { "ESC       leave selection process", "Esc 離開選取" },
        { "Unknown key pressed; ? will list available keys", "未知按鍵；按 ? 查看可用按鍵" },
        { "Showing alternate attributes; press v to cycle", "正在顯示其他屬性；按 v 切換" },
        { "Only showing goods from stores; press c to change", "只顯示商店商品；按 c 切換" },
        { "Showing possessions and goods from stores; press c to change", "顯示持有物品與商店商品；按 c 切換" },
        { "Only showing carried items; press c to change", "只顯示攜帶的物品；按 c 切換" },
        { "Successfully saved to file", "已成功儲存至檔案" },
        { "Failed to save to file!", "無法儲存至檔案！" },
        { "Select first item to examine", "請選擇第一件要查看的物品" },
        { "Select second item; x to skip", "請選擇第二件物品；按 x 略過" },
        { "Did not find attribute with that name; filter unchanged", "找不到該屬性，篩選條件未變更" },
        { "Enter 2 or 3 (for stat) character code and return or return to clear ", "輸入 2 至 3 個字元的屬性代碼並按 Enter；直接按 Enter 清除：" },
        { "Go to next page", "下一頁" },
        { "Go to previous page", "上一頁" },
        { "Go to next line", "下一列" },
        { "Go to previous line", "上一列" },
        { "Cycle equipment source", "切換物品來源" },
        { "Cycle attribute view", "切換屬性檢視" },
        { "Examine one or two items", "查看一或兩件物品" },
        { "Reverse order", "反轉排序" },
        { "Reset display", "重設顯示" },
        { "Dump as file", "匯出檔案" },
        { "Help", "說明" },
        { "Skip selection", "略過選取" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(labels) / sizeof(labels[0]); ++i)
        if (strcmp(english, labels[i].en) == 0) return labels[i].zh;
    return english;
}
#endif
