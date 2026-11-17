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
#include <limits.h>

#define ANGDROID_COMPACT_PLAYER_FIXED_ROWS 12

static inline int angdroid_compact_player_visible_rows(
    int stat_count, int term_height)
{
    if (term_height <= 0 || stat_count < 0) return 0;
    if (stat_count >= term_height - ANGDROID_COMPACT_PLAYER_FIXED_ROWS)
        return term_height;
    return stat_count + ANGDROID_COMPACT_PLAYER_FIXED_ROWS;
}

/* Do not reuse cached partial state after a resize or a window-flag
 * change.  Multiple flags may share a terminal, so only use incremental
 * character redraws when this is its sole active widget. */
struct angdroid_compact_player_redraw_state {
    const void *terminal;
    int width;
    int height;
    int ready;
};

static inline int angdroid_compact_player_can_redraw_one(
    const struct angdroid_compact_player_redraw_state *state,
    const void *terminal, int term_width, int term_height, int sole_compact_flag)
{
    return state && state->ready && sole_compact_flag &&
        terminal && state->terminal == terminal &&
        term_width > 0 && term_height > 0 &&
        state->width == term_width && state->height == term_height;
}

/* Zero-based rows owned by the compact character display. */
#define ANGDROID_COMPACT_ROW_TITLE      2
#define ANGDROID_COMPACT_ROW_EXP        4
#define ANGDROID_COMPACT_ROW_GOLD       5
#define ANGDROID_COMPACT_ROW_EQUIPMENT  6

static inline int angdroid_compact_row_hp(int stats) {
    return stats >= 0 && stats <= INT_MAX - 11 ? stats + 9 : -1;
}

static inline int angdroid_compact_row_sp(int stats) {
    return stats >= 0 && stats <= INT_MAX - 11 ? stats + 10 : -1;
}

static inline int angdroid_compact_row_monster(int stats) {
    return stats >= 0 && stats <= INT_MAX - 11 ? stats + 11 : -1;
}

#endif /* ANGDROID_COMPACT_PLAYER_VIEWPORT_H */
