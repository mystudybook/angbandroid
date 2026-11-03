/*
 * Display-only Traditional Chinese text for the Angband death menu.
 * Do not alter menu tags, command IDs, data records or saved-game fields.
 * English remains the fallback for devices not configured for zh-TW.
 */
#ifndef ANGDROID_ZH_TW_DEATH_H
#define ANGDROID_ZH_TW_DEATH_H

#include <stddef.h>

#define ANGDROID_DEATH_MENU_ITEMS 9

static inline const char *angdroid_death_menu_label(int index, int use_zh_tw)
{
    static const char *const english[ANGDROID_DEATH_MENU_ITEMS] = {
        "Information", "Messages", "File dump", "View scores",
        "Examine items", "History", "Spoilers", "New Game", "Quit"
    };
    static const char *const traditional[ANGDROID_DEATH_MENU_ITEMS] = {
        "角色資訊", "訊息記錄", "匯出角色資料", "查看排行榜",
        "檢視物品", "角色歷史", "劇透資料", "開始新遊戲", "退出"
    };

    if (index < 0 || index >= ANGDROID_DEATH_MENU_ITEMS) return NULL;
    return use_zh_tw ? traditional[index] : english[index];
}

enum angdroid_death_prompt {
    ANGDROID_DEATH_NEW_GAME,
    ANGDROID_DEATH_QUIT,
    ANGDROID_DEATH_EXAMINE,
    ANGDROID_DEATH_NO_ITEMS,
    ANGDROID_DEATH_DUMP_SUCCESS,
    ANGDROID_DEATH_DUMP_FAILED,
    ANGDROID_DEATH_MORE_INFORMATION,
    ANGDROID_DEATH_EQUIPMENT,
    ANGDROID_DEATH_INVENTORY,
    ANGDROID_DEATH_QUIVER,
    ANGDROID_DEATH_HOME_PAGE,
    ANGDROID_DEATH_PROMPT_COUNT
};

static inline const char *angdroid_death_prompt(
    enum angdroid_death_prompt index, int use_zh_tw)
{
    static const char *const english[ANGDROID_DEATH_PROMPT_COUNT] = {
        "Start a new game? ",
        "Do you want to quit? ",
        "Examine which item? ",
        "You have nothing to examine.",
        "Character dump successful.",
        "Character dump failed!",
        "Hit any key to see more information: ",
        "You are using: -more-",
        "You are carrying: -more-",
        "Your quiver holds: -more-",
        "Your home contains (page %d): -more-"
    };
    static const char *const traditional[ANGDROID_DEATH_PROMPT_COUNT] = {
        "要開始新遊戲嗎？ ",
        "確定要退出嗎？ ",
        "要檢視哪件物品？ ",
        "沒有可檢視的物品。",
        "角色資料匯出成功。",
        "角色資料匯出失敗！",
        "按任意鍵查看更多資訊： ",
        "目前裝備：－更多－",
        "攜帶物品：－更多－",
        "箭袋內容：－更多－",
        "住家庫存（第 %d 頁）：－更多－"
    };

    if (index < 0 || index >= ANGDROID_DEATH_PROMPT_COUNT) return NULL;
    return use_zh_tw ? traditional[index] : english[index];
}

#endif
