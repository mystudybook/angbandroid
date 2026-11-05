/* Display-only quantity prompts and explicit confirmation for shop operations.
 * Returning false on unrecognized input avoids accidental purchases from
 * queued Android touches that are converted into Enter by inkey().
 */
#ifndef ANGDROID_STORE_CONFIRMATION_TEXT_H
#define ANGDROID_STORE_CONFIRMATION_TEXT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum angdroid_store_quantity_action {
    ANGDROID_STORE_Q_BUY,
    ANGDROID_STORE_Q_TAKE,
    ANGDROID_STORE_Q_SELL,
    ANGDROID_STORE_Q_GIVE,
    ANGDROID_STORE_Q_STASH
};

/* Only an explicit keyboard Y/y may approve a monetary transaction.
 * General touchscreen taps and Enter are not affirmative answers. */
static inline bool angdroid_store_explicit_yes(bool keyboard, unsigned int code)
{
    return keyboard && (code == 'y' || code == 'Y');
}

/* Original max/owned values are supplied by the existing safe UI flow.
 * An owned count of zero remains hidden when item awareness requires it. */
static inline bool angdroid_store_quantity_prompt(char *out, size_t cap,
    enum angdroid_store_quantity_action action, int max, int owned)
{
    const char *verb;
    int n;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (max < 0 || owned < 0) return false;
    switch (action) {
    case ANGDROID_STORE_Q_BUY: verb = "購買"; break;
    case ANGDROID_STORE_Q_TAKE: verb = "取出"; break;
    case ANGDROID_STORE_Q_SELL: verb = "出售"; break;
    case ANGDROID_STORE_Q_GIVE: verb = "交出"; break;
    case ANGDROID_STORE_Q_STASH: verb = "存放"; break;
    default: return false;
    }
    if (owned > 0 && (action == ANGDROID_STORE_Q_BUY ||
                      action == ANGDROID_STORE_Q_TAKE))
        n = snprintf(out, cap, "%s數量（最多 %d，已持有 %d，* 全部）：",
                     verb, max, owned);
    else
        n = snprintf(out, cap, "%s數量（最多 %d，* 全部）：", verb, max);
    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif /* ANGDROID_STORE_CONFIRMATION_TEXT_H */
