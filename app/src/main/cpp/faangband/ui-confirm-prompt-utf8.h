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
    if (width > capacity - 1) width = capacity - 1;

    /* Even a tiny window should present a meaningful choice, rather
     * than a broken fragment of the normal [y/n] control label. */
    if (width < suffix_cols) {
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


/* Generic get_char() prompt. Its choices are one-byte keyboard shortcuts.
 * Display the full [options] hint whenever it fits. If a phone has fewer
 * cells, show a compact [a+] marker (the '+' means there are more choices).
 * The complete allowed key set still goes to Android's keyboard strip.
 * Unlike printf %.70s, the question is truncated only at UTF-8 boundaries. */
static inline size_t angdroid_fa_choice_prompt_line(
    char *dest, size_t capacity, const char *question,
    const char *options, int terminal_cols)
{
    size_t width, option_bytes, suffix_bytes, cells, used, shown;

    if (!dest || capacity == 0) return 0;
    dest[0] = '\0';
    if (terminal_cols <= 0) return 0;
    width = (size_t)terminal_cols;
    if (width >= capacity) width = capacity - 1;
    if (!width) return 0;
    if (!options || !options[0]) {
        return angdroid_menu_copy_columns(dest, capacity, question, width);
    }

    option_bytes = strlen(options);
    /* One byte per shortcut; require enough space for "[", "]", space
     * and the NUL before performing any addition or memcpy. */
    if (width >= 3 && capacity >= 4 &&
        option_bytes <= width - 3 && option_bytes <= capacity - 4) {
        suffix_bytes = option_bytes + 3;
        cells = angdroid_menu_copy_columns(
            dest, capacity - suffix_bytes, question, width - suffix_bytes);
        used = strlen(dest);
        dest[used++] = '[';
        memcpy(dest + used, options, option_bytes);
        used += option_bytes;
        dest[used++] = ']';
        dest[used++] = ' ';
        dest[used] = '\0';
        return cells + suffix_bytes;
    }

    /* Preserve at least one choice plus an explicit truncation marker.
     * At 1-3 cells we cannot show an honest multi-choice bracket label. */
    if (width < 4)
        return angdroid_menu_copy_columns(dest, capacity, "?", width);
    shown = width - 3; /* '[', '+', ']' */
    if (shown > option_bytes) shown = option_bytes;
    dest[0] = '[';
    memcpy(dest + 1, options, shown);
    dest[shown + 1] = '+';
    dest[shown + 2] = ']';
    dest[shown + 3] = '\0';
    return shown + 3;
}

#endif /* ANGDROID_FA_CONFIRM_PROMPT_UTF8_H */
