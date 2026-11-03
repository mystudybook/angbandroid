/* Display-only Traditional Chinese labels for the original Angband engine.
 * Command keys, menu identifiers, savefiles and parser data stay unchanged.
 */
#ifndef ANGDROID_ZH_TW_UI_H
#define ANGDROID_ZH_TW_UI_H
#include <string.h>
#include <stddef.h>
struct angdroid_ui_pair { const char *en; const char *zh; };
static inline const char *angdroid_game_ui_text(const char *en, int zh_tw) {
    static const struct angdroid_ui_pair labels[] = {
        {"New character based on previous one:", "以先前角色建立新角色："},
        {"['Y': use as is; 'N': redo; 'C': change name/history; '=': set birth options]", "Y 沿用　N 重選　C 改姓名／經歷　= 選項"},
        {"Race affects stats and skills, and may confer resistances and abilities.", "種族影響能力、技能、抗性和天賦。"},
        {"Class affects stats, skills, and other character traits.", "職業影響能力、技能及其他特性。"},
        {"Choose how to generate your intrinsic stats. Point-based is recommended.", "選擇能力值產生方式，建議點數分配。"},
        {"Point-based", "點數分配"},
        {"Standard roller", "隨機擲骰"},
        {"Show birth options", "角色建立選項"},
        {"Select one at random", "隨機選擇"},
        {"Finish with random choices", "全部隨機完成"},
        {"Quit", "離開"},
        {"Help", "說明"},
        {"Reroll", "重新擲骰"},
        {"Retrieve previous", "恢復上次擲骰"},
        {"Accept", "確認"},
        {"Reset", "重設"},
        {"Sell", "降低"},
        {"Buy", "提高"},
        {"['r' to reroll", "[r 重擲"},
        {", 'p' for previous roll", "，p 上次"},
        {" or 'Enter' to accept]", "，Enter 確認]"},
        {"Cost", "費用"},
        {"Total Cost: %2d/%2d", "合計：%2d/%2d"},
        {"[up/down to move, left/right to modify, 'r' to reset, 'Enter' to accept]", "[上下移動／左右調整／r 重設／Enter 確認]"},
        {"A savefile for that name exists.  Overwrite it? ", "已有同名存檔，確定覆寫嗎？ "},
        {"Accept character history? [y/n]", "接受角色背景故事嗎？[y/n]"},
        {"['ESC' to step back, 'S' to start over, or any other key to continue]", "[Esc 返回／S 重新開始／其他鍵繼續]"},
        {"Enter a name for your character (* for a random name): ", "請輸入角色名稱（* 隨機產生）："},
        {"Set to random name", "使用隨機名稱"},
        {"Clear name", "清除名稱"},
        {"New game", "建立新遊戲"},
        {"Use %s: %s", "使用 %s：%s"},
        {"Use %s", "使用 %s"},
        {"Select the save to use (movement keys and enter or mouse) or quit", "請選擇存檔（方向鍵／Enter／滑鼠）；Esc 離開"},
        {"(escape or second mouse button).", "（也可使用滑鼠右鍵返回）"},
        {"The previously selected savefile was unusable.", "上次選取的存檔無法使用。"},
        {"Saving game...", "正在儲存遊戲……"},
        {"Saving game... done.", "遊戲已儲存。"},
        {"Saving game... failed!", "遊戲儲存失敗！"},
        {"Failed to save subwindow preferences", "無法儲存子視窗設定"},
        {"(Up/down or ESCAPE to exit.)", "（上下鍵捲動，Esc 離開）"},
        {"(Press any key to continue.)", "（按任意鍵繼續）"},
        {"Type '?' for help.", "按 ? 查看說明。"},
        {"Do you want to retire? ", "確定讓角色退休嗎？ "},
        {"Do you really want to retire?", "真的要讓角色退休嗎？"},
        {"Please verify RETIRING THIS CHARACTER by typing the '@' sign: ", "確認角色退休，請輸入 @："},
        {"Items", "物品操作"},
        {"Action commands", "角色動作"},
        {"Manage items", "物品管理"},
        {"Information", "資訊"},
        {"Utility", "其他功能"},
        {"Hidden", "隱藏指令"},
        {"Display visible item list", "查看物品清單"},
        {"Display visible monster list", "查看怪物清單"},
        {"Save and quit", "儲存並離開"},
        {"Go up staircase", "走上樓梯"},
        {"Go down staircase", "走下樓梯"},
        {"Rest for a while", "休息一會兒"},
        {"Examine an item", "檢視物品"},
        {"Inventory", "背包"},
        {"Look", "查看"},
        {"Rest", "休息"},
        {"Character", "角色"},
        {"Knowledge", "知識"},
        {"Show Map", "顯示地圖"},
        {"Show Monster List", "怪物清單"},
        {"Show Object List", "物品清單"},
        {"Options", "選項"},
        {"Commands", "指令"},
        {"Other", "其他"},
        {"Item", "物品"},
        {"Action", "動作"},
        {"Item Management", "物品管理"},
        {"Info", "資訊"},
        {"Util", "工具"},
        {"Misc", "其他"},
    };
    size_t i;
    if (!zh_tw || !en) return en;
    for (i = 0; i < sizeof(labels)/sizeof(labels[0]); ++i)
        if (strcmp(en, labels[i].en) == 0) return labels[i].zh;
    return en;
}
#endif
