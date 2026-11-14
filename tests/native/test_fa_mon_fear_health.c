/* Host C99 regression for FAangband fear percentage and invalid max HP. */
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app/src/main/cpp/faangband/mon-fear-health.h"

static unsigned checks;
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
    return 1; } ++checks; } while (0)

int main(void)
{
    int percentage = -123;
    int hp, max_hp;
    static const int invalid[] = {INT16_MIN, -1, 0};
    size_t i;

    CHECK(angdroid_fa_mon_fear_health_percentage(100, 100, &percentage));
    CHECK(percentage == 100);
    CHECK(angdroid_fa_mon_fear_health_percentage(0, 100, &percentage));
    CHECK(percentage == 0);
    CHECK(angdroid_fa_mon_fear_health_percentage(95, 100, &percentage));
    CHECK(percentage == 95);
    CHECK(angdroid_fa_mon_fear_health_percentage(10, 100, &percentage));
    CHECK(percentage == 10);
    CHECK(angdroid_fa_mon_fear_health_percentage(INT16_MAX, 1, &percentage));
    CHECK(percentage == 3276700);
    CHECK(angdroid_fa_mon_fear_health_percentage(INT16_MIN, 1, &percentage));
    CHECK(percentage == -3276800);
    CHECK(angdroid_fa_mon_fear_health_percentage(1, INT16_MAX, &percentage));
    CHECK(percentage == 0);

    /* Reject zero and negative maximum HP without writing a result. */
    for (i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        percentage = 777;
        CHECK(!angdroid_fa_mon_fear_health_percentage(5,
            (int16_t)invalid[i], &percentage));
        CHECK(percentage == 777);
    }
    CHECK(!angdroid_fa_mon_fear_health_percentage(5, 10, NULL));

    /* Match the old calculation across normal monster HP pairs. */
    for (hp = 0; hp <= INT16_MAX; hp += 131) {
        for (max_hp = 1; max_hp <= INT16_MAX; max_hp += 173) {
            int expected = (int)((100L * hp) / max_hp);
            CHECK(angdroid_fa_mon_fear_health_percentage(
                (int16_t)hp, (int16_t)max_hp, &percentage));
            CHECK(percentage == expected);
        }
    }

    printf("PASS: %u FAangband monster fear percentage checks\n", checks);
    return 0;
}
