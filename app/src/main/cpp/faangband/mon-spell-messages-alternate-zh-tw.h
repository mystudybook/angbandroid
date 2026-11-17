/*
 * FAangband race-specific monster spell message overrides from
 * variant_lib/faangband/gamedata/monster.txt.
 *
 * Reuse the existing source-exact seventeen-entry display dictionary.
 * Do not modify alternate-message selection, monster IDs or the parser.
 */
#ifndef ANGDROID_FA_MON_SPELL_MESSAGES_ALTERNATE_ZH_TW_H
#define ANGDROID_FA_MON_SPELL_MESSAGES_ALTERNATE_ZH_TW_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_fa_spell_message_alternate(
    const char *english, bool zh_tw)
{
    static const struct {
        const char *en;
        const char *zh;
    } messages[] = {
#include "../angband/zh-tw-mon-spell-alt.inc"
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof messages / sizeof messages[0]; ++i) {
        if (strcmp(messages[i].en, english) == 0)
            return messages[i].zh;
    }
    return english;
}

#endif /* ANGDROID_FA_MON_SPELL_MESSAGES_ALTERNATE_ZH_TW_H */
