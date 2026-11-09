/* Display-only equipment slot labels: fixed 14 terminal columns.
 * utf8_strlen()/utf8_clipto() count Unicode scalars, NOT CJK screen columns.
 * Preserve the original 14-column English rendering; never split UTF-8
 * sequences or mutate source equipment names.
 */
#ifndef ANGDROID_INVENTORY_EQUIP_MENTION_H
#define ANGDROID_INVENTORY_EQUIP_MENTION_H
#include <stddef.h>
#include "menu-text-fit.h"

static inline size_t angdroid_inventory_equip_mention(char *dest,
    size_t capacity, const char *mention)
{
    const size_t target_columns = 14;
    size_t columns, bytes;
    if (!dest || !capacity) return 0;
    columns = angdroid_menu_copy_columns(dest, capacity, mention,
        target_columns);
    bytes = 0;
    while (bytes < capacity && dest[bytes]) ++bytes;
    if (bytes >= capacity) return columns;
    while (columns < target_columns && bytes + 1 < capacity) {
        dest[bytes++] = ' ';
        dest[bytes] = '\0';
        ++columns;
    }
    return columns;
}
#endif
