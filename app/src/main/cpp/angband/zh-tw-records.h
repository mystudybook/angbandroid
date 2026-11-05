/* Original Angband character-history/high-score display labels.
 * Never replace the race/class IDs, high-score records or history dump text.
 */
#ifndef ANGDROID_ZH_TW_RECORDS_H
#define ANGDROID_ZH_TW_RECORDS_H
#include <stddef.h>
#include <string.h>

struct angdroid_record_pair { const char *en; const char *zh; };
static inline const char *angdroid_record_lookup(
    const char *en, int zh_tw, const struct angdroid_record_pair *pairs,
    size_t count)
{
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < count; ++i)
        if (strcmp(en, pairs[i].en) == 0) return pairs[i].zh;
    return en;
}

static inline const char *angdroid_record_ui_text(const char *en, int zh_tw)
{
    static const struct angdroid_record_pair map[] = {
        { "[Player history]", "【角色歷史】" },
        { " (LOST)", "（已遺失）" },
        { "[Arrow keys scroll, p/PgUp for previous page, n/PgDn for next page, ESC to exit.]",
          "方向鍵捲動／p、PgUp 上頁／n、PgDn 下頁／Esc 離開" },
        { "[Press ESC to exit, up for prior page, any other key for next page.]",
          "Esc 離開／上方向鍵上一頁／其他鍵下一頁" },
        { "[Press ESC to exit, any other key to page forward till done.]",
          "Esc 離開／其他鍵翻至下一頁" },
    };
    return angdroid_record_lookup(en, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
}

static inline const char *angdroid_record_race(const char *en, int zh_tw)
{
    static const struct angdroid_record_pair map[] = {
        { "Human", "人類" },
        { "Half-Elf", "半精靈" },
        { "Elf", "精靈" },
        { "Hobbit", "哈比人" },
        { "Gnome", "地精" },
        { "Dwarf", "矮人" },
        { "Half-Orc", "半獸人" },
        { "Half-Troll", "半巨魔" },
        { "Dunadan", "杜內丹人" },
        { "High-Elf", "高等精靈" },
        { "Kobold", "狗頭人" },
    };
    return angdroid_record_lookup(en, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
}

static inline const char *angdroid_record_class(const char *en, int zh_tw)
{
    static const struct angdroid_record_pair map[] = {
        { "Warrior", "戰士" },
        { "Mage", "法師" },
        { "Druid", "德魯伊" },
        { "Priest", "祭司" },
        { "Necromancer", "死靈法師" },
        { "Paladin", "聖騎士" },
        { "Rogue", "盜賊" },
        { "Ranger", "遊俠" },
        { "Blackguard", "黑衛士" },
    };
    return angdroid_record_lookup(en, zh_tw, map,
        sizeof(map) / sizeof(map[0]));
}
#endif
