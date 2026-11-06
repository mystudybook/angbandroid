/* Strict numeric quantity parser. Item counts/limits remain in the engine. */
#ifndef ANGDROID_QUANTITY_INPUT_H
#define ANGDROID_QUANTITY_INPUT_H
#include <stdbool.h>
#include <limits.h>
static inline bool angdroid_parse_quantity(const char *input, int max, int *amount)
{
    const unsigned char *p;
    int value = 0;
    bool digits = false;
    if (amount) *amount = 0;
    if (!amount || !input || max <= 0) return false;
    p = (const unsigned char *)input;
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '*') {
        ++p;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p) return false;
        *amount = max;
        return true;
    }
    while (*p >= '0' && *p <= '9') {
        int digit = (int)(*p - '0');
        if (value > (INT_MAX - digit) / 10) return false;
        value = value * 10 + digit;
        digits = true;
        ++p;
    }
    if (!digits) return false;
    while (*p == ' ' || *p == '\t') ++p;
    if (*p) return false;
    *amount = value > max ? max : value;
    return true;
}

/* The interactive quantity dialog remembers the last valid default. The
 * base parser intentionally clears its output on failure, so parse into a
 * temporary value and commit only once validation succeeds. This must never
 * reinterpret a malformed quantity as zero or as the entire stack. */
static inline bool angdroid_parse_quantity_preserving_default(
    const char *input, int max, int *amount)
{
    int parsed = 0;
    if (!amount || !angdroid_parse_quantity(input, max, &parsed))
        return false;
    *amount = parsed;
    return true;
}
#endif
