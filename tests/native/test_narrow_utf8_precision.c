/* Regression test for byte-limited UTF-8 printf precision.
 * The helper comes from production. The small formatter below models only
 * the narrow %s branch; the source-extracted vstrnfmt() has separate tests.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../app/src/main/cpp/angband/zh-tw-format-utf8.h"

static unsigned long assertions;
#define CHECK(expr) do { if (!(expr)) { \
  fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expr); abort(); } \
  ++assertions; } while (0)

static int valid_utf8(const char *str)
{
    while (*str) {
        uint32_t cp;
        size_t step = angdroid_utf8_next_scalar(str, &cp);
        if (!step || (step == 1 && cp == 0xFFFDu &&
            (unsigned char)*str >= 0x80u)) return 0;
        str += step;
    }
    return 1;
}

static void format_with_safe_precision(char *out, size_t capacity,
    const char *fmt, const char *text, int precision)
{
    char *owned = NULL;
    const char *formatted = text;
    int written;

    if (precision >= 0) {
        size_t limit = (size_t)precision;
        size_t prefix = angdroid_utf8_precision_prefix_bytes(text, limit);
        if ((prefix < limit && text[prefix] != '\0') ||
            angdroid_utf8_prefix_contains_invalid(text, prefix)) {
            owned = malloc(prefix + 1);
            CHECK(owned != NULL);
            CHECK(angdroid_utf8_copy_sanitized_prefix(
                owned, text, prefix) == prefix);
            formatted = owned;
        }
    }
    written = snprintf(out, capacity, fmt, precision, formatted);
    free(owned);
    CHECK(written >= 0 && (size_t)written < capacity);
}

int main(void)
{
    static const struct {
        const char *text;
        size_t limit;
        size_t expected;
    } cases[] = {
        {"中文", 0, 0}, {"中文", 1, 0}, {"中文", 2, 0},
        {"中文", 3, 3}, {"中文", 4, 3}, {"中文", 5, 3},
        {"中文", 6, 6}, {"中文", 20, 6},
        {"A中文", 1, 1}, {"A中文", 2, 1}, {"A中文", 3, 1},
        {"A中文", 4, 4}, {"A中文", 5, 4}, {"A中文", 7, 7},
        {"🐱貓", 1, 0}, {"🐱貓", 3, 0}, {"🐱貓", 4, 4},
        {"🐱貓", 5, 4}, {"🐱貓", 6, 4}, {"🐱貓", 7, 7},
        {"ASCII", 3, 3}, {"ASCII", 9, 5},
        {"", 0, 0}, {"", 10, 0}
    };
    size_t i;
    char output[128];

    CHECK(angdroid_utf8_precision_prefix_bytes(NULL, 5) == 0);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        size_t result = angdroid_utf8_precision_prefix_bytes(
            cases[i].text, cases[i].limit);
        CHECK(result == cases[i].expected);
        CHECK(result <= strlen(cases[i].text));
        CHECK(valid_utf8(cases[i].text));
    }

    format_with_safe_precision(output, sizeof(output), "%.*s", "中文", 2);
    CHECK(strcmp(output, "") == 0);
    format_with_safe_precision(output, sizeof(output), "%.*s", "中文", 4);
    CHECK(strcmp(output, "中") == 0);
    format_with_safe_precision(output, sizeof(output), "%8.*s", "中文", 2);
    CHECK(strcmp(output, "        ") == 0);
    format_with_safe_precision(output, sizeof(output), "%-8.*s", "中文", 4);
    CHECK(strcmp(output, "中     ") == 0);
    format_with_safe_precision(output, sizeof(output), "%.*s", "🐱貓", 4);
    CHECK(strcmp(output, "🐱") == 0);
    format_with_safe_precision(output, sizeof(output), "%.*s", "A中文", 3);
    CHECK(strcmp(output, "A") == 0);
    format_with_safe_precision(output, sizeof(output), "%.*s", "中文", -1);
    CHECK(strcmp(output, "中文") == 0);

    {
        char normalized[80];
        const char *damaged = "A\x80" "中\xE4\xB8";
        CHECK(angdroid_utf8_prefix_contains_invalid(damaged, 0) == 0);
        CHECK(angdroid_utf8_prefix_contains_invalid(damaged, 1) == 0);
        CHECK(angdroid_utf8_prefix_contains_invalid(damaged, 2) == 1);
        CHECK(angdroid_utf8_prefix_contains_invalid("中文", 6) == 0);
        CHECK(angdroid_utf8_prefix_contains_invalid("�", 3) == 0);
        CHECK(angdroid_utf8_copy_sanitized_prefix(normalized, damaged, 7) == 7);
        CHECK(strcmp(normalized, "A?中??") == 0);
        CHECK(valid_utf8(normalized));
        CHECK(angdroid_utf8_copy_sanitized_prefix(normalized, "�", 3) == 3);
        CHECK(strcmp(normalized, "�") == 0);
        CHECK(angdroid_utf8_copy_sanitized_prefix(normalized, "\xE4\xB8", 2) == 2);
        CHECK(strcmp(normalized, "??") == 0);
        format_with_safe_precision(output, sizeof(output), "%.*s", damaged, 7);
        CHECK(strcmp(output, "A?中??") == 0);
        CHECK(valid_utf8(output));
        format_with_safe_precision(output, sizeof(output), "%.*s", damaged, 2);
        CHECK(strcmp(output, "A?") == 0);
        format_with_safe_precision(output, sizeof(output), "%8.*s", damaged, 4);
        CHECK(strcmp(output, "      A?") == 0);
        format_with_safe_precision(output, sizeof(output), "%.*s", "�", 3);
        CHECK(strcmp(output, "�") == 0);
    }

    /* Fixed-capacity vstrnfmt() can stop inside the last UTF-8 scalar.
     * The in-place repair must preserve max-1 bytes for callers using
     * its byte count to append or decide whether to grow. */
    {
        const char *sample = "A中文🐱B 臺灣";
        size_t limit, total = strlen(sample);
        for (limit = 0; limit <= total; ++limit) {
            memcpy(output, sample, limit);
            output[limit] = '\0';
            angdroid_utf8_repair_truncated_tail(output, limit);
            CHECK(strlen(output) == limit);
            CHECK(valid_utf8(output));
        }
        memcpy(output, "\xE4\xB8", 2);
        output[2] = '\0';
        angdroid_utf8_repair_truncated_tail(output, 2);
        CHECK(strcmp(output, "??") == 0);
        memcpy(output, "\xF0\x9F\x90", 3);
        output[3] = '\0';
        angdroid_utf8_repair_truncated_tail(output, 3);
        CHECK(strcmp(output, "???") == 0);
        memcpy(output, "中", 3);
        output[3] = '\0';
        angdroid_utf8_repair_truncated_tail(output, 3);
        CHECK(strcmp(output, "中") == 0);
    }

    /* Malformed multibyte prefixes must remain valid after formatting. */
    {
        static const char *parts[] = {"中", "B", "\x80", "\xC0", "🐱", "\xE4\xB8", "�"};
        unsigned seed = 0x201201u;
        for (i = 0; i < 4000; ++i) {
            char source[100];
            size_t used = 0, j;
            int precision = (int)(i % 72);
            for (j = 0; j < 12; ++j) {
                const char *part;
                size_t bytes;
                seed = seed * 1664525u + 1013904223u;
                part = parts[(seed >> 16) % (sizeof(parts) / sizeof(parts[0]))];
                bytes = strlen(part);
                CHECK(used + bytes + 1 < sizeof(source));
                memcpy(source + used, part, bytes);
                used += bytes;
            }
            source[used] = '\0';
            format_with_safe_precision(output, sizeof(output), "%.*s", source, precision);
            CHECK(valid_utf8(output));
            CHECK(strlen(output) <= (size_t)precision);
        }
    }

    for (i = 0; i < 4000; ++i) {
        const char *sample = i % 2 == 0 ? "A中文🐱B" : "繁體中文 ABC";
        int p = (int)(i % 35);
        size_t prefix = angdroid_utf8_precision_prefix_bytes(
            sample, (size_t)p);
        CHECK(prefix <= (size_t)p);
        CHECK(prefix <= strlen(sample));
        format_with_safe_precision(output, sizeof(output), "%.*s",
            sample, p);
        CHECK(valid_utf8(output));
        CHECK(strlen(output) <= (size_t)p);
    }

    printf("PASS: %lu UTF-8 precision assertions\n", assertions);
    return 0;
}
