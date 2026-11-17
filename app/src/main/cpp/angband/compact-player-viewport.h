/* Pure viewport bounds for the compact character subwindow.
 * Fixed fields: race/class/title (3), level/exp/gold (3), equipment (1),
 * a blank separator (1), AC/HP/SP (3), and monster health (1).
 * Character stats add STAT_MAX further rows.
 *
 * Window-flag changes already clear their entire terminal. Subsequent
 * character-event refreshes need to clear only these owned rows.
 */
#ifndef ANGDROID_COMPACT_PLAYER_VIEWPORT_H
#define ANGDROID_COMPACT_PLAYER_VIEWPORT_H

#define ANGDROID_COMPACT_PLAYER_FIXED_ROWS 12

static inline int angdroid_compact_player_visible_rows(
    int stat_count, int term_height)
{
    if (term_height <= 0 || stat_count < 0) return 0;
    if (stat_count >= term_height - ANGDROID_COMPACT_PLAYER_FIXED_ROWS)
        return term_height;
    return stat_count + ANGDROID_COMPACT_PLAYER_FIXED_ROWS;
}

#endif /* ANGDROID_COMPACT_PLAYER_VIEWPORT_H */
