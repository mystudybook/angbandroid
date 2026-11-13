/*
 * FAangband monster-spell battle messages are templates, not game data.
 * Keep translations source-exact; preserve unknown race-specific alternate
 * messages as English, including their original placeholders and grammar.
 */
#ifndef ANGDROID_FA_SPELL_MESSAGE_LOCALE_H
#define ANGDROID_FA_SPELL_MESSAGE_LOCALE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "mon-spell-messages-visible-zh-tw.h"
#include "mon-spell-messages-other-zh-tw.h"
#include "mon-spell-messages-alternate-zh-tw.h"
#include "../curses/unicode_display.h"

static inline const char *angdroid_fa_spell_message_locale(
    const char *english, bool zh_tw)
{
    const char *translated;
    if (!english || !zh_tw) return english;
    translated = angdroid_fa_spell_message_visible(english, true);
    if (translated != english) return translated;
    translated = angdroid_fa_spell_message_other(english, true);
    if (translated != english) return translated;
    return angdroid_fa_spell_message_alternate(english, true);
}

/*
 * The engine has already selected the monster's real visible/hidden name;
 * only replace English articles and hidden pronouns for a translated message.
 * Untranslated alternates must retain their English subject.
 *
 * This helper should be used only for ordinary (non-unique, non-ghost)
 * monsters. The caller keeps true unique and named ghosts unchanged.
 */
static inline const char *angdroid_fa_spell_subject(
    const char *english, bool localized, char *output, size_t capacity)
{
    const char *noun = NULL;
    if (!english || !localized || !output || !capacity)
        return english;
    if (strncmp(english, "the ", 4) == 0 ||
        strncmp(english, "The ", 4) == 0) {
        noun = english + 4;
        size_t used = angdroid_utf8_copy_complete(output, capacity, "這隻");
        (void)angdroid_utf8_append_complete(output, capacity, used, noun);
        return output;
    }
    if (strcmp(english, "it") == 0 || strcmp(english, "It") == 0) {
        (void)angdroid_utf8_copy_complete(output, capacity, "牠");
        return output;
    }
    if (strcmp(english, "something") == 0 ||
        strcmp(english, "Something") == 0) {
        (void)angdroid_utf8_copy_complete(output, capacity, "某個東西");
        return output;
    }
    return english;
}


/*
 * FAangband's monster_desc() formats player ghosts as:
 *   "<saved player name>, the <race>" [" (offscreen)"]
 * Retain the saved name and the true race, changing only the English joining
 * grammar. Only accept exact source-shaped phrases so a custom ghost name
 * containing commas cannot be rewritten accidentally.
 */
static inline const char *angdroid_fa_spell_ghost_subject(
    const char *full, const char *ghost_name, const char *race_name,
    bool zh_tw, char *output, size_t capacity)
{
    const char *tail;
    size_t name_len, race_len, used;

    if (!full || !ghost_name || !race_name || !zh_tw ||
        !output || !capacity) return full;

    name_len = strlen(ghost_name);
    race_len = strlen(race_name);
    if (strncmp(full, ghost_name, name_len) != 0) return full;
    tail = full + name_len;
    if (strncmp(tail, ", the ", 6) != 0) return full;
    tail += 6;
    if (strncmp(tail, race_name, race_len) != 0) return full;
    tail += race_len;
    if (*tail && strcmp(tail, " (offscreen)") != 0) return full;

    /*
     * Do not leave a dangling full-width bracket in a narrow buffer.
     * When the full identity suffix does not fit, show as much of the
     * original ghost name as will fit rather than a partial description.
     */
    {
        size_t suffix = (sizeof("（") - 1) + race_len +
            (sizeof("）") - 1);
        if (*tail) suffix += sizeof("（畫面外）") - 1;
        if (name_len >= capacity || suffix > capacity - name_len - 1) {
            (void)angdroid_utf8_copy_complete(output, capacity, ghost_name);
            return output;
        }
    }

    used = angdroid_utf8_copy_complete(output, capacity, ghost_name);
    used = angdroid_utf8_append_complete(output, capacity, used, "（");
    used = angdroid_utf8_append_complete(output, capacity, used, race_name);
    used = angdroid_utf8_append_complete(output, capacity, used, "）");
    if (*tail)
        (void)angdroid_utf8_append_complete(
            output, capacity, used, "（畫面外）");
    return output;
}


/*
 * monster_desc() appends " (offscreen)" to an otherwise visible monster
 * name when it is outside the panel.  On an already-localized battle
 * sentence, preserve that fact with a Chinese suffix for ordinary and
 * shape-unique monsters alike. The ghost helper already handles its own
 * offscreen suffix.
 *
 * Even under a tiny byte budget, only complete UTF-8 scalars are copied.
 * Prefer a shortened name plus a complete location indicator over a
 * clipped "(offscreen)" or a dangling Chinese bracket.
 */
static inline const char *angdroid_fa_spell_offscreen_subject(
    const char *display_name, bool zh_tw, char *output, size_t capacity)
{
    static const char suffix[] = " (offscreen)";
    static const char localized_suffix[] = "（畫面外）";
    size_t full_len, base_len, prefix_capacity, used;

    if (!display_name || !zh_tw || !output || !capacity)
        return display_name;

    full_len = strlen(display_name);
    if (full_len < sizeof(suffix) - 1 ||
        strcmp(display_name + full_len - (sizeof(suffix) - 1), suffix) != 0)
        return display_name;

    /* A whole suffix and NUL must fit, or keep the source unchanged. */
    if (capacity <= sizeof(localized_suffix))
        return display_name;

    base_len = full_len - (sizeof(suffix) - 1);
    prefix_capacity = capacity - (sizeof(localized_suffix) - 1);
    if (base_len < prefix_capacity)
        prefix_capacity = base_len + 1;

    used = angdroid_utf8_copy_complete(output, prefix_capacity, display_name);
    (void)angdroid_utf8_append_complete(
        output, capacity, used, localized_suffix);
    return output;
}

#endif /* ANGDROID_FA_SPELL_MESSAGE_LOCALE_H */
