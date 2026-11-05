/* Display-only formatting of original randomized Angband scroll titles.
 * Flavor generation and seed_flavor in obj-util.c must never be changed here.
 * The letters are fictional identifiers, not translatable canonical words.
 */
#ifndef ANGDROID_ZH_TW_SCROLL_TITLE_H
#define ANGDROID_ZH_TW_SCROLL_TITLE_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* Render the ORIGINAL title in Chinese punctuation without modifying it.
 * full: "題為「...」的卷軸"; short: "「...」".
 * Invalid or insufficient capacity: return false and leave output empty.
 * Input is expected to match scroll_adj: a quoted, short ASCII title.
 */
static inline bool angdroid_zh_tw_scroll_title(char *out, size_t capacity,
                                              const char *canonical, bool full)
{
    const char *before = full ? "題為「" : "「";
    const char *after = full ? "」的卷軸" : "」";
    size_t n, i, left, right, letters;

    if (!out || capacity == 0) return false;
    out[0] = '\0';
    if (!canonical || canonical[0] != '"') return false;
    n = strlen(canonical);
    if (n < 4 || n > 17 || canonical[n - 1] != '"') return false;
    if (canonical[1] == ' ' || canonical[n - 2] == ' ') return false;
    for (i = 1; i + 1 < n; ++i) {
        unsigned char c = (unsigned char)canonical[i];
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') || c == ' '))
            return false;
    }

    left = strlen(before);
    right = strlen(after);
    letters = n - 2;
    if (left + letters + right + 1 > capacity) return false;
    memcpy(out, before, left);
    memcpy(out + left, canonical + 1, letters);
    memcpy(out + left + letters, after, right);
    out[left + letters + right] = '\0';
    return true;
}

#endif /* ANGDROID_ZH_TW_SCROLL_TITLE_H */
