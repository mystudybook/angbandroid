/* Exact FAangband-only monster base identity + source description. UI only. */
#ifndef ANGDROID_ZH_TW_FAANGBAND_MONSTER_BASE_H
#define ANGDROID_ZH_TW_FAANGBAND_MONSTER_BASE_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_faangband_monster_base_text(
    const char *name, const char *source, int zh_tw)
{
    if (!name || !source || !zh_tw) return source;
    if (strcmp(name, "flying dragon") == 0 &&
        strcmp(source, "Winged Dragon") == 0) return "有翼巨龍";
    return source;
}
#endif
