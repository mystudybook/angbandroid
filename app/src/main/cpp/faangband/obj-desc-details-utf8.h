/* FAangband visible object-detail groups: bounds-safe and UTF-8 atomic.
 * The caller determines whether a status is known, and whether the user
 * selected the on-screen Traditional Chinese rendering. No game state here.
 */
#ifndef ANGDROID_FAANGBAND_OBJ_DESC_DETAILS_UTF8_H
#define ANGDROID_FAANGBAND_OBJ_DESC_DETAILS_UTF8_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "../angband/obj-desc-suffix-utf8.h"

/* A whole bracketed bonus or other formatted detail is optional: never
 * leave a lone opening delimiter on a narrow mobile display. */
static inline bool angdroid_faangband_append_detailf(
    char *out, size_t cap, size_t *end, const char *format, ...)
{
    char group[192];
    int length;
    va_list args;
    if (!out || !end || !format || !cap || *end >= cap) return false;
    va_start(args, format);
    length = vsnprintf(group, sizeof(group), format, args);
    va_end(args);
    if (length < 0 || (size_t)length >= sizeof(group)) return false;
    return angdroid_obj_desc_append_suffix_atomic(
        out, cap, end, group, false);
}

/* Translate only engine-owned diagnostic markers, NEVER a user's quark
 * inscription. The original marker remains available to English / exports. */
static inline const char *angdroid_faangband_status_ui_name(
    const char *source, bool translate)
{
    if (!source || !translate) return source;
    if (strcmp(source, "empty") == 0) return "耗盡";
    if (strcmp(source, "tried") == 0) return "已嘗試";
    if (strcmp(source, "cursed") == 0) return "受詛咒";
    if (strcmp(source, "ignore") == 0) return "忽略";
    if (strcmp(source, "unseen") == 0) return "未見過";
    return source; /* "??" and unknown markers are not changed. */
}

/* Verify all bytes and the entire visible list BEFORE touching the output.
 * This avoids dangling " {" and a cut Chinese code point in inscriptions.
 * The count is bounded by the caller's six existing annotation sources. */
static inline bool angdroid_faangband_append_inscription(
    char *out, size_t cap, size_t *end,
    const char *const *labels, size_t count)
{
    size_t i, needed = 3, used;
    if (!out || !cap || !end || !labels || !count || *end >= cap)
        return false;
    used = *end;
    for (i = 0; i < count; ++i) {
        const char *s;
        size_t bytes, pos = 0;
        if (!labels[i]) return false;
        s = labels[i];
        bytes = strlen(s);
        if (i) {
            if (needed > cap - used - 1 ||
                2 > cap - used - 1 - needed) return false;
            needed += 2;
        }
        if (needed > cap - used - 1 ||
            bytes > cap - used - 1 - needed) return false;
        needed += bytes;
        while (pos < bytes) {
            uint32_t scalar;
            size_t n = angdroid_utf8_next_scalar(s + pos, &scalar);
            if (!n || n > bytes - pos ||
                (n == 1 && scalar == 0xFFFDu &&
                 (unsigned char)s[pos] >= 0x80u)) return false;
            pos += n;
        }
    }
    out[used++] = ' ';
    out[used++] = '{';
    for (i = 0; i < count; ++i) {
        size_t bytes = strlen(labels[i]);
        if (i) {
            out[used++] = ',';
            out[used++] = ' ';
        }
        memcpy(out + used, labels[i], bytes);
        used += bytes;
    }
    out[used++] = '}';
    out[used] = '\0';
    *end = used;
    return true;
}

/* The <+N, -M> modifier group cannot be assembled one fragment at a
 * time: a narrow buffer must show the entire group or omit it. */
static inline bool angdroid_faangband_append_modifiers(
    char *out, size_t cap, size_t *end,
    const int *values, size_t count)
{
    char group[512];
    size_t used = 0, i;
    if (!out || !end || !values || !count || !cap || *end >= cap)
        return false;
    group[used++] = ' ';
    group[used++] = '<';
    for (i = 0; i < count; ++i) {
        int written;
        if (i) {
            if (sizeof(group) - used <= 2) return false;
            group[used++] = ',';
            group[used++] = ' ';
        }
        /* Always reserve both the closing '>' and trailing NUL.
         * Check before subtracting to avoid unsigned underflow. */
        if (sizeof(group) - used <= 2) return false;
        written = snprintf(group + used, sizeof(group) - used,
            "%+d", values[i]);
        if (written < 0 || (size_t)written >= sizeof(group) - used - 2)
            return false;
        used += (size_t)written;
    }
    if (used + 2 > sizeof(group)) return false;
    group[used++] = '>';
    group[used] = '\0';
    return angdroid_obj_desc_append_suffix_atomic(
        out, cap, end, group, false);
}
#endif /* ANGDROID_FAANGBAND_OBJ_DESC_DETAILS_UTF8_H */
