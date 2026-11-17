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
#include <stddef.h>
#include <stdint.h>

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
    uint32_t content_generation;
    int locale_zh_tw;
    uint64_t instance_id;
};

/* Clear both readiness and terminal identity when leaving a world,
 * reloading a character or switching window flags.  A reused terminal
 * pointer with the same dimensions must not inherit an older character. */
static inline void angdroid_compact_player_invalidate(
    struct angdroid_compact_player_redraw_state *state)
{
    if (!state) return;
    state->terminal = NULL;
    state->width = 0;
    state->height = 0;
    state->ready = 0;
    state->content_generation = 0;
    state->locale_zh_tw = 0;
    state->instance_id = 0;
}

static inline int angdroid_compact_player_can_redraw_one(
    const struct angdroid_compact_player_redraw_state *state,
    const void *terminal, int term_width, int term_height, int sole_compact_flag)
{
    return state && state->ready && sole_compact_flag &&
        terminal && state->terminal == terminal &&
        term_width > 0 && term_height > 0 &&
        state->width == term_width && state->height == term_height;
}

/* A physical Term_redraw() only repaints unchanged cells, but
 * Term_clear(), Term_load(), Term_load_all() and Term_resize() replace
 * logical screen contents. A language switch also changes visible labels
 * without necessarily changing terminal dimensions or window flags. */
static inline int angdroid_compact_player_cache_matches(
    const struct angdroid_compact_player_redraw_state *state,
    const void *terminal, int width, int height, int sole_compact_flag,
    uint32_t content_generation, int locale_zh_tw,
    uint64_t instance_id)
{
    return instance_id != 0 &&
        angdroid_compact_player_can_redraw_one(state, terminal,
            width, height, sole_compact_flag) &&
        state->content_generation == content_generation &&
        state->locale_zh_tw == locale_zh_tw &&
        state->instance_id == instance_id;
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
