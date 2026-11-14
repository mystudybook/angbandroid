/* Split top-line messages only between complete UTF-8 scalars. */
#ifndef ANGDROID_MESSAGE_WRAP_UTF8_H
#define ANGDROID_MESSAGE_WRAP_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../curses/unicode_display.h"

struct angdroid_message_piece {
    size_t bytes;     /* Bytes to display, excluding an optional break space. */
    size_t columns;   /* Visible terminal columns, never encoded byte count. */
    size_t advance;   /* Bytes consumed, including an optional break space. */
    bool more;
};

static inline struct angdroid_message_piece angdroid_message_next_piece(
    const char *text, size_t max_columns)
{
    struct angdroid_message_piece part = { 0, 0, 0, false };
    size_t bytes = 0, columns = 0;
    size_t space_bytes = 0, space_columns = 0;
    uint32_t cp;

    if (!text || !*text || max_columns == 0) return part;
    while (text[bytes]) {
        size_t count = angdroid_utf8_next_scalar(text + bytes, &cp);
        size_t width;
        if (!count) break;
        width = (size_t)angdroid_terminal_columns(cp);
        if (width > max_columns - columns) break;
        /* Prefer a word boundary only in the latter half of the line.
         * CJK text without spaces must still wrap without losing a glyph. */
        if (cp == ' ' && bytes > 0 && columns >= max_columns / 2) {
            space_bytes = bytes;
            space_columns = columns;
        }
        bytes += count;
        columns += width;
    }

    if (!text[bytes]) {
        part.bytes = bytes;
        part.columns = columns;
        part.advance = bytes;
        return part;
    }

    if (space_bytes) {
        part.bytes = space_bytes;
        part.columns = space_columns;
        part.advance = space_bytes + 1; /* Consume only the wrap delimiter. */
    } else if (bytes) {
        part.bytes = bytes;
        part.columns = columns;
        part.advance = bytes;
    } else {
        /* A very narrow terminal may have fewer columns than one glyph.
         * Consume that glyph atomically so the caller cannot loop forever. */
        part.bytes = angdroid_utf8_next_scalar(text, &cp);
        part.columns = (size_t)angdroid_terminal_columns(cp);
        part.advance = part.bytes;
    }
    part.more = true;
    return part;
}

#endif /* ANGDROID_MESSAGE_WRAP_UTF8_H */
