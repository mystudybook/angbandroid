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

/* Find the visual row containing a byte offset from the original description.
 * On device rotation this preserves the same sentence rather than reusing an
 * obsolete row number from the old terminal width. */
static inline size_t angdroid_spell_reader_line_for_offset(
    const char *text, size_t columns, size_t offset)
{
    char line[1024];
    size_t start = 0, next, row = 0;
    if (!text || columns < 2) return 0;
    while (angdroid_spell_reader_next_line(text, start, columns,
            line, sizeof(line), &next)) {
        if (next <= start || offset < next) return row;
        start = next;
        ++row;
    }
    return row > 0 ? row - 1 : 0;
}

/* Handle taps only inside the actual terminal view. The title and footer
 * both close the reader; upper/lower text areas scroll in opposite directions.
 * A touch outside the terminal must not inadvertently turn a page. */
enum angdroid_spell_reader_tap {
    ANGDROID_SPELL_READER_TAP_IGNORE = 0,
    ANGDROID_SPELL_READER_TAP_BACK = 1,
    ANGDROID_SPELL_READER_TAP_UP = 2,
    ANGDROID_SPELL_READER_TAP_DOWN = 3
};

static inline int angdroid_spell_reader_tap_action(
    int x, int y, int width, int height)
{
    if (width < 4 || height < 4 || x < 0 || y < 0 ||
        x >= width || y >= height)
        return ANGDROID_SPELL_READER_TAP_IGNORE;
    if (y == 0 || y == height - 1)
        return ANGDROID_SPELL_READER_TAP_BACK;
    return y < height / 2 ? ANGDROID_SPELL_READER_TAP_UP
        : ANGDROID_SPELL_READER_TAP_DOWN;
}

/* Recalculate the book menu's right-aligned width after device rotation.
 * The original menu reserves 15 terminal columns beside the spell list,
 * and caps the list at 80 columns. Never reuse the pre-rotation width. */
static inline int angdroid_spell_reader_menu_width(int terminal_width)
{
    if (terminal_width <= 15) return 0;
    return terminal_width - 15 < 80 ? terminal_width - 15 : 80;
}

/* menu_select() sends an active-row tap as the original EVT_MOUSE after
 * menu_handle_mouse() has moved its cursor, rather than as EVT_SELECT.
 * An outside/header tap is also offered to the row handler as EVT_MOUSE:
 * only a valid, in-bounds primary selection may select or cast a spell.
 * Mouse button 2 is the existing menu 'back' gesture. */
static inline int angdroid_spell_menu_touch_selectable(
    int inside_active, int button, int oid)
{
    return inside_active && button != 2 && oid >= 0;
}


/* A deliberately generous window for confirming a spell by touch.
 * Longer gaps must NEVER commit a stale spell or study selection. */
#define ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS UINT64_C(2000)

/* Keep the selected row and its monotonic timestamp in sync whenever the
 * menu changes, the reader opens, or a gesture is cancelled. */
static inline void angdroid_spell_menu_cancel_touch(
    int *armed_oid, uint64_t *armed_at_ms)
{
    if (armed_oid) *armed_oid = -1;
    if (armed_at_ms) *armed_at_ms = 0;
}

/* This display-only eligibility check never changes a character or casts a
 * spell. First tap arms the row. Only a second tap on the SAME row within
 * two monotonic seconds confirms. A stale, backward, or unavailable clock
 * rearms the row rather than confirming a previously highlighted spell.
 * Browsing stays single tap, and keyboard selection is handled separately. */
static inline int angdroid_spell_menu_confirm_touch(
    int *armed_oid, uint64_t *armed_at_ms, int oid, int browse,
    uint64_t now_ms)
{
    if (!armed_oid || !armed_at_ms) return 0;
    if (oid < 0) {
        angdroid_spell_menu_cancel_touch(armed_oid, armed_at_ms);
        return 0;
    }
    if (browse) {
        angdroid_spell_menu_cancel_touch(armed_oid, armed_at_ms);
        return 1;
    }
    if (*armed_oid == oid && *armed_at_ms != 0 && now_ms != 0 &&
            now_ms > *armed_at_ms &&
            now_ms - *armed_at_ms <=
                ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS) {
        angdroid_spell_menu_cancel_touch(armed_oid, armed_at_ms);
        return 1;
    }
    /* Wrong row, elapsed window, clock rollback or clock failure:
     * this touch is ONLY a new first tap, never a cast. */
    *armed_oid = oid;
    *armed_at_ms = now_ms;
    return 0;
}

#endif
