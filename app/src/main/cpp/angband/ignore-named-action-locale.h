/* Complete bilingual captions for ignore-by-kind and ignore-by-ego actions.
 * Caller performs all item knowledge, selected action, and ID checks.
 * Never return a truncated UTF-8 string as a visible menu action.
 */
#ifndef ANGDROID_IGNORE_NAMED_ACTION_LOCALE_H
#define ANGDROID_IGNORE_NAMED_ACTION_LOCALE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static inline bool angdroid_ignore_named_action_text(
    char *out, size_t capacity, const char *name, bool undo, bool zh_tw)
{
    const char *prefix;
    int n;
    if (!out || !capacity) return false;
    out[0] = '\0';
    if (!name || !*name) return false;
    prefix = zh_tw ?
        (undo ? "取消忽略全部 " : "全部忽略 ") :
        (undo ? "Unignore all " : "All ");
    n = snprintf(out, capacity, "%s%s", prefix, name);
    if (n < 0 || (size_t)n >= capacity) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif
