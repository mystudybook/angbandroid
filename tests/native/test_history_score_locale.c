/* Record labels and exact canonical race/class coverage: native C99. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-records.h"
static int checks;
static void verify(const char *(*lookup)(const char*,int),
                   const char *en, const char *zh)
{
    assert(strcmp(lookup(en, 1), zh) == 0);
    assert(strcmp(lookup(en, 0), en) == 0);
    checks += 2;
}
int main(void)
{
    verify(angdroid_record_race, "Human", "人類");
    verify(angdroid_record_race, "Half-Elf", "半精靈");
    verify(angdroid_record_race, "Elf", "精靈");
    verify(angdroid_record_race, "Hobbit", "哈比人");
    verify(angdroid_record_race, "Gnome", "地精");
    verify(angdroid_record_race, "Dwarf", "矮人");
    verify(angdroid_record_race, "Half-Orc", "半獸人");
    verify(angdroid_record_race, "Half-Troll", "半巨魔");
    verify(angdroid_record_race, "Dunadan", "杜內丹人");
    verify(angdroid_record_race, "High-Elf", "高等精靈");
    verify(angdroid_record_race, "Kobold", "狗頭人");
    verify(angdroid_record_class, "Warrior", "戰士");
    verify(angdroid_record_class, "Mage", "法師");
    verify(angdroid_record_class, "Druid", "德魯伊");
    verify(angdroid_record_class, "Priest", "祭司");
    verify(angdroid_record_class, "Necromancer", "死靈法師");
    verify(angdroid_record_class, "Paladin", "聖騎士");
    verify(angdroid_record_class, "Rogue", "盜賊");
    verify(angdroid_record_class, "Ranger", "遊俠");
    verify(angdroid_record_class, "Blackguard", "黑衛士");
    verify(angdroid_record_ui_text, "[Player history]", "【角色歷史】");
    verify(angdroid_record_ui_text, " (LOST)", "（已遺失）");
    verify(angdroid_record_ui_text,
        "[Arrow keys scroll, p/PgUp for previous page, n/PgDn for next page, ESC to exit.]",
        "方向鍵捲動／p、PgUp 上頁／n、PgDn 下頁／Esc 離開");
    verify(angdroid_record_ui_text,
        "[Press ESC to exit, up for prior page, any other key for next page.]",
        "Esc 離開／上方向鍵上一頁／其他鍵下一頁");
    verify(angdroid_record_ui_text,
        "[Press ESC to exit, any other key to page forward till done.]",
        "Esc 離開／其他鍵翻至下一頁");
    verify(angdroid_record_race, "Unknown Race", "Unknown Race");
    verify(angdroid_record_class, "Custom Class", "Custom Class");
    verify(angdroid_record_ui_text, "Unused label", "Unused label");
    assert(angdroid_record_race(NULL, 1) == NULL);
    assert(angdroid_record_class(NULL, 1) == NULL);
    assert(angdroid_record_ui_text(NULL, 1) == NULL);
    checks += 3;
    printf("PASS: history and high-score labels (%d checks)\n", checks);
    return 0;
}
