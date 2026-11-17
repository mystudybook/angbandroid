/* Exact FAangband-only monster spell knowledge labels; unchanged logic. */
#ifndef ANGDROID_ZH_TW_FAANGBAND_MON_SPELL_LORE_H
#define ANGDROID_ZH_TW_FAANGBAND_MON_SPELL_LORE_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_faangband_mon_spell_lore(const char *en,
    int zh_tw)
{
    static const struct {const char *en, *zh;} map[] = {
        { "ice", "冰霜" },
        { "storm", "暴風" },
        { "dragonfire", "龍焰" },
        { "create hunger", "使人飢餓" },
        { "summon swamp creatures", "召喚沼澤生物" },
        { "summon ants", "召喚螞蟻" },
        { "summon winged dragons", "召喚有翼巨龍" },
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(en, map[i].en) == 0) return map[i].zh;
    return en;
}
#endif
