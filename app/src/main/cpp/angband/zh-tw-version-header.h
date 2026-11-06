/* Display-only, bounded bilingual version header.
 * Keep the real build/version string unchanged in game metadata and exports.
 * Reject an insufficient buffer rather than displaying partial UTF-8.
 */
#ifndef ANGDROID_ZH_TW_VERSION_HEADER_H
#define ANGDROID_ZH_TW_VERSION_HEADER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static inline bool angdroid_version_header_text(
    char *out, size_t capacity, const char *version, bool zh_tw)
{
    int written;
    if (!out || capacity == 0) return false;
    out[0] = '\0';
    if (!version) version = "";

    written = zh_tw ?
        snprintf(out, capacity, "正在遊玩 %s。按 ? 查看詳細資訊。", version) :
        snprintf(out, capacity,
                 "You are playing %s.  Type '?' for more info.", version);

    if (written < 0 || (size_t)written >= capacity) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif
