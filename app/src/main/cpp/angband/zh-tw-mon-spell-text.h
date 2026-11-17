/* Original Angband monster spell messages. All source message text stays
 * unchanged; translated display templates preserve canonical dynamic tags.
 */
#ifndef ANGDROID_ZH_TW_MON_SPELL_TEXT_H
#define ANGDROID_ZH_TW_MON_SPELL_TEXT_H
#include <stddef.h>
#include <string.h>
#include "zh-tw-faangband-mon-spell-messages.h"
static inline const char *angdroid_mon_spell_text(const char *english,
    int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
#include "zh-tw-mon-spell-0.inc"
#include "zh-tw-mon-spell-1.inc"
#include "zh-tw-mon-spell-2.inc"
#include "zh-tw-mon-spell-3.inc"
#include "zh-tw-mon-spell-alt.inc"
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return angdroid_faangband_mon_spell_text(english, zh_tw);
}
#endif
