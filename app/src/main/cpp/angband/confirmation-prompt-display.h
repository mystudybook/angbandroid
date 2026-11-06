/* UTF-8-safe yes/no and choice prompts for Angband terminal UI.
 * The original input-event handling and English command keys are unchanged.
 * Reserve enough bytes and display cells for the actionable [y/n] suffix.
 */
#ifndef ANGDROID_CONFIRMATION_PROMPT_DISPLAY_H
#define ANGDROID_CONFIRMATION_PROMPT_DISPLAY_H
#include <stddef.h>
#include <string.h>
#include "menu-text-fit.h"

static inline size_t angdroid_confirmation_prompt_display(
    char *out, size_t capacity, const char *message, const char *suffix,
    size_t terminal_columns)
{
    size_t suffix_cols, suffix_bytes, message_cols, message_bytes, used;
    size_t available, prefix_capacity;

    if (!out || capacity == 0) return 0;
    out[0] = '\0';
    if (!suffix || terminal_columns == 0) return 0;

    suffix_cols = angdroid_utf8_display_columns(suffix);
    suffix_bytes = strlen(suffix);
    /* The suffix is needed to tell players how to respond. Leave room
     * for it before copying any potentially long multi-byte item name. */
    available = terminal_columns > suffix_cols ?
        terminal_columns - suffix_cols : 0;
    if (available > 70) available = 70;
    prefix_capacity = suffix_bytes < capacity ? capacity - suffix_bytes : 1;
    message_cols = angdroid_menu_copy_columns(
        out, prefix_capacity, message, available);
    message_bytes = strlen(out);

    used = angdroid_menu_copy_columns(out + message_bytes,
        capacity - message_bytes, suffix, terminal_columns - message_cols);
    return message_cols + used;
}
#endif
