#ifndef ANGDROID_MENU_VISIBLE_TAGS_H
#define ANGDROID_MENU_VISIBLE_TAGS_H

#include <stddef.h>
#include <string.h>

/* Relative tags on scrolling/object menus describe the visible page.
 * Column menus show all rows and therefore use absolute row indices. */
static inline int angdroid_menu_visible_tag_first(
    int count, int top, int paged)
{
    if (count <= 0 || !paged || top <= 0) return 0;
    return top < count ? top : count;
}

static inline int angdroid_menu_visible_tag_end(
    int count, int top, int page_rows, int paged)
{
    int first = angdroid_menu_visible_tag_first(count, top, paged);

    if (count <= 0) return 0;
    if (!paged) return count;
    if (page_rows <= 0) return first;
    if (page_rows >= count - first) return count;
    return first + page_rows;
}

/* Short tag strings (or negative page-relative offsets) have no tag.
 * Never index before the start or after the terminating NUL. */
static inline char angdroid_menu_tag_at(const char *tags, int index)
{
    if (!tags || index < 0 || (size_t)index >= strlen(tags)) return '\0';
    return tags[index];
}

#endif
