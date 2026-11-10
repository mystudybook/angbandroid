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
        if (prefix < limit && text[prefix] != '\0') {
            owned = malloc(prefix + 1);
            CHECK(owned != NULL);
            memcpy(owned, text, prefix);
            owned[prefix] = '\0';
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
