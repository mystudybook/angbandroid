/* Host C99 regression for actual FAangband int16_t monster HP arithmetic. */
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "app/src/main/cpp/faangband/mon-damage-hp.h"

static unsigned checks;
#define CHECK(cond) do { if (!(cond)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #cond); \
    return 1; } ++checks; } while (0)

int main(void)
{
    static const int damages[] = {
        INT_MIN, -100000, -32768, -100, -1, 0, 1, 30, 100,
        101, 32767, 32768, 50000, 100000, INT_MAX
    };
    int hp;
    size_t j;

    CHECK(angdroid_fa_mon_hp_after_damage(100, 30) == 70);
    CHECK(angdroid_fa_mon_hp_after_damage(100, 100) == 0);
    CHECK(angdroid_fa_mon_hp_after_damage(100, 101) == -1);
    CHECK(angdroid_fa_mon_hp_after_damage(100, 100000) == INT16_MIN);
    CHECK(angdroid_fa_mon_hp_after_damage(100, INT_MAX) == INT16_MIN);
    CHECK(angdroid_fa_mon_hp_after_damage(-10, INT_MAX) == INT16_MIN);
    CHECK(angdroid_fa_mon_hp_after_damage(100, INT_MIN) == INT16_MAX);
    CHECK(angdroid_fa_mon_hp_after_damage(INT16_MAX, -1) == INT16_MAX);
    CHECK(angdroid_fa_mon_hp_after_damage(INT16_MIN, 1) == INT16_MIN);
    CHECK(angdroid_fa_mon_hp_after_damage(100, -30) == 130);
    CHECK(angdroid_fa_mon_hp_after_damage(INT16_MIN, -1) == INT16_MIN + 1);

    /* Both the player and non-player hit functions use this helper.
     * A unique monster's non-player damage is capped to its existing HP:
     * a 100-HP unique at exactly 100 damage must remain at zero. */
    CHECK(angdroid_fa_mon_hp_after_damage(100, 100) == 0);

    /* Cross-check the full 16-bit HP range with extreme and normal damage. */
    for (hp = INT16_MIN; hp <= INT16_MAX; ++hp) {
        for (j = 0; j < sizeof damages / sizeof damages[0]; ++j) {
            int64_t raw = (int64_t)hp - (int64_t)damages[j];
            int16_t expected = raw < INT16_MIN ? INT16_MIN :
                (raw > INT16_MAX ? INT16_MAX : (int16_t)raw);
            int16_t actual = angdroid_fa_mon_hp_after_damage(
                (int16_t)hp, damages[j]);
            CHECK(actual == expected);
            CHECK((actual < 0) == (expected < 0));
        }
    }

    /* Batch 266: healing cannot wrap around before the maxhp clamp. */
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, 30) == 130);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, 100) == 200);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, 100000) == 200);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, INT64_MAX) == 200);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, INT64_MIN) == 100);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, -50) == 100);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 200, 0) == 100);
    CHECK(angdroid_fa_mon_hp_after_healing(100, 0, 50) == 100);
    CHECK(angdroid_fa_mon_hp_after_healing(100, -1, 50) == 100);
    CHECK(angdroid_fa_mon_hp_after_healing(INT16_MAX, INT16_MAX,
        INT64_MAX) == INT16_MAX);
    CHECK(angdroid_fa_mon_hp_after_healing(INT16_MIN, INT16_MAX,
        INT64_MAX) == INT16_MAX);
    CHECK(angdroid_fa_mon_hp_after_healing(INT16_MIN, INT16_MAX, 1)
        == INT16_MIN + 1);
    CHECK(angdroid_fa_mon_hp_after_healing(300, 200, 1) == 200);
    {
        static const int64_t amounts[] = {
            INT64_MIN, -100000, -1, 0, 1, 100, 32768,
            100000, (int64_t)INT_MAX * 6, INT64_MAX
        };
        int maxhp;
        size_t k;
        for (hp = INT16_MIN; hp <= INT16_MAX; hp += 131) {
            for (maxhp = 1; maxhp <= INT16_MAX; maxhp += 173) {
                for (k = 0; k < sizeof amounts / sizeof amounts[0]; ++k) {
                    int64_t heal = amounts[k];
                    int16_t expect = heal <= 0 ? (int16_t)hp :
                        (heal >= (int64_t)maxhp - hp ? (int16_t)maxhp :
                        (int16_t)((int64_t)hp + heal));
                    CHECK(angdroid_fa_mon_hp_after_healing(
                        (int16_t)hp, (int16_t)maxhp, heal) == expect);
                }
            }
        }
    }

    printf("PASS: %u FAangband monster HP damage checks\n", checks);
    return 0;
}
