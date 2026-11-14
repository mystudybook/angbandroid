/* Defensive monster pain percentage for display-only messages.
 * Current HP and reported damage are both integer values, but 'long' is only
 * 32-bit on 32-bit Android; restore previous HP using int64_t instead.
 */
#ifndef ANGDROID_FA_MON_MSG_PAIN_H
#define ANGDROID_FA_MON_MSG_PAIN_H

#include <stdint.h>

static inline int angdroid_fa_mon_msg_pain_percentage(int remaining_hp,
    int damage)
{
    int64_t previous_hp;

    /* Nonpositive HP is already the strongest pain grade.  This also
     * avoids a zero divisor for malformed/delayed death notifications. */
    if (damage <= 0 || remaining_hp <= 0) return 0;

    previous_hp = (int64_t)remaining_hp + (int64_t)damage;
    /* Both values are positive, so previous_hp is positive as well. */
    return (int)(((int64_t)remaining_hp * 100) / previous_hp);
}

#endif /* ANGDROID_FA_MON_MSG_PAIN_H */
