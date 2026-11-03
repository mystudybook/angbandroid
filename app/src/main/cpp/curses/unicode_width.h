#ifndef ANGDROID_UNICODE_WIDTH_H
#define ANGDROID_UNICODE_WIDTH_H

#include <stdint.h>

/*
 * Keep this small East Asian display-column classifier aligned with the
 * Java TerminalUnicode.isWide() implementation. It does not try to handle
 * zero-width combining marks or full Unicode grapheme segmentation.
 * Native screen-position changes must be reviewed together with ui-term.c.
 */
static inline int angdroid_terminal_columns(uint32_t cp)
{
    return cp >= 0x1100u && (
        cp <= 0x115Fu
        || cp == 0x2329u || cp == 0x232Au
        || (cp >= 0x2E80u && cp <= 0xA4CFu && cp != 0x303Fu)
        || (cp >= 0xAC00u && cp <= 0xD7A3u)
        || (cp >= 0xF900u && cp <= 0xFAFFu)
        || (cp >= 0xFE10u && cp <= 0xFE19u)
        || (cp >= 0xFE30u && cp <= 0xFE6Fu)
        || (cp >= 0xFF00u && cp <= 0xFF60u)
        || (cp >= 0xFFE0u && cp <= 0xFFE6u)
        || (cp >= 0x20000u && cp <= 0x2FFFDu)
        || (cp >= 0x30000u && cp <= 0x3FFFDu)
    ) ? 2 : 1;
}

#endif
