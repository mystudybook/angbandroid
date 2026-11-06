/* UTF-8 display-column pagination for the native Angband spell reader.
 * This is presentation-only; the source spell description is never modified.
 */
#ifndef ANGDROID_SPELL_READER_PAGES_H
#define ANGDROID_SPELL_READER_PAGES_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* Return one complete screen row, never cutting inside a UTF-8 character.
 * Physical columns (not encoded bytes) govern wrapping. Explicit newlines
 * can produce empty rows. Returns 0 at the end or for invalid dimensions.
 */
static inline int angdroid_spell_reader_next_line(
    const char *text, size_t start, size_t columns,
    char *out, size_t capacity, size_t *next)
{
    size_t pos, used = 0, last_space = 0, end;
    uint32_t ch;
    size_t step;
    if (!text || !out || !next || !columns || capacity < 2 ||
        !text[start]) return 0;
    pos = start;
    while (text[pos] && text[pos] != '\n') {
        step = angdroid_utf8_next_scalar(text + pos, &ch);
        if (!step) break;
        if ((size_t)angdroid_terminal_columns(ch) > columns - used ||
            pos - start + step >= capacity) break;
        if (ch == ' ' && pos > start) last_space = pos;
        used += (size_t)angdroid_terminal_columns(ch);
        pos += step;
    }
    if (pos == start && text[pos] != '\n') {
        /* The viewport or buffer cannot display even the first glyph.
         * Never move the cursor into the middle of its UTF-8 bytes. */
        out[0] = '\0';
        return 0;
    }
    end = pos;
    if (text[pos] == '\n') {
        *next = pos + 1;
    } else if (text[pos] && last_space > start) {
        /* Prefer a whole English word when a space exists in this row. */
        end = last_space;
        pos = last_space + 1;
        while (text[pos] == ' ') ++pos;
        *next = pos;
    } else {
        *next = pos;
    }
    memcpy(out, text + start, end - start);
    out[end - start] = '\0';
    return 1;
}

static inline size_t angdroid_spell_reader_line_count(
    const char *text, size_t columns)
{
    char line[1024];
    size_t start = 0, next, count = 0;
    if (!text || columns < 2) return 0;
    while (angdroid_spell_reader_next_line(text, start, columns,
        line, sizeof(line), &next)) {
        if (next <= start) break;
        ++count;
        start = next;
    }
    return count;
}

static inline size_t angdroid_spell_reader_clamp_scroll(
    size_t first, size_t count, size_t visible)
{
    size_t last = count > visible ? count - visible : 0;
    return first < last ? first : last;
}

static inline size_t angdroid_spell_reader_page_down(
    size_t first, size_t count, size_t visible)
{
    size_t last = count > visible ? count - visible : 0;
    if (first >= last) return last;
    return visible > last - first ? last : first + visible;
}

static inline size_t angdroid_spell_reader_page_up(
    size_t first, size_t visible)
{
    return first < visible ? 0 : first - visible;
}

#endif
