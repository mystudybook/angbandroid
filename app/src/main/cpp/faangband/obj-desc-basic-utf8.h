/* FAangband short/unknown item labels and money render safely on narrow
 * displays. Keep all canonical game data and English source strings intact.
 */
#ifndef ANGDROID_FAANGBAND_OBJ_DESC_BASIC_UTF8_H
#define ANGDROID_FAANGBAND_OBJ_DESC_BASIC_UTF8_H
#include <stddef.h>
#include <string.h>
#include "../angband/obj-desc-name-utf8.h"
#include "../angband/obj-desc-chest-money-utf8.h"

/* Like my_strcpy(), but never stop halfway through a UTF-8 code point.
 * Invalid input bytes become '?' through the common scalar-safe helper.
 * Unlike the format grammar, this preserves '&', '#', '~' and '|' literally
 * in original unidentified flavors and user-visible source labels. */
static inline size_t angdroid_faangband_copy_utf8(char *out, size_t cap,
    const char *source)
{
    size_t end = 0;
    if (!out || !cap) return 0;
    out[0] = '\0';
    if (!source) return 0;
    (void)angdroid_obj_desc_append_span(
        out, cap, &end, source, strlen(source));
    return end;
}

/* The original empty/unknown-item early returns passed cap == 0 straight
 * into FA's strnfmt(), which asserts(max). Preserve English output when
 * localization is off; translate only explicit opted-in screen calls. */
static inline size_t angdroid_faangband_basic_item_text(char *out,
    size_t cap, bool unknown, bool prefix, bool translated_ui)
{
    const char *label;
    if (unknown)
        label = translated_ui ? "未知物品" :
            (prefix ? "an unknown item" : "unknown item");
    else
        label = translated_ui ? "（空）" : "(nothing)";
    return angdroid_faangband_copy_utf8(out, cap, label);
}

/* Reuse the source-complete Angband money dictionary: FAangband's 11
 * canonical gold kind names are identical. No invented type mappings. */
static inline size_t angdroid_faangband_money_text(char *out, size_t cap,
    int amount, const char *kind, bool ignored, bool translated_ui)
{
    return angdroid_obj_desc_money_text(
        out, cap, amount, kind, ignored, translated_ui);
}
#endif /* ANGDROID_FAANGBAND_OBJ_DESC_BASIC_UTF8_H */
