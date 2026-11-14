/* Display-only chest status and money descriptions with complete UTF-8. */
#ifndef ANGDROID_OBJ_DESC_CHEST_MONEY_UTF8_H
#define ANGDROID_OBJ_DESC_CHEST_MONEY_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "obj-desc-effect-utf8.h"

/* Exact source labels from the Angband and FAangband chest_trap.txt data.
 * Unknown labels are preserved verbatim; never infer or reveal a trap. */
static inline const char *angdroid_zh_tw_chest_status_name(const char *english)
{
    static const struct { const char *en, *zh; } names[] = {
        { "locked", "已上鎖" },
        { "unlocked", "已解鎖" },
        { "disarmed", "已解除陷阱" },
        { "empty", "空箱" },
        { "multiple traps", "多重陷阱" },
        { "gas trap", "氣體陷阱" },
        { "poison needle", "毒針" },
        { "summoning runes", "召喚符文" },
        { "explosion device", "爆炸裝置" },
        { "a strange rune", "奇異符文" },
        { "gleaming black runes", "閃亮的黑色符文" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        if (strcmp(english, names[i].en) == 0) return names[i].zh;
    return english;
}

/* Atomic append: full label plus both parentheses, or no change. */
static inline bool angdroid_obj_desc_append_chest_status(char *out,
    size_t cap, size_t *end, const char *source, bool zh_tw)
{
    const char *name = zh_tw ? angdroid_zh_tw_chest_status_name(source) : source;
    return angdroid_obj_desc_append_effect_wrapped(out, cap, end,
        zh_tw ? "（" : " (", name, zh_tw ? "）" : ")");
}

/* Exact gold-kind labels from Angband object.txt.  Do not change their
 * source strings or their canonical object identity. */
static inline const char *angdroid_zh_tw_money_kind_name(const char *english)
{
    static const struct { const char *en, *zh; } names[] = {
        { "copper", "銅幣" },
        { "silver", "銀幣" },
        { "garnets", "石榴石" },
        { "gold", "金幣" },
        { "opals", "蛋白石" },
        { "sapphires", "藍寶石" },
        { "rubies", "紅寶石" },
        { "diamonds", "鑽石" },
        { "emeralds", "祖母綠" },
        { "mithril", "秘銀" },
        { "adamantite", "精金" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        if (strcmp(english, names[i].en) == 0) return names[i].zh;
    return english;
}

/* The original English output is unchanged when zh_tw is false.
 * The returned byte count is the number of complete UTF-8 bytes written.
 * If the entire description cannot fit, return empty instead of clipping
 * the money kind or a closing {ignore} annotation. */
static inline size_t angdroid_obj_desc_money_text(char *out, size_t cap,
    int amount, const char *kind, bool ignored, bool zh_tw)
{
    char prefix[80];
    const char *label;
    const char *annotation = ignored ?
        (zh_tw ? " {忽略}" : " {ignore}") : "";
    size_t used = 0;
    int n;

    if (!out || !cap) return 0;
    out[0] = '\0';
    if (!kind || !*kind) return 0;
    n = snprintf(prefix, sizeof(prefix),
        zh_tw ? "價值 %d 金幣的" : "%d gold pieces worth of ", amount);
    if (n < 0 || (size_t)n >= sizeof(prefix)) return 0;
    label = zh_tw ? angdroid_zh_tw_money_kind_name(kind) : kind;
    if (!angdroid_obj_desc_append_effect_wrapped(out, cap, &used,
            prefix, label, annotation)) return 0;
    return used;
}

#endif /* ANGDROID_OBJ_DESC_CHEST_MONEY_UTF8_H */
