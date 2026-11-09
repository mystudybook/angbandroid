/* Presentation-only targeting / look translations for apparent terrain and
 * visible traps.  Callers must obtain names through square_apparent_name()
 * and check square_isvisibletrap() before reaching the translated display.
 */
#ifndef ANGDROID_ZH_TW_TARGET_WORLD_H
#define ANGDROID_ZH_TW_TARGET_WORLD_H
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "../curses/unicode_display.h"
#include "zh-tw-terrain-trap.h"

static inline const char *angdroid_target_world_intro(const char *en,
    int zh_tw)
{
    if (!en || !zh_tw) return en;
    if (strcmp(en, "You are ") == 0) return "你站在";
    if (strcmp(en, "You see ") == 0) return "你看到";
    if (strcmp(en, "You sense ") == 0) return "你感應到";
    if (strcmp(en, "You recall ") == 0) return "你記得";
    return en;
}


/* English linking prepositions are not grammatical after zh-TW introductions.
 * Preserve unknown variants as-is; this only changes presentation text. */
static inline const char *angdroid_target_world_link(const char *en,
    int zh_tw)
{
    if (!en || !zh_tw) return en;
    if (strcmp(en, "on ") == 0) return "";
    return en;
}

static inline const char *angdroid_target_world_pile_intro(const char *en,
    int zh_tw)
{
    if (!en || !zh_tw) return en;
    if (strcmp(en, "You are ") == 0) return "你所在的位置有";
    return angdroid_target_world_intro(en, true);
}

static inline const char *angdroid_target_world_terrain(const char *apparent,
    int zh_tw)
{
    return angdroid_terrain_name(apparent, zh_tw);
}

static inline const char *angdroid_target_world_trap_name(const char *known,
    int zh_tw)
{
    return angdroid_trap_name(known, zh_tw);
}

static inline const char *angdroid_target_world_trap_desc(const char *known,
    int zh_tw)
{
    return angdroid_trap_lore(known, zh_tw);
}

/* The hallucinating player may know there is something strange, but must not
 * be shown a name from a hidden monster or unrevealed feature. */
/* strnfmt() is byte-limited and may end halfway through a UTF-8 glyph.
 * Finish a zh-TW target-line buffer at the last complete scalar.  The
 * terminal's Term_addstr() separately clips by visible screen columns;
 * this guard prevents malformed UTF-8 before the terminal even sees it.
 * Call only on NUL-terminated output from strnfmt(), never on saved data.
 */
static inline void angdroid_target_world_utf8_trim(char *text,
    size_t capacity, int zh_tw)
{
    size_t offset = 0;
    uint32_t scalar;
    if (!zh_tw || !text || !capacity) return;
    while (offset < capacity && text[offset]) {
        size_t count = angdroid_utf8_next_scalar(text + offset, &scalar);
        if (!count || count > capacity - offset ||
            (count == 1 && scalar == 0xFFFDu &&
             (unsigned char)text[offset] >= 0x80u)) {
            text[offset] = '\0';
            return;
        }
        offset += count;
    }
    if (offset == capacity) {
        /* Defensive: strnfmt() normally guarantees a NUL. */
        text[capacity - 1] = '\0';
    }
}

static inline const char *angdroid_target_world_hallucination(int zh_tw)
{
    return zh_tw ? "某種奇怪的東西" : "something strange";
}
#endif /* ANGDROID_ZH_TW_TARGET_WORLD_H */
