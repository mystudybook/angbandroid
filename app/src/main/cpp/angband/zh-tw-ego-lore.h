/* The five original ego-item descriptions from ego_item.txt.
 * Presentation only: exact canonical English ego name -> zh-TW paragraph.
 * Not an ego identity or save-format conversion. English fallback is NULL.
 */
#ifndef ANGDROID_ZH_TW_EGO_LORE_H
#define ANGDROID_ZH_TW_EGO_LORE_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_EGO_LORE 5
static inline const char *angdroid_zh_tw_ego_flavor(const char *name)
{
    static const struct { const char *en; const char *zh; }
        entries[ANGDROID_ZH_TW_EGO_LORE] = {
        { "of Accuracy", "它沒有特殊能力，但命中加值可能異常地高。" },
        { "of Power", "它沒有特殊能力，但傷害加值可能異常地高。" },
        { "of Wounding", "重創彈藥通常具有異常高的命中與傷害加值。" },
        { "of Slaying", "它能提高你的命中率與造成的傷害。" },
        { "of Craftsmanship", "這件盔甲做工精細，既堅固耐用，又不會妨礙行動。" },
    };
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_EGO_LORE; ++i)
        if (strcmp(name, entries[i].en) == 0) return entries[i].zh;
    return NULL;
}
#endif
