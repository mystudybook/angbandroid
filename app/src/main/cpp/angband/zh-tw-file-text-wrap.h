/* File export of native UTF-8 lore uses display columns, not encoded bytes.
 * Presentation only: never modifies canonical monster descriptions. */
#ifndef ANGDROID_ZH_TW_FILE_TEXT_WRAP_H
#define ANGDROID_ZH_TW_FILE_TEXT_WRAP_H

#include <stddef.h>
#include <stdint.h>
#include "../curses/unicode_display.h"

typedef void (*angdroid_text_file_write_bytes)(void *, const char *, size_t);
typedef void (*angdroid_text_file_write_char)(void *, char);

static inline int angdroid_text_has_non_ascii(const char *text)
{
    if (!text) return 0;
    while (*text) {
        if ((unsigned char)*text >= 0x80u) return 1;
        ++text;
    }
    return 0;
}

/* Render only complete UTF-8 scalars. `position` is a display-column count
 * shared across successive text_out_to_file() calls. Invalid high bytes
 * become '?' rather than being written to the output stream. The legacy
 * English/ASCII renderer remains in charge of ASCII-only input. */
static inline void angdroid_utf8_text_file_wrap(
    const char *text, int *position, int wrap, int indent, void *context,
    angdroid_text_file_write_bytes write_bytes,
    angdroid_text_file_write_char write_char)
{
    int margin;
    if (!text || !position || !write_bytes || !write_char) return;
    if (wrap <= 0) wrap = 75;
    margin = indent > 0 ? indent : 0;
    if (margin >= wrap) margin = wrap - 1;

    while (*text) {
        uint32_t codepoint;
        size_t n = angdroid_utf8_next_scalar(text, &codepoint);
        int span = angdroid_terminal_columns(codepoint);
        int i;

        if (*position == 0) {
            for (i = 0; i < margin; ++i) write_char(context, ' ');
            *position = margin;
        }

        if (codepoint == (uint32_t)'\n') {
            write_char(context, '\n');
            *position = 0;
            text += n;
            continue;
        }

        /* Keep short English words intact inside mixed Chinese prose.
         * The old ASCII-only writer already wraps at spaces; without this,
         * adding one Chinese word can split the next English word.
         * Only pre-wrap if the entire next word fits a fresh line. */
        if (codepoint == (uint32_t)' ' && *position > margin) {
            const char *word = text;
            int word_columns = 0;
            int word_fits = 1;
            int max_word_columns = wrap - margin;
            while (*word == ' ') ++word;
            while (*word) {
                uint32_t word_scalar;
                size_t bytes = angdroid_utf8_next_scalar(word,
                    &word_scalar);
                int cells;
                if (word_scalar == (uint32_t)' ' ||
                    word_scalar == (uint32_t)'\n') break;
                cells = angdroid_terminal_columns(word_scalar);
                if (cells > max_word_columns - word_columns) {
                    word_fits = 0;
                    break;
                }
                word_columns += cells;
                word += bytes;
            }
            if (word_columns > 0 && word_fits &&
                word_columns >= wrap - *position) {
                write_char(context, '\n');
                *position = 0;
                while (*text == ' ') ++text;
                continue;
            }
        }

        if (span > wrap - *position) {
            if (*position > margin) {
                write_char(context, '\n');
                *position = 0;
                while (*text == ' ') ++text;
                continue;
            }
            /* A two-cell scalar cannot fit in a one-cell content region. */
            write_char(context, '?');
            ++*position;
            text += n;
            continue;
        }

        if (n == 1 && codepoint == 0xFFFDu &&
            (unsigned char)*text >= 0x80u)
            write_char(context, '?');
        else
            write_bytes(context, text, n);

        *position += span;
        text += n;
    }
}
#endif
