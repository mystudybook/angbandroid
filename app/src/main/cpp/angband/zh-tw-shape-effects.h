/* Display-only names for the status and projection effects appearing
 * in variant_lib/angband/gamedata/shape.txt. Unknown names stay English.
 */
#ifndef ANGDROID_ZH_TW_SHAPE_EFFECTS_H
#define ANGDROID_ZH_TW_SHAPE_EFFECTS_H
#include <stddef.h>
#include <string.h>

struct angdroid_shape_effect_pair { const char *en; const char *zh; };
static inline const char *angdroid_shape_effect_term(const char *en)
{
    static const struct angdroid_shape_effect_pair map[] = {
        { "poisoning", "中毒" },
        { "stunning", "暈眩" },
        { "fear", "恐懼" },
        { "berserk rage", "狂暴" },
        { "temporary hitpoint siphoning", "暫時吸取生命" },
        { "causes monsters to flee", "使視線內怪物逃跑" },
    };
    size_t i;
    if (!en) return NULL;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i) {
        if (strcmp(en, map[i].en) == 0) return map[i].zh;
    }
    return en;
}

static inline int angdroid_shape_effect_known(const char *en)
{
    return en && angdroid_shape_effect_term(en) != en;
}
#endif
