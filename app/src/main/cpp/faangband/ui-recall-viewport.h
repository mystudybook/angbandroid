/* FAangband recall menu: pure, terminal-bounded layout and selection rules.
 * Keep these independent of the menu framework for host C99 coverage. */
#ifndef ANGDROID_FA_RECALL_VIEWPORT_H
#define ANGDROID_FA_RECALL_VIEWPORT_H

struct angdroid_fa_recall_layout {
    int col;
    int row;
    int width;
    int page_rows;
};

static inline struct angdroid_fa_recall_layout
angdroid_fa_recall_bounds(int width, int height, int entries)
{
    struct angdroid_fa_recall_layout area = { 0, 0, 0, 0 };
    if (width < 8 || height < 4 || entries <= 0) return area;
    area.width = width < 48 ? width : 48;
    area.col = (width - area.width) / 2;
    /* Avoid entries + 2 overflowing int; reserve two title rows. */
    area.page_rows = entries >= height - 2 ? height : entries + 2;
    return area;
}

/* The last two entries are the legacy no-replace/extra-slot choices.
 * Never inspect a recall slot after Escape or for an unselected sentinel. */
static inline int angdroid_fa_recall_can_confirm(
    int inward, int selected_event, int choice, int entries)
{
    return inward && selected_event && choice >= 0 &&
        entries >= 3 && choice < entries - 2;
}

#endif /* ANGDROID_FA_RECALL_VIEWPORT_H */
