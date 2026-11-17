/* Visible FAangband locality labels only, keyed by list-localities.h.
 * Do not change level_name(), world.txt, map identities or save records.
 * Proper-name transliterations here are UI strings, not canonical IDs.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_LOCALITIES_H
#define ANGDROID_ZH_TW_FAANGBAND_LOCALITIES_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_faangband_locality_text(
    const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "Hithaeglir", "迷霧山脈" },
        { "Eriador", "伊利雅德" },
        { "Ered Luin", "藍色山脈" },
        { "Ered Luin South", "南藍色山脈" },
        { "Ossiriand", "歐西瑞安德" },
        { "Taur-Im-Duinath", "杜伊納斯之森" },
        { "East Beleriand", "東貝烈瑞安德" },
        { "Andram", "安德蘭" },
        { "West Beleriand", "西貝烈瑞安德" },
        { "Thargelion", "薩格理安" },
        { "Doriath", "多瑞亞斯" },
        { "Himlad", "希姆拉德" },
        { "Dor Dínen", "多爾迪能" },
        { "Dorthonion", "多松尼安" },
        { "Talath Dirnen", "塔拉斯迪能" },
        { "Brethil", "布瑞希爾" },
        { "Sirion Vale", "西瑞安河谷" },
        { "Fen of Serech", "瑟瑞克沼澤" },
        { "Anfauglith", "安法格利斯" },
        { "Lothlann", "洛斯蘭" },
        { "Amon Rûdh", "阿蒙魯茲" },
        { "Nan Dungortheb", "南頓戈塞布" },
        { "Nargothrond", "納國斯隆德" },
        { "Tol-In-Gaurhoth", "托爾因高洛斯" },
        { "Angband", "安格班" },
        { "Anduin Vale", "安都因河谷" },
        { "Gladden Fields", "格拉頓平原" },
        { "Khazad Dûm", "凱薩督姆" },
        { "Belegost", "貝烈戈斯特" },
        { "Menegroth", "明霓國斯" },
        { "Ephel Brandir", "埃斐爾班迪爾" },
        { "Gondolin", "剛多林" },
        { "Ent Path", "樹人小徑" },
        { "Eriador South", "南伊利雅德" },
        { "Underworld", "地底世界" },
        { "Mountain Top", "山頂" },
        { "Arena", "競技場" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
