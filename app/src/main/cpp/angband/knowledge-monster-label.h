/* Localized labels for the knowledge-browser monster column.
 * Names and uniqueness belong to the unchanged canonical monster_race.
 * A bounded cell copy keeps wide CJK glyphs out of the symbol column.
 */
#ifndef ANGDROID_KNOWLEDGE_MONSTER_LABEL_H
#define ANGDROID_KNOWLEDGE_MONSTER_LABEL_H

#include <stddef.h>
#include "zh-tw-monsters.h"
#include "zh-tw-unique-monsters.h"
#include "menu-text-fit.h"

static inline const char *angdroid_knowledge_monster_label(
    const char *english, int is_unique, int zh_tw)
{
    const char *translated;
    if (!english || !zh_tw) return english;
    translated = is_unique
        ? angdroid_zh_tw_unique_monster_name(english)
        : angdroid_zh_tw_monster_list_name(english);
    return translated ? translated : english;
}

static inline size_t angdroid_knowledge_monster_copy(
    char *dest, size_t capacity, const char *english,
    int is_unique, int zh_tw, int available_columns)
{
    const char *label = angdroid_knowledge_monster_label(
        english, is_unique, zh_tw);
    if (available_columns <= 0)
        return angdroid_menu_copy_columns(dest, capacity, label, 0);
    return angdroid_menu_copy_columns(dest, capacity, label,
                                      (size_t)available_columns);
}

#endif /* ANGDROID_KNOWLEDGE_MONSTER_LABEL_H */
