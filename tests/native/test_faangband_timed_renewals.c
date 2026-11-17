/* FAangband-only on-increase resistance messages, original production lookup. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-timed-messages.h"
static unsigned checks;
#define CHECK(cond) do { assert(cond); ++checks; } while (0)
int main(void)
{
    static const struct {const char *english, *traditional;} cases[] = {
        { "Your resistance to acid feels renewed!", "你的酸液抗性效果已延長！" },
        { "Your resistance to electricity feels renewed!", "你的閃電抗性效果已延長！" },
        { "Your resistance to fire feels renewed!", "你的火焰抗性效果已延長！" },
        { "Your resistance to cold feels renewed!", "你的寒冷抗性效果已延長！" },
        { "Your resistance to poison feels renewed!", "你的毒素抗性效果已延長！" },
        { "Your resistance to confusion feels renewed!", "你的混亂抗性效果已延長！" },
    };
    size_t i;
    CHECK(sizeof(cases) / sizeof(cases[0]) == 6);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const char *actual = angdroid_timed_message(cases[i].english, 1);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, cases[i].traditional) == 0);
        CHECK(angdroid_timed_message(cases[i].english, 0) == cases[i].english);
    }
    /* Original Angband increase messages must remain unchanged. */
    CHECK(strcmp(angdroid_timed_message(
        "You feel more resistant to acid!", 1),
        "你的酸液抗性增強了！") == 0);
    CHECK(strcmp(angdroid_timed_message(
        "You feel more resistant to confusion!", 1),
        "你的混亂抗性增強了！") == 0);
    CHECK(angdroid_timed_message(NULL, 1) == NULL);
    CHECK(angdroid_timed_message(NULL, 0) == NULL);
    CHECK(strcmp(angdroid_timed_message(
        "Your resistance to acid feels renewed?", 1),
        "Your resistance to acid feels renewed?") == 0);
    CHECK(strcmp(angdroid_timed_message(
        "Unknown effect.", 1), "Unknown effect.") == 0);
    printf("PASS: FAangband resistance renewals (%u checks)\n", checks);
    return 0;
}
