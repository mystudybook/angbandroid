/* UI-only player-relative coordinates for monster and object list rows.
 * Preserve the original " 12 S 35 W" layout in English and the same
 * terminal-column budget in zh-TW (" 12南 35西").  Never change grid data.
 *
 * An optional location must disappear as a whole if its output buffer is
 * too small: neither clipped directions nor partial UTF-8 are useful.
 */
#ifndef ANGDROID_ZH_TW_LIST_LOCATION_H
#define ANGDROID_ZH_TW_LIST_LOCATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../curses/unicode_display.h"

static inline void angdroid_list_relative_location(char *out,
    size_t capacity, int dy, int dx, int zh_tw)
{
    int64_t offset_y = (int64_t)dy;
    int64_t offset_x = (int64_t)dx;
    unsigned int distance_y = (unsigned int)(offset_y < 0 ?
        -offset_y : offset_y);
    unsigned int distance_x = (unsigned int)(offset_x < 0 ?
        -offset_x : offset_x);
    const char *y_direction = offset_y <= 0 ? "N" : "S";
    const char *x_direction = offset_x <= 0 ? "W" : "E";
    const char *y_zh = offset_y <= 0 ? "北" : "南";
    const char *x_zh = offset_x <= 0 ? "西" : "東";
    char formatted[64];
    int length;

    if (!out || capacity == 0) return;
    out[0] = '\0';
    if (zh_tw) {
        length = snprintf(formatted, sizeof formatted, " %u%s %u%s",
            distance_y, y_zh, distance_x, x_zh);
    } else {
        length = snprintf(formatted, sizeof formatted, " %u %s %u %s",
            distance_y, y_direction, distance_x, x_direction);
    }
    if (length < 0 || (size_t)length >= sizeof formatted ||
        (size_t)length >= capacity) return;
    angdroid_utf8_copy_complete(out, capacity, formatted);
}

#endif /* ANGDROID_ZH_TW_LIST_LOCATION_H */
