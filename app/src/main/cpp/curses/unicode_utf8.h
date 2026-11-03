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


/*
 * Decode a UTF-8 string into 32-bit wchar_t independent of LC_CTYPE.
 * Matches mbstowcs-style output count/capacity behavior: a terminating
 * wchar_t NUL is stored only when the output has spare capacity.
 * Invalid byte sequences consume one byte and emit U+FFFD.
 * A NULL destination requests only the decoded scalar count.
 */
static size_t angdroid_decode_utf8_wide(wchar_t *output, const char *input, int max_chars)
{
    size_t at = 0;
    size_t count = 0;
    size_t capacity;

    if (!input || max_chars < 0) return (size_t)-1;
    capacity = (size_t)max_chars;

    while (input[at]) {
        unsigned char c0 = (unsigned char)input[at];
        uint32_t cp;
        int length = 1;
        int i;
        int valid = 1;

        if (c0 <= 0x7F) cp = c0;
        else if (c0 >= 0xC2 && c0 <= 0xDF) {
            cp = (uint32_t)(c0 & 0x1F);
            length = 2;
        } else if (c0 >= 0xE0 && c0 <= 0xEF) {
            cp = (uint32_t)(c0 & 0x0F);
            length = 3;
        } else if (c0 >= 0xF0 && c0 <= 0xF4) {
            cp = (uint32_t)(c0 & 0x07);
            length = 4;
        } else {
            cp = 0xFFFD;
            valid = 0;
        }
        if (valid) {
            for (i = 1; i < length; ++i) {
                unsigned char continuation = (unsigned char)input[at + (size_t)i];
                if (!continuation || (continuation & 0xC0u) != 0x80u) {
                    valid = 0;
                    break;
                }
                cp = (cp << 6) | (uint32_t)(continuation & 0x3Fu);
            }
            if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu) ||
                (length == 2 && cp < 0x80u) ||
                (length == 3 && cp < 0x800u) ||
                (length == 4 && cp < 0x10000u)) {
                valid = 0;
            }
        }
        if (!valid) {
            cp = 0xFFFD;
            length = 1;
        }
        if (output) {
            if (count >= capacity) return count;
            output[count] = (wchar_t)cp;
        }
        ++count;
        at += (size_t)length;
    }

    if (output && count < capacity) output[count] = 0;
    return count;
}

#endif
