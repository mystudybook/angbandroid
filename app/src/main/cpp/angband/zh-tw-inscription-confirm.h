/* Safe, display-only localization of inscription !* / !key confirmation.
 * Match native cmd_verb() English strings exactly: do not translate command
 * identifiers, key bindings, inscriptions or any source command descriptors.
 * Unrecognized verbs keep the original "Really X" English wording.
 */
#ifndef ANGDROID_ZH_TW_INSCRIPTION_CONFIRM_H
#define ANGDROID_ZH_TW_INSCRIPTION_CONFIRM_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static inline bool angdroid_inscription_confirm_prefix(
    char *out, size_t cap, const char *verb, bool zh_tw)
{
    static const struct { const char *english; const char *zh; } actions[] = {
        { "wear or wield", "裝備" },
        { "take off", "卸下" },
        { "drop", "丟棄" },
        { "eat", "食用" },
        { "quaff", "飲用" },
        { "zap", "啟動魔棒" },
        { "use", "使用" },
        { "aim", "瞄準魔杖" },
        { "read", "閱讀" },
        { "activate", "啟動" },
        { "refuel with", "補充燃料" },
        { "fire", "發射" },
        { "throw", "投擲" },
        { "inscribe", "刻字" },
        { "un-inscribe", "清除刻字" },
        { "study", "研讀" },
        { "cast", "施法" },
        { "sell", "出售" },
        { "stash", "存放" },
        { "buy", "購買" },
        { "retrieve", "取出" },
    };
    const char *translated = NULL;
    size_t i;
    int n;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (!verb) verb = "do that with";
    if (zh_tw) {
        for (i = 0; i < sizeof(actions) / sizeof(actions[0]); ++i) {
            if (strcmp(verb, actions[i].english) == 0) {
                translated = actions[i].zh;
                break;
            }
        }
    }
    n = translated ? snprintf(out, cap, "確定要%s", translated) :
        snprintf(out, cap, "Really %s", verb);
    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif
