/* Standalone C99 regression for knowledge-browser kill summary captions.
 * Render text only: no monster lore or gameplay counters are changed.
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-knowledge-kill-summary.h"

static unsigned long checks;
#define CHECK(expr) do { ++checks; if (!(expr)) { \
    fprintf(stderr, "FAIL at %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    static const struct {
        bool uniques;
        int known, group, total;
        const char *zh, *en;
    } scenarios[] = {
        {true, 3, 2, 19, "已知特殊怪物 3 隻，已擊敗 2 隻。",
            "3 known uniques, 2 slain."},
        {false, 0, 0, 0, "擊敗怪物：本類 0／全部 0",
            "Creatures slain: 0/0 (in group/in total)"},
        {false, 8, 17, 245, "擊敗怪物：本類 17／全部 245",
            "Creatures slain: 17/245 (in group/in total)"},
        {true, 120, 50, 900, "已知特殊怪物 120 隻，已擊敗 50 隻。",
            "120 known uniques, 50 slain."}
    };
    char buffer[160];
    size_t i, cap;
    int zh;

    for (i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); ++i) {
        for (zh = 0; zh <= 1; ++zh) {
            const char *expected = zh ? scenarios[i].zh : scenarios[i].en;
            size_t expected_length = strlen(expected);
            for (cap = 0; cap <= sizeof(buffer); ++cap) {
                bool success;
                memset(buffer, 'X', sizeof(buffer));
                success = angdroid_knowledge_kill_summary(buffer, cap,
                    scenarios[i].uniques, scenarios[i].known,
                    scenarios[i].group, scenarios[i].total, zh != 0);
                CHECK(success == (expected_length < cap));
                if (cap > 0) {
                    if (success) CHECK(strcmp(buffer, expected) == 0);
                    else CHECK(buffer[0] == '\0');
                }
                if (cap < sizeof(buffer))
                    CHECK(buffer[sizeof(buffer) - 1] == 'X');
            }
        }
    }
    CHECK(!angdroid_knowledge_kill_summary(
        NULL, sizeof(buffer), true, 1, 2, 3, true));
    CHECK(!angdroid_knowledge_kill_summary(
        buffer, 0, false, 1, 2, 3, true));
    printf("PASS: %lu zh-TW/English monster totals and buffer checks\n", checks);
    return 0;
}
