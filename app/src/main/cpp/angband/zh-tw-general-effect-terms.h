/* Display-only terms for general effect formatted messages. These do not
 * change the canonical object property/ego-brand names, stat indices or IDs.
 */
#ifndef ANGDROID_ZH_TW_GENERAL_EFFECT_TERMS_H
#define ANGDROID_ZH_TW_GENERAL_EFFECT_TERMS_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_general_stat_adjective(const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "weak", "虛弱" },
        { "stupid", "愚鈍" },
        { "naive", "天真" },
        { "clumsy", "笨拙" },
        { "sickly", "病弱" },
        { "strong", "強壯" },
        { "smart", "聰明" },
        { "wise", "睿智" },
        { "dextrous", "靈巧" },
        { "healthy", "健康" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}

static inline const char *angdroid_general_brand_name(const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "Flame", "火焰" },
        { "Frost", "冰霜" },
        { "Venom", "毒素" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}

static inline const char *angdroid_general_item_type(const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "staff", "法杖" },
        { "wand", "魔杖" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
