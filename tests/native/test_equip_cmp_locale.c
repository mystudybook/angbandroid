/* Native equipment-compare display labels, C99, both locales. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-equip-cmp.h"
static int checks;
static void check(const char *en, const char *zh)
{
    assert(strcmp(angdroid_equip_cmp_text(en, 1), zh) == 0);
    assert(strcmp(angdroid_equip_cmp_text(en, 0), en) == 0);
    ++checks;
}
int main(void)
{
    check("Sorry, could not display.  Press any key.", "無法顯示，請按任意鍵。");
    check("Object comparison", "裝備比較");
    check("Tap a row to select; tap again to confirm", "點選物品列以選取，再點一次確認");
    check("No items to examine", "沒有可查看的裝備。");
    check("[k/up, j/down, p/PgUp, n/PgDn to move; ? for help; ESC to exit]", "k／↑、j／↓、p／PgUp、n／PgDn 移動；? 說明；Esc 離開");
    check("[k/up, j/down, p/PgUp, n/PgDn to move; return to accept]", "k／↑、j／↓、p／PgUp、n／PgDn 移動；Enter 確認");
    check("No items; use q, !, c, or R to change filter", "沒有符合的物品；按 q、!、c 或 R 調整篩選");
    check("Movement/scrolling ---------------------------------", "移動／捲動");
    check("j, down  one line down    k, up    one line up", "j／↓ 下一列；k／↑ 上一列");
    check("n, PgDn  one page down    p, PgUp  one page up", "n／PgDn 下一頁；p／PgUp 上一頁");
    check("space    one page down", "Space 下一頁");
    check("Filtering/searching/sorting ------------------------", "篩選／搜尋／排序");
    check("q        quick filter     !        use opposite quick", "q 快速篩選；! 反向快速篩選");
    check("c        cycle through sources of items", "c 切換物品來源");
    check("r        reverse", "r 反轉排序");
    check("Information ----------------------------------------", "資訊");
    check("v        cycle through attribute views", "v 切換屬性檢視");
    check("I, x     select one or two items for details", "I／x 選取一至兩件物品查看詳情");
    check("Other ----------------------------------------------", "其他");
    check("d        dump to file     R        reset display", "d 匯出檔案；R 重設顯示");
    check("ESC      exit", "Esc 離開");
    check("Press any key to continue", "按任意鍵繼續");
    check("j, down   move selection one line down", "j／↓ 選取下一列");
    check("k, up     move selection one line up", "k／↑ 選取上一列");
    check("n, PgDn   move selection one page up", "n／PgDn 下一頁");
    check("p, PgUp   move selection one page up", "p／PgUp 上一頁");
    check("x         stop selection; if first item, escapes", "x 停止選取；若為第一件則離開");
    check("return    select current item", "Enter 選取目前物品");
    check("ESC       leave selection process", "Esc 離開選取");
    check("Unknown key pressed; ? will list available keys", "未知按鍵；按 ? 查看可用按鍵");
    check("Showing alternate attributes; press v to cycle", "正在顯示其他屬性；按 v 切換");
    check("Only showing goods from stores; press c to change", "只顯示商店商品；按 c 切換");
    check("Showing possessions and goods from stores; press c to change", "顯示持有物品與商店商品；按 c 切換");
    check("Only showing carried items; press c to change", "只顯示攜帶的物品；按 c 切換");
    check("Successfully saved to file", "已成功儲存至檔案");
    check("Failed to save to file!", "無法儲存至檔案！");
    check("Select first item to examine", "請選擇第一件要查看的物品");
    check("Select second item; x to skip", "請選擇第二件物品；按 x 略過");
    check("Did not find attribute with that name; filter unchanged", "找不到該屬性，篩選條件未變更");
    check("Enter 2 or 3 (for stat) character code and return or return to clear ", "輸入 2 至 3 個字元的屬性代碼並按 Enter；直接按 Enter 清除：");
    check("Go to next page", "下一頁");
    check("Go to previous page", "上一頁");
    check("Go to next line", "下一列");
    check("Go to previous line", "上一列");
    check("Cycle equipment source", "切換物品來源");
    check("Cycle attribute view", "切換屬性檢視");
    check("Examine one or two items", "查看一或兩件物品");
    check("Reverse order", "反轉排序");
    check("Reset display", "重設顯示");
    check("Dump as file", "匯出檔案");
    check("Help", "說明");
    check("Skip selection", "略過選取");
    check("Unsupported future label", "Unsupported future label");
    assert(angdroid_equip_cmp_text(NULL, 1) == NULL);
    assert(angdroid_equip_cmp_text(NULL, 0) == NULL);
    checks += 2;
    printf("PASS: equipment comparison bilingual display (%d pairs/guards)\n",
        checks);
    return 0;
}
