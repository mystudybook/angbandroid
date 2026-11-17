/* FAangband flavor labels missing from the standard Angband flavor
 * dictionary. Display-only: source flavor records and shuffle stay intact.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_FLAVOR_NAMES_H
#define ANGDROID_ZH_TW_FAANGBAND_FLAVOR_NAMES_H
#include <string.h>
static inline const char *angdroid_zh_tw_faangband_flavor_name(
    const char *english)
{
    if (!english) return NULL;
    if (strcmp(english, "Adamantium") == 0) return "精金";
    if (strcmp(english, "Damascene Steel") == 0) return "大馬士革鋼";
    return NULL;
}
#endif /* ANGDROID_ZH_TW_FAANGBAND_FLAVOR_NAMES_H */
