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

/* Treat this operation as healing only.  Reject zero/negative healing
 * and invalid maximum HP rather than allowing a healing event to damage the
 * monster.  Compare against remaining capacity before adding, so even
 * INT64_MAX healing does not overflow or pass through an int16_t wrap. */
static inline int16_t angdroid_fa_mon_hp_after_healing(int16_t hp,
    int16_t maxhp, int64_t amount)
{
    if (amount <= 0 || maxhp <= 0) return hp;
    if (amount >= (int64_t)maxhp - (int64_t)hp) return maxhp;
    return (int16_t)((int64_t)hp + amount);
}

/* Clamp generated int HP before narrowing to int16_t.  Ordinary rolls
 * remain unchanged, including the unique-monster fixed HP path. */
static inline int16_t angdroid_fa_mon_maxhp_from_roll(int rolled_hp)
{
    if (rolled_hp < 1) return 1;
    if (rolled_hp > INT16_MAX) return INT16_MAX;
    return (int16_t)rolled_hp;
}

#endif /* ANGDROID_FA_MON_DAMAGE_HP_H */
