/* Display-only known kind and identified ego names for FAangband.
 * The caller owns all identification, flavor, spoiler and language guards.
 * Return NULL when no translation applies; never change canonical data.
 */
#ifndef ANGDROID_FAANGBAND_OBJ_DESC_ITEM_ZH_TW_H
#define ANGDROID_FAANGBAND_OBJ_DESC_ITEM_ZH_TW_H
#include <stdbool.h>
#include "../angband/zh-tw-items.h"
#include "../angband/zh-tw-faangband-items.h"
#include "../angband/zh-tw-class-book-names.h"
#include "../angband/zh-tw-ego-names.h"
#include "zh-tw-faangband-ego-names.h"

static inline const char *angdroid_faangband_known_kind_ui_name(
    const char *english, const char *tval, bool translate)
{
    const char *name;
    if (!english || !tval || !translate) return NULL;
    name = angdroid_zh_tw_item_kind_name(english);
    if (!name) name = angdroid_zh_tw_class_book_name(english, tval);
    if (!name)
        name = angdroid_zh_tw_faangband_item_kind_name(english, tval);
    return name;
}

static inline const char *angdroid_faangband_known_ego_ui_name(
    const char *english, bool translate)
{
    const char *name;
    if (!english || !translate) return NULL;
    name = angdroid_zh_tw_ego_name(english);
    if (!name) name = angdroid_zh_tw_faangband_ego_name(english);
    return name;
}
#endif /* ANGDROID_FAANGBAND_OBJ_DESC_ITEM_ZH_TW_H */
