/* FAangband user-visible flavor descriptions.  All helpers are pure:
 * no flavor reassignment, no kind awareness changes, no saved-text edits.
 * The caller decides whether this on-screen localization is enabled.
 */
#ifndef ANGDROID_FAANGBAND_OBJ_DESC_FLAVOR_ZH_TW_H
#define ANGDROID_FAANGBAND_OBJ_DESC_FLAVOR_ZH_TW_H
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "../angband/zh-tw-flavor-names.h"
#include "zh-tw-faangband-flavor-names.h"
#include "../angband/zh-tw-flavored-object.h"
#include "../angband/zh-tw-scroll-title.h"
#include "../angband/zh-tw-item-grammar.h"
#include "../angband/obj-desc-suffix-utf8.h"
#include "obj-desc-item-zh-tw.h"

/* Preserve the exact appearance when the dictionary has no match. */
static inline const char *angdroid_faangband_ui_flavor_name(
    const char *english)
{
    const char *translated;
    if (!english) return NULL;
    translated = angdroid_zh_tw_flavor_name(english);
    if (!translated)
        translated = angdroid_zh_tw_faangband_flavor_name(english);
    return translated ? translated : english;
}

/* Awareness is supplied by the Native object-description caller.
 * When false, source_kind MUST NOT enter any returned display text. */
static inline bool angdroid_faangband_ui_flavored_object(
    char *out, size_t capacity, const char *tval,
    const char *source_appearance, const char *source_kind,
    bool aware, bool prefix, unsigned int count)
{
    const char *appearance, *effect = NULL;
    if (!out || !capacity) return false;
    out[0] = '\0';
    if (!source_appearance || !*source_appearance || !tval) return false;
    appearance = angdroid_faangband_ui_flavor_name(source_appearance);
    if (aware && source_kind && *source_kind) {
        effect = angdroid_faangband_known_kind_ui_name(
            source_kind, tval, true);
        if (!effect) effect = source_kind;
    }
    return angdroid_zh_tw_flavored_object(out, capacity, tval,
        appearance, effect, prefix, count);
}

/* Keep the game-created ASCII scroll title verbatim.  Only translate
 * surrounding UI punctuation and an already-known effect.  Construct
 * the complete label in temporary storage before publishing to caller. */
static inline bool angdroid_faangband_ui_scroll(
    char *out, size_t capacity, const char *canonical_title,
    const char *source_kind, bool aware, bool prefix, unsigned int count)
{
    char title[96], name[256], suffix[192];
    const char *effect;
    size_t end, length;
    if (!out || !capacity) return false;
    out[0] = '\0';
    if (!angdroid_zh_tw_scroll_title(
            title, sizeof(title), canonical_title, true))
        return false;
    if (!angdroid_zh_tw_known_item_name(
            name, sizeof(name), "scroll", title, prefix, count))
        return false;
    if (aware && source_kind && *source_kind) {
        effect = angdroid_faangband_known_kind_ui_name(
            source_kind, "scroll", true);
        if (!effect) effect = source_kind;
        if (!angdroid_zh_tw_item_suffix(
                suffix, sizeof(suffix), effect))
            return false;
        end = strlen(name);
        if (!angdroid_obj_desc_append_suffix_atomic(
                name, sizeof(name), &end, suffix, false))
            return false;
    }
    length = strlen(name);
    if (length >= capacity) return false;
    memcpy(out, name, length + 1);
    return true;
}
#endif /* ANGDROID_FAANGBAND_OBJ_DESC_FLAVOR_ZH_TW_H */
