/* CJK-cell-safe copy for fixed knowledge item/rune columns. Display only. */
#ifndef ANGDROID_KNOWLEDGE_COLUMN_TEXT_H
#define ANGDROID_KNOWLEDGE_COLUMN_TEXT_H
#include <stddef.h>
#include "menu-text-fit.h"

static inline size_t angdroid_knowledge_clip_label(char *dest, size_t capacity,
    const char *label, int col, int right_column, int terminal_columns)
{
    int last = right_column < terminal_columns ?
        right_column : terminal_columns;
    size_t available = 0;
    if (col >= 0 && col < terminal_columns && last > col)
        available = (size_t)(last - col);
    return angdroid_menu_copy_columns(dest, capacity, label, available);
}

static inline const char *angdroid_knowledge_tried_suffix(int zh_tw)
{
    return zh_tw ? " {已試用}" : " {tried}";
}
#endif
