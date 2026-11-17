/* Line editor cursor indices are Unicode scalars, not byte offsets or
 * terminal cells. Keep these three measurements distinct. */
#ifndef ANGDROID_UI_INPUT_UTF8_H
#define ANGDROID_UI_INPUT_UTF8_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

static inline size_t angdroid_input_cursor_columns(
    const char *text, size_t scalar_position)
{
    size_t bytes = 0, columns = 0;
    uint32_t scalar;
    size_t n;
    if (!text) return 0;
    while (scalar_position && text[bytes]) {
        n = angdroid_utf8_next_scalar(text + bytes, &scalar);
        if (!n) break;
        columns += (size_t)angdroid_terminal_columns(scalar);
        bytes += n;
        --scalar_position;
    }
    return columns;
}

/* Called with a byte-aligned cursor pointer, not buf + scalar_index. */
static inline char *angdroid_input_next_codepoint(char *cursor)
{
    uint32_t scalar;
    size_t n = angdroid_utf8_next_scalar(cursor, &scalar);
    return n ? cursor + n : NULL;
}

/* Do not silently clip a saved name/inscription to an editor width. */
static inline bool angdroid_input_prefill_fits(
    const char *text, size_t capacity)
{
    size_t i;
    if (!text || !capacity) return false;
    for (i = 0; i < capacity; ++i)
        if (!text[i]) return true;
    return false;
}
/* Copy an existing user-owned name/history into an editor only if the
 * entire source fits and every UTF-8 scalar is valid.  Unlike strlcpy(),
 * a truncated prefix is not a valid editable default.  Save data is never
 * changed by this function. */
static inline bool angdroid_input_prefill_copy(
    char *dest, size_t capacity, const char *source)
{
    size_t length, offset;
    if (!dest || !capacity) return false;
    dest[0] = '\0';
    if (!source || !angdroid_input_prefill_fits(source, capacity))
        return false;

    length = strlen(source);
    for (offset = 0; offset < length; ) {
        uint32_t scalar = 0;
        size_t n = angdroid_utf8_next_scalar(source + offset, &scalar);
        if (!n || n > length - offset) return false;
        if (n == 1 && scalar == 0xFFFDu &&
                (unsigned char)source[offset] >= 0x80u)
            return false;
        offset += n;
    }
    memcpy(dest, source, length + 1);
    return true;
}
#endif
