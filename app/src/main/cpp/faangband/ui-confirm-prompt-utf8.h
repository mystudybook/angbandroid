/* FAangband Yes/No prompt: reserve the controls before copying UTF-8 text.
 * Keep all characters complete and count terminal cells, never bytes.
 * Presentation only: confirmation semantics and source data are unchanged.
 */
#ifndef ANGDROID_FA_CONFIRM_PROMPT_UTF8_H
#define ANGDROID_FA_CONFIRM_PROMPT_UTF8_H

#include <stddef.h>
#include <string.h>
#include "../angband/menu-text-fit.h"

/* Returns visible terminal columns, excluding the trailing NUL. */
static inline size_t angdroid_fa_confirm_prompt_line(
    char *dest, size_t capacity, const char *question, int terminal_cols)
{
    const char *suffix = "[y/n] ";
    size_t suffix_bytes = 6, suffix_cols = 6, width, used, cells;

    if (!dest || !capacity) return 0;
    dest[0] = '\0';
    if (terminal_cols <= 0) return 0;
    width = (size_t)terminal_cols;

    /* Even a tiny window should present a meaningful choice, rather
     * than a broken fragment of the normal [y/n] control label. */
    if (width < suffix_cols || capacity <= suffix_bytes) {
        const char *small = width == 1 ? "?" :
            width == 2 ? "yn" : width < suffix_cols ? "y/n" : suffix;
        return angdroid_menu_copy_columns(dest, capacity, small, width);
    }

    /* Leave enough BYTES for the intact ASCII suffix. The preceding
     * text is copied by display cells, never by UTF-8 byte precision. */
    cells = angdroid_menu_copy_columns(dest, capacity - suffix_bytes,
        question, width - suffix_cols);
    used = strlen(dest);
    memcpy(dest + used, suffix, suffix_bytes + 1);
    return cells + suffix_cols;
}

#endif /* ANGDROID_FA_CONFIRM_PROMPT_UTF8_H */
