/* Screen strings and English fallback, native C99. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-character-sheet.h"
static int checks;
static void pair(const char *en, const char *zh) {
    assert(strcmp(angdroid_character_sheet_text(en, 1), zh) == 0);
    assert(strcmp(angdroid_character_sheet_text(en, 0), en) == 0);
    checks += 2;
}
int main(void) {
    pair("Armor", "護甲");
    pair("Melee", "近戰");
    pair("To-hit", "命中");
    pair("Blows", "攻擊次數");
    pair("Shoot to-dam", "射擊傷害");
    pair("Shots", "射擊次數");
    pair("Saving Throw", "豁免");
    pair("Stealth", "潛行");
    pair("Disarm - phys.", "解除陷阱");
    pair("Disarm - magic", "解除魔法");
    pair("Magic Devices", "魔法裝置");
    pair("Searching", "搜索");
    pair("Infravision", "夜視");
    pair("Speed", "速度");
    pair("Age", "年齡");
    pair("Height", "身高");
    pair("Weight", "體重");
    pair("Turns used:", "經過回合：");
    pair("Game", "遊戲");
    pair("Standard", "標準");
    pair("Resting", "休息");
    pair("Very Bad", "極差");
    pair("Bad", "很差");
    pair("Poor", "差");
    pair("Fair", "普通");
    pair("Good", "良好");
    pair("Very Good", "很好");
    pair("Excellent", "優秀");
    pair("Superb", "卓越");
    pair("Heroic", "英雄");
    pair("Legendary", "傳奇");
    pair("Town", "城鎮");
    pair("Normal", "正常");
    pair("['c' to change name, 'f' to file, 'h' to change mode, or ESC]", "按 c 改名／f 匯出／h 切換畫面／Esc 離開");
    pair("You are not allowed to change your name!", "你無法更改角色名稱！");
    pair("Character dump successful.", "角色資料已成功匯出。");
    pair("Character dump failed!", "角色資料匯出失敗！");
    const char *unknown = "A future character property";
    assert(angdroid_character_sheet_text(unknown, 1) == unknown);
    assert(angdroid_character_sheet_text(NULL, 1) == NULL);
    assert(angdroid_character_sheet_text(NULL, 0) == NULL);
    checks += 3;
    printf("PASS: %d bilingual character-sheet checks\n", checks);
    return 0;
}
