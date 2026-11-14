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

/*
 * Validate an entire stored display name before using it as an atomic
 * component. The UTF-8 copier replaces invalid bytes with '?', but this
 * could turn a known unique name into a misleading partial identity.
 * Prefer the existing anonymous fallback instead. A correctly encoded
 * U+FFFD replacement scalar (three bytes) is still valid UTF-8.
 */
static inline bool angdroid_fa_mon_msg_valid_utf8(const char *name)
{
    size_t offset = 0;

    if (!name) return false;
    while (name[offset]) {
        uint32_t scalar;
        size_t len = angdroid_utf8_next_scalar(name + offset, &scalar);
        if (!len || (len == 1 && scalar == 0xFFFDu &&
                (unsigned char)name[offset] >= 0x80u))
            return false;
        offset += len;
    }
    return true;
}

/* A display component is atomic: never print half a monster species, a
 * malformed plural count, or an incomplete saved player-ghost name. */
static inline bool angdroid_fa_mon_msg_can_append(
    size_t capacity, size_t used, const char *component)
{
    return component && used < capacity &&
        strlen(component) <= capacity - used - 1 &&
        angdroid_fa_mon_msg_valid_utf8(component);
}

static inline size_t angdroid_fa_mon_msg_append(
    char *dst, size_t capacity, size_t used, const char *component)
{
    if (!angdroid_fa_mon_msg_can_append(capacity, used, component))
        return used;
    return angdroid_utf8_append_complete(dst, capacity, used, component);
}

/* A complete fallback keeps every message grammatically identifiable even
 * when full unique, ghost, plural and player-race names cannot be shown. */
static inline size_t angdroid_fa_mon_msg_generic(
    char *dst, size_t capacity, bool plural)
{
    const char *fallback = plural ? "怪物" : "牠";
    if (angdroid_fa_mon_msg_can_append(capacity, 0, fallback))
        return angdroid_fa_mon_msg_append(dst, capacity, 0, fallback);
    if (plural && angdroid_fa_mon_msg_can_append(capacity, 0, "牠"))
        return angdroid_fa_mon_msg_append(dst, capacity, 0, "牠");
    return 0;
}

/*
 * Render the existing rounded-up positive average for damage reports.
 * count/2 + count%2 is equivalent to (count+1)/2 for valid counts,
 * without overflowing when a very large stacked count reaches INT_MAX.
 * Grouped damage values and combat calculations are not changed.
 */
static inline int angdroid_fa_mon_msg_average_damage(int total_damage,
    int count)
{
    if (count <= 0) return 0;
    return total_damage / count +
        (total_damage % count >= count / 2 + count % 2 ? 1 : 0);
}

/* Only format localized display; never alter monster race information,
 * visibility rules, message stack keys, or damage/kill calculations. */
static inline void angdroid_fa_mon_msg_chinese_subject(
    char *dst, size_t capacity, const char *race_name,
    const char *plural, const char *player_race_name,
    const char *ghost_name, int count, bool invisible,
    bool offscreen, bool unique, bool comma)
{
    static const char suffix[] = "（畫面外）";
    char number[64];
    size_t base_capacity, used = 0, reserved_bytes = 0;
    bool suffix_fits, comma_fits;

    if (!dst || !capacity) return;
    dst[0] = '\0';

    /* The full offscreen marker and comma follow the noun phrase.
     * Reserve them only when there is room for a complete minimal
     * subject ("牠"), the entire marker(s), and the terminating NUL.
     * That prevents a dangling "（畫面外）" or a comma-only sentence. */
    suffix_fits = offscreen &&
        capacity > (sizeof(suffix) - 1) + (sizeof("牠") - 1);
    if (suffix_fits)
        reserved_bytes = sizeof(suffix) - 1;
    comma_fits = !invisible && comma &&
        capacity > reserved_bytes + (sizeof("，") - 1) +
            (sizeof("牠") - 1);
    if (comma_fits)
        reserved_bytes += sizeof("，") - 1;
    base_capacity = capacity - reserved_bytes;

    if (invisible) {
        if (count <= 1) {
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, used, "牠");
        } else {
            (void)snprintf(number, sizeof(number), "%d 隻怪物", count);
            if (angdroid_fa_mon_msg_can_append(
                    base_capacity, 0, number)) {
                used = angdroid_fa_mon_msg_append(
                    dst, base_capacity, 0, number);
            } else {
                used = angdroid_fa_mon_msg_generic(
                    dst, base_capacity, true);
            }
        }
    } else if (ghost_name) {
        /* The ghost's full personal name comes first; the optional
         * parenthesized species is inserted only if it fits as a unit. */
        if (ghost_name[0] &&
            angdroid_fa_mon_msg_can_append(base_capacity, 0, ghost_name)) {
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, 0, ghost_name);
            if (race_name && race_name[0]) {
                size_t opening = sizeof("（") - 1;
                size_t closing = sizeof("）") - 1;
                size_t race_bytes = strlen(race_name);
                size_t remaining = base_capacity - used - 1;
                if (opening <= remaining &&
                    closing <= remaining - opening &&
                    race_bytes <= remaining - opening - closing) {
                    used = angdroid_fa_mon_msg_append(
                        dst, base_capacity, used, "（");
                    used = angdroid_fa_mon_msg_append(
                        dst, base_capacity, used, race_name);
                    used = angdroid_fa_mon_msg_append(
                        dst, base_capacity, used, "）");
                }
            }
        } else {
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, 0, "幽靈");
            if (!used) used = angdroid_fa_mon_msg_generic(
                dst, base_capacity, false);
        }
    } else if (unique) {
        if (race_name && race_name[0] &&
            angdroid_fa_mon_msg_can_append(base_capacity, 0, race_name))
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, 0, race_name);
        else
            used = angdroid_fa_mon_msg_generic(
                dst, base_capacity, false);
    } else {
        const char *visible_race =
            count <= 1 || !plural || !plural[0] ? race_name : plural;
        if (count <= 1) {
            (void)snprintf(number, sizeof(number), "%s", "這隻");
        } else {
            (void)snprintf(number, sizeof(number), "%d 隻", count);
        }

        /* Keep the full prefix and actual monster species together.
         * A player-race qualifier is optional and is checked afterward. */
        if (visible_race && visible_race[0] &&
            angdroid_fa_mon_msg_can_append(base_capacity, 0, number) &&
            angdroid_fa_mon_msg_can_append(
                base_capacity, strlen(number), visible_race)) {
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, 0, number);
            if (player_race_name && player_race_name[0]) {
                size_t qualifier_len = strlen(player_race_name);
                size_t race_len = strlen(visible_race);
                size_t remaining = base_capacity - used - 1;
                if (qualifier_len < remaining &&
                    race_len <= remaining - qualifier_len - 1) {
                    used = angdroid_fa_mon_msg_append(
                        dst, base_capacity, used, player_race_name);
                    used = angdroid_fa_mon_msg_append(
                        dst, base_capacity, used, " ");
                }
            }
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, used, visible_race);
        } else if (visible_race && visible_race[0] &&
                   angdroid_fa_mon_msg_can_append(
                       base_capacity, 0, visible_race)) {
            /* Keep a complete species rather than "這隻" alone. */
            used = angdroid_fa_mon_msg_append(
                dst, base_capacity, 0, visible_race);
        } else {
            /* A truncated race name or number is not a useful subject. */
            if (angdroid_fa_mon_msg_can_append(
                    base_capacity, 0, "這隻怪物") && count <= 1)
                used = angdroid_fa_mon_msg_append(
                    dst, base_capacity, 0, "這隻怪物");
            else
                used = angdroid_fa_mon_msg_generic(
                    dst, base_capacity, count > 1);
        }
    }

    /* These never appear by themselves; under very small byte budgets
     * the source-identifying noun is more important than decoration. */
    if (used != 0 && suffix_fits)
        used = angdroid_fa_mon_msg_append(dst, capacity, used, suffix);
    if (used != 0 && comma_fits)
        (void)angdroid_fa_mon_msg_append(dst, capacity, used, "，");
}

#endif /* ANGDROID_FA_MON_MSG_UTF8_H */
