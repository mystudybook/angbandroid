/* A render-only formatter for monster knowledge totals.
 * No creature counters, visibility, save data or game rules change here.
 */
#ifndef ANGDROID_ZH_TW_KNOWLEDGE_KILL_SUMMARY_H
#define ANGDROID_ZH_TW_KNOWLEDGE_KILL_SUMMARY_H
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

static inline bool angdroid_knowledge_kill_summary(char *out, size_t capacity,
    bool uniques, int known, int group_kills, int total_kills, bool zh_tw)
{
    int n;
    if (!out || capacity == 0) return false;
    out[0] = '\0';
    if (uniques) {
        n = zh_tw
            ? snprintf(out, capacity, "已知特殊怪物 %d 隻，已擊敗 %d 隻。",
                known, group_kills)
            : snprintf(out, capacity, "%d known uniques, %d slain.",
                known, group_kills);
    } else {
        n = zh_tw
            ? snprintf(out, capacity, "擊敗怪物：本類 %d／全部 %d",
                group_kills, total_kills)
            : snprintf(out, capacity,
                "Creatures slain: %d/%d (in group/in total)",
                group_kills, total_kills);
    }
    if (n < 0 || (size_t)n >= capacity) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif /* ANGDROID_ZH_TW_KNOWLEDGE_KILL_SUMMARY_H */
