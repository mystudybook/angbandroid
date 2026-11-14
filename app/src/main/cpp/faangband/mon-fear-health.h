/* FAangband monster fear calculation: preserve the original percentage for
 * valid monsters, but reject an invalid maximum HP before dividing.
 * This does not modify actual HP, monster status or random rolls.
 */
#ifndef ANGDROID_FA_MON_FEAR_HEALTH_H
#define ANGDROID_FA_MON_FEAR_HEALTH_H

#include <stdbool.h>
#include <stdint.h>

static inline bool angdroid_fa_mon_fear_health_percentage(int16_t current_hp,
    int16_t max_hp, int *percentage)
{
    if (!percentage || max_hp <= 0) return false;
    /* An int16_t HP times 100 safely fits in a 32-bit long (Android). */
    *percentage = (int)((100L * current_hp) / max_hp);
    return true;
}

#endif /* ANGDROID_FA_MON_FEAR_HEALTH_H */
