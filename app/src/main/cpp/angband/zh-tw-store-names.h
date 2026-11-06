/* Display-only exact match for the seven canonical Angband terrain.txt
 * shop names. Do not change terrain features, parser strings or store IDs.
 */
#ifndef ANGDROID_ZH_TW_STORE_NAMES_H
#define ANGDROID_ZH_TW_STORE_NAMES_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_store_display_name(
    const char *english, int zh_tw)
{
    static const struct {
        const char *en;
        const char *zh;
    } labels[] = {
        { "General Store", "雜貨店" },
        { "Armoury", "防具店" },
        { "Weapon Smiths", "武器店" },
        { "Bookseller", "書店" },
        { "Alchemy Shop", "鍊金商店" },
        { "Magic Shop", "魔法商店" },
        { "Black Market", "黑市" },
        { "Home", "家" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(labels) / sizeof(labels[0]); ++i) {
        if (strcmp(english, labels[i].en) == 0)
            return labels[i].zh;
    }
    return english;
}
#endif
