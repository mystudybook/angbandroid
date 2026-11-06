/* Bounded, display-only English/Traditional Chinese item list captions.
 * Numerical values, selection keys, object identities and save data are not
 * modified. The renderer separately checks 9/10 terminal-cell column widths.
 */
#ifndef ANGDROID_INVENTORY_LIST_DISPLAY_H
#define ANGDROID_INVENTORY_LIST_DISPLAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum angdroid_inventory_detail_kind {
    ANGDROID_INVEN_PRICE,
    ANGDROID_INVEN_DEVICE_FAIL,
    ANGDROID_INVEN_DEVICE_UNKNOWN,
    ANGDROID_INVEN_RECHARGE_FAIL,
    ANGDROID_INVEN_WEIGHT
};

/* first and second are the same exact native numeric components that the
 * original list formatter already computed. No hidden knowledge is exposed. */
static inline bool angdroid_inventory_detail_text(
    char *out, size_t cap, enum angdroid_inventory_detail_kind kind,
    int first, int second, bool zh_tw)
{
    int n = -1;
    if (!out || !cap) return false;
    out[0] = '\0';

    switch (kind) {
    case ANGDROID_INVEN_PRICE:
        n = zh_tw ? snprintf(out, cap, "%6d 金", first) :
            snprintf(out, cap, "%6d au", first);
        break;
    case ANGDROID_INVEN_DEVICE_FAIL:
        n = zh_tw ? snprintf(out, cap, "%4d%% 失敗", first) :
            snprintf(out, cap, "%4d%% fail", first);
        break;
    case ANGDROID_INVEN_DEVICE_UNKNOWN:
        n = snprintf(out, cap, "%s",
            zh_tw ? "    ? 失敗" : "    ? fail");
        break;
    case ANGDROID_INVEN_RECHARGE_FAIL:
        n = zh_tw ? snprintf(out, cap, "%2d.%1d%% 失敗", first, second) :
            snprintf(out, cap, "%2d.%1d%% fail", first, second);
        break;
    case ANGDROID_INVEN_WEIGHT:
        n = zh_tw ? snprintf(out, cap, "%4d.%1d 磅", first, second) :
            snprintf(out, cap, "%4d.%1d lb", first, second);
        break;
    default: return false;
    }
    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}

/* The original negative remaining weight means overweight. Cast to long
 * long before negation so INT_MIN is still safe and never leaks a partial
 * UTF-8 Chinese suffix through a small output buffer. */
static inline bool angdroid_inventory_burden_text(
    char *out, size_t cap, int total_tenths, int remaining_tenths, bool zh_tw)
{
    long long delta = remaining_tenths;
    int n;
    if (!out || !cap) return false;
    out[0] = '\0';
    if (delta < 0) delta = -delta;

    if (zh_tw) {
        n = snprintf(out, cap, "負重 %d.%d 磅（%s %lld.%lld 磅） ",
            total_tenths / 10, total_tenths % 10,
            remaining_tenths < 0 ? "超出" : "剩餘",
            delta / 10, delta % 10);
    } else {
        n = snprintf(out, cap, "Burden %d.%d lb (%lld.%lld lb %s) ",
            total_tenths / 10, total_tenths % 10, delta / 10, delta % 10,
            remaining_tenths < 0 ? "overweight" : "remaining");
    }
    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}

/* Preserve quiver slot number, stable item selector and row geometry.
 * Translation is only a display label, never a command/slot identifier. */
static inline bool angdroid_inventory_slot_text(
    char *out, size_t cap, int slot, bool zh_tw)
{
    int n;
    if (!out || !cap) return false;
    out[0] = '\0';
    n = zh_tw ? snprintf(out, cap, "格 %-9d: ", slot) :
        snprintf(out, cap, "Slot %-9d: ", slot);
    if (n < 0 || (size_t)n >= cap) {
        out[0] = '\0';
        return false;
    }
    return true;
}
#endif
