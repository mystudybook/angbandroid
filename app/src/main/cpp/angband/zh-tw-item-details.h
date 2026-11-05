/* Bounded display-only Chinese details for Angband item descriptions.
 * Identification, item identities, save data and combat calculations stay native.
 */
#ifndef ANGDROID_ZH_TW_ITEM_DETAILS_H
#define ANGDROID_ZH_TW_ITEM_DETAILS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "zh-tw-item-grammar.h"

enum angdroid_zh_tw_combat_field {
    ANGDROID_ITEM_DICE, ANGDROID_ITEM_MULTIPLIER,
    ANGDROID_ITEM_ATTACK_PAIR, ANGDROID_ITEM_HIT, ANGDROID_ITEM_DAMAGE,
    ANGDROID_ITEM_ARMOR_PAIR, ANGDROID_ITEM_ARMOR, ANGDROID_ITEM_ARMOR_BONUS
};

/* Append only if every UTF-8 codepoint and trailing punctuation fits.
 * Failed appends roll back the end offset and restore the original string.
 */
static inline bool angdroid_zh_tw_item_append_whole(
    char *out, size_t cap, size_t *end, const char *text)
{
    size_t old, wanted, written;
    if (!out || !cap || !end || !text || *end >= cap) return false;
    old = *end;
    wanted = strlen(text);
    if (wanted >= cap - old) return false;
    written = angdroid_zh_tw_item_append_utf8(out, cap, end, text);
    if (written == wanted) return true;
    *end = old;
    out[old] = '\0';
    return false;
}

static inline bool angdroid_zh_tw_item_combat_text(
    char *out, size_t cap, enum angdroid_zh_tw_combat_field type,
    int first, int second)
{
    int n = -1;
    if (!out || !cap) return false;
    out[0] = '\0';
    switch (type) {
    case ANGDROID_ITEM_DICE:
        n = snprintf(out, cap, "（傷害 %dd%d）", first, second); break;
    case ANGDROID_ITEM_MULTIPLIER:
        n = snprintf(out, cap, "（倍率 ×%d）", first); break;
    case ANGDROID_ITEM_ATTACK_PAIR:
        n = snprintf(out, cap, "（命中 %+d／傷害 %+d）", first, second); break;
    case ANGDROID_ITEM_HIT:
        n = snprintf(out, cap, "（命中 %+d）", first); break;
    case ANGDROID_ITEM_DAMAGE:
        n = snprintf(out, cap, "（傷害 %+d）", first); break;
    case ANGDROID_ITEM_ARMOR_PAIR:
        n = snprintf(out, cap, "〔護甲 %d／防護 %+d〕", first, second); break;
    case ANGDROID_ITEM_ARMOR:
        n = snprintf(out, cap, "〔護甲 %d〕", first); break;
    case ANGDROID_ITEM_ARMOR_BONUS:
        n = snprintf(out, cap, "〔防護 %+d〕", first); break;
    default: return false;
    }
    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}

/* Receives the already known nonzero distinct modifier VALUES in original
 * discovery order; does not read the object or disclose hidden modifiers. */
static inline bool angdroid_zh_tw_item_modifiers_text(
    char *out, size_t cap, const int *values, size_t count)
{
    size_t i, used = 0;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (!values || !count) return false;
    for (i = 0; i < count; ++i) {
        int n = snprintf(out + used, cap - used,
                         "%s%+d", i ? "、" : "〈", values[i]);
        if (n < 0 || (size_t)n >= cap - used) {
            out[0] = '\0';
            return false;
        }
        used += (size_t)n;
    }
    if (strlen("〉") >= cap - used) {
        out[0] = '\0';
        return false;
    }
    memcpy(out + used, "〉", strlen("〉") + 1);
    return true;
}

/* Preserve original values while appending a complete localized status. */
static inline bool angdroid_zh_tw_item_append_amount(
    char *out, size_t cap, size_t *end,
    const char *before, int amount, const char *after)
{
    char text[96];
    int n;
    if (!before || !after) return false;
    n = snprintf(text, sizeof(text), "%s%d%s", before, amount, after);
    if (n < 0 || (size_t)n >= sizeof(text)) return false;
    return angdroid_zh_tw_item_append_whole(out, cap, end, text);
}
#endif
