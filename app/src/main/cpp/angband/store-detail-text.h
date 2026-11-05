/* All-or-nothing store numeric labels for phone-width terminal columns.
 * Pure display formatting; never alters actual stock price or weight.
 */
#ifndef ANGDROID_STORE_DETAIL_TEXT_H
#define ANGDROID_STORE_DETAIL_TEXT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ANGDROID_STORE_PRICE_COLS 14
#define ANGDROID_STORE_WEIGHT_COLS 10

/* Average-priced charge stacks: "均價" occupies four display cells,
 * "avg" only three. If the value is too wide, keep the entire number and
 * omit the optional suffix rather than truncating a real purchase price.
 */
static inline bool angdroid_store_price_text(char *out, size_t cap,
                                             long price, bool average,
                                             bool chinese)
{
    char digits[32];
    size_t n, suffix_cols;
    int written;
    if (!out || !cap) return false;
    out[0] = '\0';
    written = snprintf(digits, sizeof(digits), "%ld", price);
    if (written < 0 || (size_t)written >= sizeof(digits))
        return false;
    n = (size_t)written;
    if (n > ANGDROID_STORE_PRICE_COLS || n >= cap) return false;

    suffix_cols = average ? (chinese ? 5 : 4) : 4;
    if (n + suffix_cols > ANGDROID_STORE_PRICE_COLS) {
        /* Preserve exact numeric value; omit a label that would be cut. */
        memcpy(out, digits, n + 1);
        return true;
    }
    if (average)
        written = snprintf(out, cap, chinese ? "%9ld 均價" : "%9ld avg",
                           price);
    else
        written = snprintf(out, cap, "%9ld    ", price);
    if (written < 0 || (size_t)written >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}

/* Original weight is in tenths of a pound. The Chinese label uses 磅;
 * never show half of a quantity or partial two-cell Chinese suffix. */
static inline bool angdroid_store_weight_text(char *out, size_t cap,
                                              int tenths, bool chinese)
{
    char digits[32];
    int written;
    size_t cells;
    if (!out || !cap) return false;
    out[0] = '\0';
    written = snprintf(digits, sizeof(digits), "%3d.%d",
                       tenths / 10, tenths % 10);
    if (written < 0 || (size_t)written >= sizeof(digits))
        return false;
    cells = (size_t)written;
    if (cells > ANGDROID_STORE_WEIGHT_COLS || cells >= cap) return false;
    if (cells + 3 > ANGDROID_STORE_WEIGHT_COLS) {
        memcpy(out, digits, cells + 1);
        return true;
    }
    written = snprintf(out, cap, chinese ? "%s 磅" : "%s lb", digits);
    if (written < 0 || (size_t)written >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif
