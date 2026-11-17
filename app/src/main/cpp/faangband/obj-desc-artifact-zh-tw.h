/* Display-only FAangband artifact suffix selection.
 * The game keeps the canonical English artifact identity and name.
 * Callers must opt in for a known on-screen item, never dumps or spoilers.
 */
#ifndef ANGDROID_FAANGBAND_OBJ_DESC_ARTIFACT_ZH_TW_H
#define ANGDROID_FAANGBAND_OBJ_DESC_ARTIFACT_ZH_TW_H
#include <stdbool.h>
#include "../angband/zh-tw-artifact-names.h"
#include "../angband/zh-tw-faangband-artifact-names.h"

static inline const char *angdroid_faangband_artifact_ui_name(
    const char *english, bool translate)
{
    const char *translated;
    if (!english || !translate) return english;
    translated = angdroid_zh_tw_artifact_name(english);
    if (!translated)
        translated = angdroid_zh_tw_faangband_artifact_name(english);
    return translated ? translated : english;
}
#endif /* ANGDROID_FAANGBAND_OBJ_DESC_ARTIFACT_ZH_TW_H */
