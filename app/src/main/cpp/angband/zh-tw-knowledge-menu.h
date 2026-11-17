/* Display-only knowledge-menu store labels. Shop indices, menu action IDs,
 * and digit shortcuts remain canonical English-engine control values.
 */
#ifndef ANGDROID_ZH_TW_KNOWLEDGE_MENU_H
#define ANGDROID_ZH_TW_KNOWLEDGE_MENU_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "zh-tw-terrain-trap.h"

/* Return NULL on insufficient space. Never display truncated UTF-8. */
static inline const char *angdroid_knowledge_store_label(
    char *out, size_t capacity, const char *name, int store_number,
    const char *shortcut, int zh_tw)
{
    const char *hint = shortcut ? shortcut : "";
    int length;
    if (!out || !capacity) return NULL;
    out[0] = '\0';
    if (store_number <= 0) return NULL;
    if (name && *name) {
        if (zh_tw) {
            length = snprintf(out, capacity, "查看%s的商品%s",
                angdroid_terrain_name(name, 1), hint);
        } else {
            size_t n = strlen(name);
            length = snprintf(out, capacity,
                "Display %s'%s contents%s", name,
                name[n - 1] == 's' ? "" : "s", hint);
        }
    } else if (zh_tw) {
        length = snprintf(out, capacity, "查看第 %d 間商店的商品%s",
            store_number, hint);
    } else {
        length = snprintf(out, capacity, "Display store %d's contents%s",
            store_number, hint);
    }
    if (length < 0 || (size_t)length >= capacity) {
        out[0] = '\0';
        return NULL;
    }
    return out;
}

/* Do not turn a long custom shop name into an English menu instruction.
 * The numbered Chinese label preserves selection identity and shortcut. */
static inline const char *angdroid_knowledge_store_label_for_menu(
    char *out, size_t capacity, const char *name, int store_number,
    const char *shortcut, int zh_tw)
{
    const char *label = angdroid_knowledge_store_label(out, capacity,
        name, store_number, shortcut, zh_tw);
    if (!label && zh_tw)
        label = angdroid_knowledge_store_label(out, capacity,
            NULL, store_number, shortcut, zh_tw);
    return label;
}
#endif
