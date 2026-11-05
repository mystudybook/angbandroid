/* Screen-only Traditional Chinese item classifiers and known-name punctuation.
 * Source objects, tval IDs, English grammar and save/export strings stay as-is.
 */
#ifndef ANGDROID_ZH_TW_ITEM_GRAMMAR_H
#define ANGDROID_ZH_TW_ITEM_GRAMMAR_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ANGDROID_ZH_TW_ITEM_CLASSIFIERS 36
static inline const char *angdroid_zh_tw_item_classifier(const char *tval)
{
    static const struct { const char *english, *unit; }
        units[ANGDROID_ZH_TW_ITEM_CLASSIFIERS] = {
        { "none", "件" },
        { "chest", "個" },
        { "shot", "顆" },
        { "arrow", "支" },
        { "bolt", "支" },
        { "bow", "把" },
        { "digger", "把" },
        { "hafted", "把" },
        { "polearm", "把" },
        { "sword", "把" },
        { "boots", "雙" },
        { "gloves", "雙" },
        { "helm", "頂" },
        { "crown", "頂" },
        { "shield", "面" },
        { "cloak", "件" },
        { "soft armor", "件" },
        { "hard armor", "件" },
        { "dragon armor", "件" },
        { "light", "盞" },
        { "amulet", "枚" },
        { "ring", "枚" },
        { "staff", "根" },
        { "wand", "根" },
        { "rod", "根" },
        { "scroll", "張" },
        { "potion", "瓶" },
        { "flask", "瓶" },
        { "food", "份" },
        { "mushroom", "朵" },
        { "magic book", "本" },
        { "prayer book", "本" },
        { "nature book", "本" },
        { "shadow book", "本" },
        { "other book", "本" },
        { "gold", "枚" },
    };
    size_t i;
    if (!tval) return "件";
    for (i = 0; i < ANGDROID_ZH_TW_ITEM_CLASSIFIERS; ++i)
        if (strcmp(units[i].english, tval) == 0) return units[i].unit;
    return "件";
}

/* The original kind->name for flavored items is an EFFECT (e.g. Strength),
 * not the full kind's name (Ring of Strength). Add the native Chinese noun
 * only if the original object has a flavor and already is identified.
 */
static inline const char *angdroid_zh_tw_identified_flavor_noun(const char *tval)
{
    static const struct { const char *tval, *noun; } nouns[] = {
        { "ring", "戒指" },
        { "amulet", "護身符" },
        { "staff", "法杖" },
        { "wand", "魔杖" },
        { "rod", "魔棒" },
        { "potion", "藥水" },
        { "scroll", "卷軸" },
        { "mushroom", "蘑菇" }
    };
    size_t i;
    if (!tval) return "";
    for (i = 0; i < sizeof(nouns) / sizeof(nouns[0]); ++i)
        if (strcmp(tval, nouns[i].tval) == 0) return nouns[i].noun;
    return "";
}

/* Compose a source-matched, already identified effect + category noun.
 * Avoid duplicate nouns should the localized effect contain one already.
 */
static inline bool angdroid_zh_tw_identified_flavor_name(char *out,
    size_t cap, const char *tval, const char *translated_effect)
{
    const char *noun = angdroid_zh_tw_identified_flavor_noun(tval);
    size_t a, b;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (!translated_effect || !*translated_effect) return false;
    a = strlen(translated_effect);
    b = strlen(noun);
    if (b && a >= b && strcmp(translated_effect + a - b, noun) == 0)
        b = 0;
    if (a >= cap || b >= cap - a) return false;
    memcpy(out, translated_effect, a);
    if (b) memcpy(out + a, noun, b);
    out[a + b] = '\0';
    return true;
}

/* Atomic formatting: never return a partially written UTF-8 name. */
static inline bool angdroid_zh_tw_known_item_name(char *out, size_t cap,
    const char *tval, const char *translated, bool prefix, unsigned int count)
{
    char count_text[48];
    const char *lead = "";
    size_t a, b;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (!translated || !*translated) return false;
    if (prefix && count == 0) lead = "沒有剩餘的";
    else if (prefix && count > 1) {
        int n = snprintf(count_text, sizeof(count_text), "%u %s", count,
                         angdroid_zh_tw_item_classifier(tval));
        if (n < 0 || (size_t)n >= sizeof(count_text)) return false;
        lead = count_text;
    }
    a = strlen(lead);
    b = strlen(translated);
    if (a >= cap || b >= cap - a) return false;
    memcpy(out, lead, a);
    memcpy(out + a, translated, b + 1);
    return true;
}

/* Form only the translated suffix. Already quoted/parenthesized names keep
 * their own punctuation; unmarked ego and named-artifact labels use （）. */
static inline bool angdroid_zh_tw_item_suffix(char *out, size_t cap,
                                              const char *translated)
{
    const char *left, *right;
    size_t a, b, c;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (!translated || !*translated) return false;
    if (strncmp(translated, "（", strlen("（")) == 0 ||
        strncmp(translated, "「", strlen("「")) == 0) {
        left = "";
        right = "";
    } else {
        left = "（";
        right = "）";
    }
    a = strlen(left);
    b = strlen(translated);
    c = strlen(right);
    if (a >= cap || b >= cap - a || c >= cap - a - b) return false;
    memcpy(out, left, a);
    memcpy(out + a, translated, b);
    memcpy(out + a + b, right, c + 1);
    return true;
}

/* Append the longest complete UTF-8 prefix that fits. Used only by opted-in
 * Chinese screen output, not savefile and original English formatter paths. */
static inline size_t angdroid_zh_tw_item_append_utf8(char *out, size_t cap,
                                                      size_t *end,
                                                      const char *text)
{
    size_t length, limit, pos = 0;
    if (!out || !cap || !end || !text || *end >= cap) return 0;
    length = strlen(text);
    limit = cap - *end - 1;
    while (pos < length) {
        unsigned char c = (unsigned char)text[pos];
        size_t width, j;
        bool good = true;
        if (c < 0x80) width = 1;
        else if (c >= 0xC2 && c <= 0xDF) width = 2;
        else if (c >= 0xE0 && c <= 0xEF) width = 3;
        else if (c >= 0xF0 && c <= 0xF4) width = 4;
        else break;
        if (width > length - pos || width > limit - pos) break;
        for (j = 1; j < width; ++j) {
            unsigned char tail = (unsigned char)text[pos + j];
            if ((tail & 0xC0) != 0x80) good = false;
        }
        if (width >= 3) {
            unsigned char second = (unsigned char)text[pos + 1];
            if ((c == 0xE0 && second < 0xA0) ||
                (c == 0xED && second >= 0xA0) ||
                (c == 0xF0 && second < 0x90) ||
                (c == 0xF4 && second >= 0x90)) good = false;
        }
        if (!good) break;
        memcpy(out + *end + pos, text + pos, width);
        pos += width;
    }
    out[*end + pos] = '\0';
    *end += pos;
    return pos;
}
#endif /* ANGDROID_ZH_TW_ITEM_GRAMMAR_H */
