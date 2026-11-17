/*
 * FAangband monster recall: display-only Traditional Chinese clause joining.
 * The caller still chooses the visible monster flags with the original masks.
 * Ordinal is the zero-based index of an already-approved visible flag.
 */
#ifndef ANGDROID_FA_MON_LORE_DISPLAY_H
#define ANGDROID_FA_MON_LORE_DISPLAY_H

#include <stdbool.h>

static inline const char *angdroid_fa_lore_separator(int ordinal,
    int total, bool alternatives)
{
    if (ordinal <= 0 || total <= 1) return "";
    if (ordinal == total - 1) return alternatives ? "或" : "與";
    return "、";
}

static inline const char *angdroid_fa_lore_sentence_end(
    const char *english, bool zh_tw)
{
    if (zh_tw && english && english[0] == '.' && english[1] == ' ' &&
        english[2] == ' ' && english[3] == '\0')
        return "。  ";
    return english;
}

#endif /* ANGDROID_FA_MON_LORE_DISPLAY_H */
