/*
 * FAangband monster-message display helpers.
 * Build Chinese subjects from display-only source fields: never alter
 * monster races, visibility flags, or the message-stacking keys.
 */
#ifndef ANGDROID_FA_MON_MSG_UTF8_H
#define ANGDROID_FA_MON_MSG_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../curses/unicode_display.h"

/*
 * Reserve room for the entire off-panel marker before appending a potentially
 * long race, plural, or player-ghost name. Only whole UTF-8 scalars are copied.
 * The caller has already established whether the source is visible.
 */
static inline void angdroid_fa_mon_msg_chinese_subject(
    char *dst, size_t capacity, const char *race_name,
    const char *plural, const char *player_race_name,
    const char *ghost_name, int count, bool invisible,
    bool offscreen, bool unique, bool comma)
{
    static const char suffix[] = "（畫面外）";
    char number[64];
    size_t base_capacity, used = 0;
    bool suffix_fits;

    if (!dst || !capacity) return;
    dst[0] = '\0';
    suffix_fits = offscreen && capacity > sizeof(suffix);
    base_capacity = suffix_fits ? capacity - (sizeof(suffix) - 1) :
        capacity;

    if (invisible) {
        if (count == 1) {
            used = angdroid_utf8_copy_complete(dst, base_capacity, "牠");
        } else {
            (void)snprintf(number, sizeof(number), "%d 隻怪物", count);
            used = angdroid_utf8_copy_complete(dst, base_capacity, number);
        }
    } else if (ghost_name) {
        /* Either display the complete "(race)" identity, or only a
         * complete-name prefix: never leave an unmatched Han bracket. */
        size_t name_bytes = strlen(ghost_name);
        size_t race_bytes = race_name ? strlen(race_name) : 0;
        const size_t brackets = (sizeof("（") - 1) +
            (sizeof("）") - 1);
        used = angdroid_utf8_copy_complete(dst, base_capacity, ghost_name);
        if (race_name && name_bytes < base_capacity &&
            base_capacity - name_bytes - 1 >= brackets &&
            race_bytes <= base_capacity - name_bytes - 1 - brackets) {
            used = angdroid_utf8_append_complete(
                dst, base_capacity, used, "（");
            used = angdroid_utf8_append_complete(
                dst, base_capacity, used, race_name);
            used = angdroid_utf8_append_complete(
                dst, base_capacity, used, "）");
        }
    } else if (unique) {
        used = angdroid_utf8_copy_complete(dst, base_capacity, race_name);
    } else {
        if (count == 1) {
            used = angdroid_utf8_copy_complete(
                dst, base_capacity, "這隻");
        } else {
            (void)snprintf(number, sizeof(number), "%d 隻", count);
            used = angdroid_utf8_copy_complete(
                dst, base_capacity, number);
        }
        if (player_race_name && player_race_name[0]) {
            used = angdroid_utf8_append_complete(
                dst, base_capacity, used, player_race_name);
            used = angdroid_utf8_append_complete(
                dst, base_capacity, used, " ");
        }
        used = angdroid_utf8_append_complete(dst, base_capacity, used,
            count == 1 || !plural ? race_name : plural);
    }

    if (!invisible && comma)
        used = angdroid_utf8_append_complete(
            dst, base_capacity, used, "，");
    if (suffix_fits)
        (void)angdroid_utf8_append_complete(dst, capacity, used, suffix);
}

#endif /* ANGDROID_FA_MON_MSG_UTF8_H */
