/* Format item-quality ignore action descriptions, not gameplay rules.
 * Quality/type IDs, ignore level, object identities and menu key codes remain
 * English / native. Unknown labels use the complete original English phrase.
 */
#ifndef ANGDROID_ZH_TW_IGNORE_QUALITY_ACTION_H
#define ANGDROID_ZH_TW_IGNORE_QUALITY_ACTION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "zh-tw-ui.h"

static inline bool angdroid_ignore_quality_action_text(
    char *out, size_t cap, const char *quality, const char *type, bool zh_tw)
{
    const char *quality_zh;
    const char *type_zh;
    int n;

    if (!out || !cap) return false;
    out[0] = '\0';
    if (!quality || !type) return false;

    quality_zh = angdroid_game_ui_text(quality, zh_tw);
    type_zh = angdroid_game_ui_text(type, zh_tw);

    /* If either label is not in the shipped translation table, keep
     * the entire English phrase instead of emitting a mixed-language
     * command. This includes modded/unknown ignore types. */
    if (zh_tw && strcmp(quality_zh, quality) != 0 &&
            strcmp(type_zh, type) != 0)
        n = snprintf(out, cap, "忽略所有%s%s", quality_zh, type_zh);
    else
        n = snprintf(out, cap, "All %s %s", quality, type);

    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif
