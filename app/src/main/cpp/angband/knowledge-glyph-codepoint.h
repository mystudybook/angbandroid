/* Strict Unicode scalar validation for the native Angband glyph editor.
 * The editor asks for up to five ASCII hex digits. Do not treat invalid or
 * cancelled input as U+0000, and never persist unprintable terminal controls.
 */
#ifndef ANGDROID_KNOWLEDGE_GLYPH_CODEPOINT_H
#define ANGDROID_KNOWLEDGE_GLYPH_CODEPOINT_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

static inline bool angdroid_knowledge_parse_glyph_hex(
    const char *input, uint32_t *value)
{
    uint32_t codepoint = 0;
    size_t i;
    if (!input || !value) return false;

    for (i = 0; i < 5 && input[i]; ++i) {
        unsigned char c = (unsigned char)input[i];
        uint32_t digit;
        if (c >= '0' && c <= '9') digit = (uint32_t)(c - '0');
        else if (c >= 'a' && c <= 'f') digit = (uint32_t)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') digit = (uint32_t)(c - 'A' + 10);
        else return false;
        codepoint = codepoint * 16 + digit;
    }
    if (i == 0 || input[i] != '\0') return false;
    /* Reject controls, surrogate halves and Unicode noncharacters. */
    if (codepoint < 0x20 || (codepoint >= 0x7f && codepoint <= 0x9f) ||
            (codepoint >= 0xd800 && codepoint <= 0xdfff) ||
            (codepoint >= 0xfdd0 && codepoint <= 0xfdef) ||
            (codepoint & 0xfffeu) == 0xfffeu ||
            codepoint > 0x10ffffu) return false;
    *value = codepoint;
    return true;
}
#endif
