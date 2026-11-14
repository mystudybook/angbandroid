/* FAangband spell list geometry on short and narrow terminals.
 * Functions are independent of the game engine for strict host C99 tests. */
#ifndef ANGDROID_FA_SPELL_VIEWPORT_H
#define ANGDROID_FA_SPELL_VIEWPORT_H

static inline int angdroid_fa_spell_menu_columns(int terminal_width)
{
    if (terminal_width < 8) return 0;
    return terminal_width < 80 ? terminal_width : 80;
}

static inline int angdroid_fa_spell_menu_rows(
    int terminal_height, int spell_count)
{
    int reserve, maximum;
    /* Row 0 is the prompt, row 1 the header, and browsing needs
     * at least one row beneath the visible spell list. */
    if (terminal_height < 4 || spell_count <= 0) return 0;
    reserve = terminal_height / 3;
    if (reserve < 1) reserve = 1;
    if (reserve > 5) reserve = 5;
    maximum = terminal_height - 1 - reserve;
    if (maximum < 2) maximum = 2;
    /* Adding one for the header must never overflow int. */
    return spell_count >= maximum - 1 ? maximum : spell_count + 1;
}

#endif /* ANGDROID_FA_SPELL_VIEWPORT_H */
