/* Display-only Traditional Chinese rune titles and lore for Angband.
 * Use the existing canonical rune name/description and exact object-term
 * translations. Unknown properties and curses retain the original English.
 */
#ifndef ANGDROID_ZH_TW_RUNE_LORE_H
#define ANGDROID_ZH_TW_RUNE_LORE_H

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "zh-tw-object-info.h"

/* Extract one canonical property term between exact prefix/suffix markers.
 * Do not translate an unrecognized term, nor truncate a longer source term.
 * Lowercase ASCII property names to match curated translation keys.
 */
static inline const char *angdroid_rune_mapped_term(
    const char *english, const char *prefix, const char *suffix,
    char *term, size_t capacity)
{
    size_t n, a, b, length, i;
    const char *mapped;
    if (!english || !prefix || !suffix || !term || !capacity) return NULL;
    n = strlen(english);
    a = strlen(prefix);
    b = strlen(suffix);
    if (n <= a + b || n - a - b >= capacity ||
        strncmp(english, prefix, a) != 0 ||
        strcmp(english + n - b, suffix) != 0) return NULL;
    length = n - a - b;
    for (i = 0; i < length; ++i)
        term[i] = (char)tolower((unsigned char)english[a + i]);
    term[length] = '\0';
    mapped = angdroid_zh_tw_object_term(term, 1);
    return strcmp(mapped, term) == 0 ? NULL : mapped;
}

/* Names are solely for the knowledge browser, not the rune IDs or
 * messages that might depend on canonical formatting. */
static inline const char *angdroid_zh_tw_rune_title(
    const char *english, int zh_tw, char *out, size_t capacity)
{
    char term[128];
    const char *mapped;
    int len;
    if (!english || !zh_tw || !out || !capacity) return english;

    if (strcmp(english, "enchantment to armor") == 0) return "護甲強化";
    if (strcmp(english, "enchantment to hit") == 0) return "命中強化";
    if (strcmp(english, "enchantment to damage") == 0) return "傷害強化";

    mapped = angdroid_rune_mapped_term(english, "resist ", "",
                                       term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "%s抗性", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english, "slay ", "",
                                       term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "剋制%s", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english, "", " brand",
                                       term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "%s附魔", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english, "", "",
                                       term, sizeof(term));
    return mapped ? mapped : english;
}

static inline const char *angdroid_zh_tw_rune_description(
    const char *english, int zh_tw, char *out, size_t capacity)
{
    char term[128];
    const char *mapped;
    int len;
    if (!english || !zh_tw || !out || !capacity) return english;

    if (strcmp(english,
        "Object magically increases the player's armor class") == 0)
        return "物品以魔法提高玩家的護甲值。";
    if (strcmp(english,
        "Object magically increases the player's chance to hit") == 0)
        return "物品以魔法提高玩家的命中率。";
    if (strcmp(english,
        "Object magically increases the player's damage") == 0)
        return "物品以魔法提高玩家的傷害。";

    mapped = angdroid_rune_mapped_term(english,
        "Object gives the player a magical bonus to ", ".",
        term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "物品讓玩家的%s獲得魔法加成。", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english,
        "Object affects the player's resistance to ", ".",
        term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "物品會影響玩家對%s的抗性。", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english,
        "Object brands the player's attacks with ", ".",
        term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "物品可為玩家的攻擊附加%s屬性。", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english,
        "Object makes the player's attacks against ", " more powerful.",
        term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "物品會提高玩家對%s的攻擊威力。", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    mapped = angdroid_rune_mapped_term(english,
        "Object gives the player the property of ", ".",
        term, sizeof(term));
    if (mapped) {
        len = snprintf(out, capacity, "物品賦予玩家%s特性。", mapped);
        return len >= 0 && (size_t)len < capacity ? out : english;
    }
    return english;
}

#endif /* ANGDROID_ZH_TW_RUNE_LORE_H */
