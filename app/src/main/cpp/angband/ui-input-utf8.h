/* Line editor cursor indices are Unicode scalars, not byte offsets or
 * terminal cells. Keep these three measurements distinct. */
#ifndef ANGDROID_UI_INPUT_UTF8_H
#define ANGDROID_UI_INPUT_UTF8_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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
#endif
