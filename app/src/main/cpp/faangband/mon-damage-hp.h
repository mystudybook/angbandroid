/* Keep FAangband monster HP within its int16_t storage range after damage.
 * Calculate in int64_t so large positive or negative int damage cannot wrap
 * in int arithmetic or change a lethal hit into a positive stored HP value.
 * Normal in-range results, exact-zero survival and existing callers' damage
 * classification are unchanged.
 */
#ifndef ANGDROID_FA_MON_DAMAGE_HP_H
#define ANGDROID_FA_MON_DAMAGE_HP_H

#include <stdint.h>

static inline int16_t angdroid_fa_mon_hp_after_damage(int16_t hp, int damage)
{
    int64_t remaining = (int64_t)hp - (int64_t)damage;
    if (remaining < INT16_MIN) return INT16_MIN;
    if (remaining > INT16_MAX) return INT16_MAX;
    return (int16_t)remaining;
}

#endif /* ANGDROID_FA_MON_DAMAGE_HP_H */
