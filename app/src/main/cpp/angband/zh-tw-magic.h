/* Original Angband realm.txt player-facing vocabulary.
 * Render-only mapping: spell realm IDs and saved records remain English. */
#ifndef ANGDROID_ZH_TW_MAGIC_H
#define ANGDROID_ZH_TW_MAGIC_H
#include <string.h>
#include <stddef.h>
struct angdroid_magic_term { const char *en; const char *zh; };
static inline const char *angdroid_zh_tw_magic_term(const char *term, int zh_tw)
{
    static const struct angdroid_magic_term terms[] = {
        { "cast", "施放" },
        { "recite", "誦念" },
        { "chant", "吟唱" },
        { "perform", "舉行" },
        { "study", "研讀" },
        { "spell", "法術" },
        { "prayer", "祈禱" },
        { "verse", "詠唱" },
        { "ritual", "儀式" },
    };
    size_t i;
    if (!zh_tw || !term) return term;
    for (i = 0; i < sizeof(terms) / sizeof(terms[0]); ++i)
        if (strcmp(term, terms[i].en) == 0) return terms[i].zh;
    return term;
}
#endif
