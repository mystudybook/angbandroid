/* Native character-sheet presentation-only localization. */
#ifndef ANGDROID_ZH_TW_CHARACTER_SHEET_H
#define ANGDROID_ZH_TW_CHARACTER_SHEET_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_character_sheet_text(const char *english,
    int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "Armor", "護甲" },
        { "Melee", "近戰" },
        { "To-hit", "命中" },
        { "Blows", "攻擊次數" },
        { "Shoot to-dam", "射擊傷害" },
        { "Shots", "射擊次數" },
        { "Saving Throw", "豁免" },
        { "Stealth", "潛行" },
        { "Disarm - phys.", "解除陷阱" },
        { "Disarm - magic", "解除魔法" },
        { "Magic Devices", "魔法裝置" },
        { "Searching", "搜索" },
        { "Infravision", "夜視" },
        { "Speed", "速度" },
        { "Age", "年齡" },
        { "Height", "身高" },
        { "Weight", "體重" },
        { "Turns used:", "經過回合：" },
        { "Game", "遊戲" },
        { "Standard", "標準" },
        { "Resting", "休息" },
        { "Very Bad", "極差" },
        { "Bad", "很差" },
        { "Poor", "差" },
        { "Fair", "普通" },
        { "Good", "良好" },
        { "Very Good", "很好" },
        { "Excellent", "優秀" },
        { "Superb", "卓越" },
        { "Heroic", "英雄" },
        { "Legendary", "傳奇" },
        { "Town", "城鎮" },
        { "Normal", "正常" },
        { "['c' to change name, 'f' to file, 'h' to change mode, or ESC]", "按 c 改名／f 匯出／h 切換畫面／Esc 離開" },
        { "You are not allowed to change your name!", "你無法更改角色名稱！" },
        { "Character dump successful.", "角色資料已成功匯出。" },
        { "Character dump failed!", "角色資料匯出失敗！" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
