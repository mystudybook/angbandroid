/* Display-only Traditional Chinese noun phrases for flavored Angband objects.
 * Caller determines identification state. Never mutate canonical game data.
 */
#ifndef ANGDROID_ZH_TW_FLAVORED_OBJECT_H
#define ANGDROID_ZH_TW_FLAVORED_OBJECT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static inline bool angdroid_zh_tw_flavored_object(char *out, size_t capacity,
    const char *tval, const char *appearance, const char *known_effect,
    bool prefix, unsigned int count)
{
    static const struct { const char *tval, *noun, *classifier; } types[] = {
        { "ring", "戒指", "枚" },
        { "amulet", "護身符", "枚" },
        { "staff", "法杖", "根" },
        { "wand", "魔杖", "根" },
        { "rod", "魔棒", "根" },
        { "potion", "藥水", "瓶" },
        { "mushroom", "蘑菇", "朵" }
    };
    size_t i, needed;
    const char *noun = NULL, *classifier = NULL;
    const char *start = "", *open = known_effect ? "（" : "";
    const char *close = known_effect ? "）" : "";
    char quantity[48];

    if (!out || !capacity) return false;
    out[0] = '\0';
    if (!tval || !appearance || !*appearance ||
        (known_effect && !*known_effect)) return false;
    for (i = 0; i < sizeof(types) / sizeof(types[0]); ++i) {
        if (strcmp(tval, types[i].tval) == 0) {
            noun = types[i].noun;
            classifier = types[i].classifier;
            break;
        }
    }
    if (!noun) return false;
    if (prefix && count == 0) {
        start = "沒有剩餘的";
    } else if (prefix && count > 1) {
        int len = snprintf(quantity, sizeof(quantity), "%u %s", count,
                           classifier);
        if (len < 0 || (size_t)len >= sizeof(quantity)) return false;
        start = quantity;
    }
    needed = strlen(start) + strlen(appearance) + strlen(noun) +
        strlen(open) + (known_effect ? strlen(known_effect) : 0) +
        strlen(close);
    if (needed >= capacity) return false;
    (void)snprintf(out, capacity, "%s%s%s%s%s%s", start, appearance,
                   noun, open, known_effect ? known_effect : "", close);
    return true;
}
#endif /* ANGDROID_ZH_TW_FLAVORED_OBJECT_H */
