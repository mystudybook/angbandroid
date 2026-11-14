/* FAangband history viewport math, independent of rendering or save data. */
#ifndef ANGDROID_FA_HISTORY_VIEWPORT_H
#define ANGDROID_FA_HISTORY_VIEWPORT_H
#include <stddef.h>

static inline int angdroid_fa_history_page_rows(int height)
{
    return height > 3 ? height - 3 : 0;
}

static inline size_t angdroid_fa_history_clamp_top(size_t top, size_t count)
{
    if (!count) return 0;
    return top < count ? top : count - 1;
}

static inline size_t angdroid_fa_history_step(size_t top,
    size_t count, int steps, int forward)
{
    top = angdroid_fa_history_clamp_top(top, count);
    if (!count || steps <= 0) return top;
    if (forward) {
        size_t remaining = count - 1 - top;
        return (size_t)steps >= remaining ? count - 1 : top + (size_t)steps;
    }
    return (size_t)steps >= top ? 0 : top - (size_t)steps;
}

/* Reserve enough room for event text on 20-40 column terminals. */
static inline int angdroid_fa_history_place_columns(int width)
{
    if (width < 30) return 0;
    if (width < 40) return 6;
    if (width < 60) return 10;
    return 20;
}

#endif /* ANGDROID_FA_HISTORY_VIEWPORT_H */
