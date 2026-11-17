/* Bounds-safe Angband item grammar formatter; does not modify game data. */
#ifndef ANGDROID_OBJ_DESC_NAME_UTF8_H
#define ANGDROID_OBJ_DESC_NAME_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* Copy complete UTF-8 scalar values from a bounded source span. Grammar
 * separators may be adjacent to a multibyte character: never copy half
 * of one. Invalid input bytes become '?' on screen. */
static inline bool angdroid_obj_desc_append_span(char *buf, size_t cap,
    size_t *end, const char *src, size_t bytes)
{
    size_t pos = 0;
    if (!buf || !end || !cap || (!src && bytes)) return false;
    while (pos < bytes) {
        uint32_t scalar;
        size_t n = angdroid_utf8_next_scalar(src + pos, &scalar);
        bool invalid;
        if (!n || n > bytes - pos || *end >= cap) return false;
        invalid = n == 1 && scalar == 0xFFFDu &&
            (unsigned char)src[pos] >= 0x80u;
        if (invalid) {
            if (cap - *end <= 1) return false;
            buf[(*end)++] = '?';
        } else {
            if (n > cap - *end - 1) return false;
            memcpy(buf + *end, src + pos, n);
            *end += n;
        }
        buf[*end] = '\0';
        pos += n;
    }
    return true;
}

/* Keep the original &, ~, |singular|plural| and # grammar. The old
 * obj_desc_name_format() copied literal bytes with buf[end++] even after
 * the destination was full; this version stops on complete glyph edges. */
static inline size_t angdroid_obj_desc_name_format_safe(char *buf,
    size_t cap, size_t end, const char *fmt, const char *modstr, bool pluralise)
{
    const char *start = fmt;
    if (!buf || !cap) return 0;
    if (end >= cap) {
        buf[cap - 1] = '\0';
        return cap - 1;
    }
    buf[end] = '\0';
    if (!fmt) return end;

    while (*fmt) {
        if (*fmt == '&') {
            while (*fmt == ' ' || *fmt == '&') ++fmt;
            continue;
        }
        if (*fmt == '~') {
            char prev = fmt > start ? fmt[-1] : '\0';
            const char *suffix = (prev == 's' || prev == 'h' || prev == 'x') ?
                "es" : "s";
            if (pluralise && !angdroid_obj_desc_append_span(
                    buf, cap, &end, suffix, strlen(suffix))) return end;
            ++fmt;
            continue;
        }
        if (*fmt == '|') {
            const char *singular = fmt + 1;
            const char *middle = strchr(singular, '|');
            const char *closing = middle ? strchr(middle + 1, '|') : NULL;
            const char *chosen;
            size_t bytes;
            if (!closing) return end;
            chosen = pluralise ? middle + 1 : singular;
            bytes = (size_t)((pluralise ? closing : middle) - chosen);
            if (!angdroid_obj_desc_append_span(buf, cap, &end, chosen, bytes))
                return end;
            fmt = closing + 1;
            continue;
        }
        if (*fmt == '#') {
            if (modstr)
                end = angdroid_obj_desc_name_format_safe(buf, cap,
                    end, modstr, NULL, pluralise);
            ++fmt;
            continue;
        }
        {
            uint32_t scalar;
            size_t n = angdroid_utf8_next_scalar(fmt, &scalar);
            if (!n || !angdroid_obj_desc_append_span(
                    buf, cap, &end, fmt, n)) return end;
            fmt += n;
        }
    }
    return end;
}

#endif /* ANGDROID_OBJ_DESC_NAME_UTF8_H */
