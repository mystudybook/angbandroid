/* Display-only canonical timed status labels, independent of Android/NDK. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-timed-status.h"

static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

int main(void)
{
    static const struct { const char *en, *zh; } samples[] = {
        { "Haste", "加速" },
        { "Slow", "減速" },
        { "Blind", "失明" },
        { "Paralyzed!", "麻痺" },
        { "Confused", "混亂" },
        { "Afraid", "恐懼" },
        { "Halluc", "幻覺" },
        { "Poisoned", "中毒" },
        { "Mortal Wound", "致命傷" },
        { "Heavy Stun", "重度暈眩" },
        { "Knocked Out", "昏厥" },
        { "Starving", "極度飢餓" },
        { "Faint", "飢餓昏厥" },
        { "Weak", "虛弱" },
        { "Hungry", "飢餓" },
        { "Fed", "已進食" },
        { "Full", "飽足" },
        { "Invuln", "無敵" },
        { "Berserk", "狂暴" },
        { "SInvis", "識破隱形" },
        { "RAcid", "抗酸" },
        { "RConf", "抗混亂" },
        { "FastCast", "快速施法" },
        { "AttFire", "火焰攻擊" },
        { "AttDemon", "惡魔剋星" },
        { "Bloodlust", "嗜血" },
        { "BlkBrth", "黑色氣息" },
        { "FrAct", "行動自由" },
    };
    const char *unknown = "New future grade";
    const char *almost = "Haste ";
    const char *lowercase = "haste";
    size_t i;

    for (i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        CHECK(strcmp(angdroid_timed_status_name(samples[i].en, 1),
                     samples[i].zh) == 0);
        CHECK(angdroid_timed_status_name(samples[i].en, 0)
              == samples[i].en);
    }
    CHECK(angdroid_timed_status_name(NULL, 0) == NULL);
    CHECK(angdroid_timed_status_name(NULL, 1) == NULL);
    CHECK(angdroid_timed_status_name(unknown, 1) == unknown);
    CHECK(angdroid_timed_status_name(almost, 1) == almost);
    CHECK(angdroid_timed_status_name(lowercase, 1) == lowercase);
    CHECK(angdroid_timed_status_name("", 1)[0] == '\0');
    printf("PASS: timed status display translations (%u checks)\n", checks);
    return 0;
}
