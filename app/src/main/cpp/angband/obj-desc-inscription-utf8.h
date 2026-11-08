/* Format visible item inscriptions without splitting UTF-8 or leaving braces. */
#ifndef ANGDROID_OBJ_DESC_INSCRIPTION_UTF8_H
#define ANGDROID_OBJ_DESC_INSCRIPTION_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "../curses/unicode_display.h"

/* Only the display buffer is modified: player-defined inscriptions and
 * English game labels are passed through unchanged on the source side.
 * Reserve the closing brace and NUL before appending any content. Player
 * notes may be clipped on scalar boundaries; generated labels are atomic. */
static inline size_t angdroid_obj_desc_inscription_append(char *out,
    size_t capacity, size_t *end, const char *const *labels, size_t count,
    bool zh_tw, bool first_is_player_note)
{
    const char *separator = zh_tw ? "、" : ", ";
    const size_t separator_bytes = strlen(separator);
    size_t initial, used, i;
    bool has_label = false;

    if (!out || !capacity || !end || !labels || !count || *end >= capacity)
        return 0;
    initial = used = *end;
    if (capacity - used < 4) return 0; /* space, '{', '}', NUL */

    out[used++] = ' ';
    out[used++] = '{';
    out[used] = '\0';

    for (i = 0; i < count; ++i) {
        const char *label = labels[i];
        size_t label_bytes, before_separator, before_text;
        if (!label) continue;
        label_bytes = strlen(label);
        before_separator = used;

        if (has_label) {
            if (capacity - used < separator_bytes + 2 ||
                label_bytes > capacity - used - separator_bytes - 2)
                break;
            memcpy(out + used, separator, separator_bytes);
            used += separator_bytes;
            out[used] = '\0';
        } else if (!(i == 0 && first_is_player_note) &&
                   label_bytes > capacity - used - 2) {
            break;
        }

        before_text = used;
        /* Pass capacity - 1 to reserve an additional byte for '}'. */
        used = angdroid_utf8_append_complete(out, capacity - 1, used, label);
        if (label_bytes && used == before_text) {
            used = before_separator;
            out[used] = '\0';
            break;
        }
        has_label = true;
        if (used - before_text != label_bytes) break; /* clipped note */
    }

    if (!has_label) {
        out[initial] = '\0';
        return 0;
    }
    out[used++] = '}';
    out[used] = '\0';
    *end = used;
    return used - initial;
}

#endif /* ANGDROID_OBJ_DESC_INSCRIPTION_UTF8_H */
