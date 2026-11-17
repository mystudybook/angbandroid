/* Display-only localized hints for the monster list. */
#ifndef ANGDROID_ZH_TW_MONSTER_LIST_UI_H
#define ANGDROID_ZH_TW_MONSTER_LIST_UI_H
#include <stddef.h>
#include "../curses/unicode_display.h"

static inline const char *angdroid_monster_list_hallucination_text(int zh_tw)
{
    return zh_tw ? "幻覺太強烈，無法清楚辨識怪物。\n" :
        "Your hallucinations are too wild to see things clearly.\n";
}

/* Preserve the historical one-line newline while measuring display
 * columns, rather than counting the three UTF-8 bytes of a Han glyph. */
static inline size_t angdroid_monster_list_hallucination_columns(int zh_tw)
{
    return angdroid_utf8_display_columns(
        angdroid_monster_list_hallucination_text(zh_tw));
}

static inline const char *angdroid_monster_list_sort_hint(int zh_tw,
    int sort_exp)
{
    if (zh_tw)
        return sort_exp ? "按 x 關閉依經驗值排序" : "按 x 開啟依經驗值排序";
    return sort_exp ? "Press 'x' to turn OFF 'sort by exp'" :
        "Press 'x' to turn ON 'sort by exp'";
}

#endif /* ANGDROID_ZH_TW_MONSTER_LIST_UI_H */
