#ifndef ANGDROID_UNICODE_UTF8_H
#define ANGDROID_UNICODE_UTF8_H

#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

/*
 * Encode an explicit number of wchar_t code points to UTF-8, regardless of
 * the device C locale. Android uses 32-bit wchar_t. Invalid scalars become
 * U+FFFD, so the Java terminal never receives UTF-8 surrogate byte strings.
 *
 * Returns byte length, or (size_t)-1 if the input/capacity is invalid.
 * Always NUL-terminates successful output; callers must use the byte length
 * rather than strlen(), because the input may contain U+0000.
 */
static size_t angdroid_encode_wide_utf8(const wchar_t *input, size_t count,
                                        char *output, size_t capacity)
{
    size_t i;
    size_t offset = 0;

    if (!output || !capacity || (!input && count)) return (size_t)-1;

    for (i = 0; i < count; ++i) {
        uint32_t cp = (uint32_t)input[i];
        size_t bytes;

        if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu)) {
            cp = 0xFFFDu;
        }

        if (cp < 0x80u) bytes = 1;
        else if (cp < 0x800u) bytes = 2;
        else if (cp < 0x10000u) bytes = 3;
        else bytes = 4;

        /* Reserve one extra byte for NUL without unsigned overflow. */
        if (offset >= capacity || bytes >= capacity - offset) {
            return (size_t)-1;
        }

        if (bytes == 1) {
            output[offset++] = (char)cp;
        } else if (bytes == 2) {
            output[offset++] = (char)(0xC0u | (cp >> 6));
            output[offset++] = (char)(0x80u | (cp & 0x3Fu));
        } else if (bytes == 3) {
            output[offset++] = (char)(0xE0u | (cp >> 12));
            output[offset++] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
            output[offset++] = (char)(0x80u | (cp & 0x3Fu));
        } else {
            output[offset++] = (char)(0xF0u | (cp >> 18));
            output[offset++] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
            output[offset++] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
            output[offset++] = (char)(0x80u | (cp & 0x3Fu));
        }
    }

    output[offset] = '\0';
    return offset;
}

#endif
