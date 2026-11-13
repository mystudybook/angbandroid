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

    used = angdroid_utf8_copy_complete(output, capacity, ghost_name);
    used = angdroid_utf8_append_complete(output, capacity, used, "（");
    used = angdroid_utf8_append_complete(output, capacity, used, race_name);
    used = angdroid_utf8_append_complete(output, capacity, used, "）");
    if (*tail)
        (void)angdroid_utf8_append_complete(
            output, capacity, used, "（畫面外）");
    return output;
}

#endif /* ANGDROID_FA_SPELL_MESSAGE_LOCALE_H */
