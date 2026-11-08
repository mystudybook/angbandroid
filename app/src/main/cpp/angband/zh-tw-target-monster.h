/* Display-only monster names/statuses for the targeting and look screens.
 * Only translate a full, source-matched race name embedded in monster_desc:
 * unseen pronouns and hidden monsters must never reveal their race identity.
 * Keep the combat grammar and the canonical race->name entirely unchanged.
 */
#ifndef ANGDROID_ZH_TW_TARGET_MONSTER_H
#define ANGDROID_ZH_TW_TARGET_MONSTER_H
#include <stddef.h>
#include <string.h>
#include "../curses/unicode_display.h"
#include "zh-tw-monsters.h"
#include "zh-tw-unique-monsters.h"

/* Returns 1 only when the original monster_desc actually exposed a race name.
 * On all other inputs, copy the original English description without changes. */
static inline int angdroid_target_monster_display(char *out, size_t capacity,
    const char *english_desc, const char *race_name, int is_unique, int zh_tw)
{
    const char *name;
    const char *start, *suffix;
    size_t used;
    if (!out || capacity == 0) return 0;
    out[0] = '\0';
    if (!english_desc) return 0;
    name = (!race_name || !zh_tw) ? NULL :
        (is_unique ? angdroid_zh_tw_unique_monster_name(race_name) :
            angdroid_zh_tw_monster_list_name(race_name));
    if (!name) {
        angdroid_utf8_copy_complete(out, capacity, english_desc);
        return 0;
    }
    start = english_desc;
    if (strncmp(start, "a ", 2) == 0) start += 2;
    else if (strncmp(start, "an ", 3) == 0) start += 3;
    else if (strncmp(start, "the ", 4) == 0) start += 4;
    else if (strncmp(start, "The ", 4) == 0) start += 4;
    if (strncmp(start, race_name, strlen(race_name)) != 0) {
        angdroid_utf8_copy_complete(out, capacity, english_desc);
        return 0;
    }
    suffix = start + strlen(race_name);
    if (strcmp(suffix, "") && strcmp(suffix, ",") &&
        strcmp(suffix, " (offscreen)") &&
        strcmp(suffix, ", (offscreen)")) {
        angdroid_utf8_copy_complete(out, capacity, english_desc);
        return 0;
    }
    used = angdroid_utf8_append_complete(out, capacity, 0, name);
    if (suffix[0] == ',')
        used = angdroid_utf8_append_complete(out, capacity, used, "，");
    if (strstr(suffix, "(offscreen)") != NULL)
        angdroid_utf8_append_complete(out, capacity, used, "（畫面外）");
    return 1;
}

static inline const char *angdroid_target_monster_intro(const char *en,
    int zh_tw)
{
    if (!en || !zh_tw) return en;
    if (strcmp(en, "You see ") == 0) return "你看到";
    if (strcmp(en, "You sense ") == 0) return "你感應到";
    if (strcmp(en, "You recall ") == 0) return "你回想起";
    return en;
}

/* look_mon_desc() produces an English health label followed by comma-separated
 * timed conditions.  Convert only the presentation; do not touch timed values. */
static inline void angdroid_target_monster_status(char *out, size_t capacity,
    const char *english_status, int zh_tw)
{
    static const struct { const char *en, *zh; } terms[] = {
        { "unhurt", "未受傷" },
        { "undamaged", "沒有損傷" },
        { "somewhat wounded", "輕微受傷" },
        { "somewhat damaged", "輕微受損" },
        { "wounded", "受傷" },
        { "damaged", "受損" },
        { "badly wounded", "身受重傷" },
        { "badly damaged", "嚴重受損" },
        { "almost dead", "瀕死" },
        { "almost destroyed", "幾近毀滅" },
        { "asleep", "睡眠中" },
        { "held", "遭束縛" },
        { "disenchanted", "遭消魔" },
        { "confused", "混亂" },
        { "afraid", "恐懼" },
        { "stunned", "昏迷" },
        { "slowed", "緩速" },
        { "hasted", "加速" },
    };
    const char *part;
    size_t used = 0;
    int first = 1;
    if (!out || capacity == 0) return;
    out[0] = '\0';
    if (!english_status) return;
    if (!zh_tw) {
        angdroid_utf8_copy_complete(out, capacity, english_status);
        return;
    }
    part = english_status;
    while (*part) {
        const char *sep = strstr(part, ", ");
        size_t len = sep ? (size_t)(sep - part) : strlen(part);
        size_t i;
        const char *translated = NULL;
        char unknown[128];
        for (i = 0; i < sizeof(terms) / sizeof(terms[0]); ++i) {
            if (strlen(terms[i].en) == len &&
                strncmp(part, terms[i].en, len) == 0) {
                translated = terms[i].zh;
                break;
            }
        }
        if (!translated) {
            if (len >= sizeof(unknown)) {
                angdroid_utf8_copy_complete(out, capacity, english_status);
                return;
            }
            memcpy(unknown, part, len);
            unknown[len] = '\0';
            translated = unknown;
        }
        if (!first)
            used = angdroid_utf8_append_complete(out, capacity, used, "、");
        used = angdroid_utf8_append_complete(out, capacity, used, translated);
        first = 0;
        if (!sep) break;
        part = sep + 2;
    }
}
#endif /* ANGDROID_ZH_TW_TARGET_MONSTER_H */
