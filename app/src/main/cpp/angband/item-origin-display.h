/* A source-exact monster name for the item-origin display line.
 * Build the entire English article/name/comma combination or write nothing:
 * bounded strcat() can leave half of a UTF-8 name in an item lore panel.
 * The caller may use the unmodified source pointer as a last-resort display
 * fallback. Do not change monster race names or item-origin records.
 */
#ifndef ANGDROID_ITEM_ORIGIN_DISPLAY_H
#define ANGDROID_ITEM_ORIGIN_DISPLAY_H
#include <stddef.h>
#include <string.h>

static inline int angdroid_item_origin_monster_name(char *out,
    size_t capacity, const char *source, const char *article,
    int is_unique, int needs_comma)
{
    const char *prefix = is_unique ? "" : article;
    size_t prefix_len, source_len, suffix_len = needs_comma ? 1 : 0;

    if (!out || capacity == 0) return 0;
    out[0] = '\0';
    if (!source || !prefix) return 0;

    prefix_len = strlen(prefix);
    source_len = strlen(source);
    /* Subtraction before addition avoids size_t overflow.  A missing NUL
     * or half a glyph is never an acceptable monster display name. */
    if (prefix_len >= capacity ||
        suffix_len >= capacity - prefix_len ||
        source_len >= capacity - prefix_len - suffix_len) return 0;

    memcpy(out, prefix, prefix_len);
    memcpy(out + prefix_len, source, source_len);
    if (needs_comma) out[prefix_len + source_len] = ',';
    out[prefix_len + source_len + suffix_len] = '\0';
    return 1;
}
#endif /* ANGDROID_ITEM_ORIGIN_DISPLAY_H */
