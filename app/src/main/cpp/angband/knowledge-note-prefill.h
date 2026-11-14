/* Preserve saved user inscriptions exactly in fixed-size edit widgets.
 * If the full UTF-8 text does not fit, reject the edit before any state
 * changes. Never sanitize an existing malformed user note implicitly. */
#ifndef ANGDROID_KNOWLEDGE_NOTE_PREFILL_H
#define ANGDROID_KNOWLEDGE_NOTE_PREFILL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../curses/unicode_display.h"

static inline bool angdroid_knowledge_note_prefill(
    char *out, size_t capacity, const char *existing)
{
    size_t length, offset;
    if (!out || capacity == 0) return false;
    out[0] = '\0';
    if (!existing) return true;
    length = strlen(existing);
    if (length >= capacity) return false;

    offset = 0;
    while (offset < length) {
        uint32_t scalar;
        size_t step = angdroid_utf8_next_scalar(existing + offset, &scalar);
        /* Valid U+FFFD uses 3 bytes; a single invalid byte is rejected. */
        if (step == 1 && scalar == 0xFFFDu &&
                (unsigned char)existing[offset] >= 0x80u)
            return false;
        if (!step || step > length - offset) return false;
        offset += step;
    }

    memcpy(out, existing, length + 1);
    return true;
}
#endif /* ANGDROID_KNOWLEDGE_NOTE_PREFILL_H */
