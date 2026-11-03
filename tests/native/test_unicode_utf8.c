#include <assert.h>
#include <locale.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "../../app/src/main/cpp/curses/unicode_utf8.h"

static void expect_utf8(const wchar_t *input, size_t count,
                        const unsigned char *expected, size_t bytes)
{
    char output[128];
    size_t n;
    memset(output, 0x55, sizeof(output));
    n = angdroid_encode_wide_utf8(input, count, output, sizeof(output));
    assert(n == bytes);
    assert(memcmp(output, expected, bytes) == 0);
    assert(output[bytes] == '\0');
}


static void expect_wide(const char *utf8, const uint32_t *expected, size_t count)
{
    wchar_t output[32];
    size_t n;
    size_t i;
    memset(output, 0x55, sizeof(output));
    n = angdroid_decode_utf8_wide(output, utf8, 32);
    assert(n == count);
    assert(output[count] == 0);
    for (i = 0; i < count; ++i) {
        assert((uint32_t)output[i] == expected[i]);
    }
    assert(angdroid_decode_utf8_wide(NULL, utf8, 0) == count);
}

int main(void)
{
    const wchar_t ascii[] = {L'A', L'B', L'C'};
    const unsigned char ascii_bytes[] = {'A', 'B', 'C'};
    const wchar_t han[] = {L'A', 0x4E2D, L'B', 0x9F8D};
    const unsigned char han_bytes[] =
        {0x41, 0xE4, 0xB8, 0xAD, 0x42, 0xE9, 0xBE, 0x8D};
    const wchar_t extension_b[] = {0x20000};
    const unsigned char extension_bytes[] = {0xF0, 0xA0, 0x80, 0x80};
    const wchar_t invalid[] = {0xD800, 0x110000};
    const unsigned char replacement_bytes[] =
        {0xEF, 0xBF, 0xBD, 0xEF, 0xBF, 0xBD};
    const wchar_t embedded_nul[] = {L'A', 0, L'B'};
    const unsigned char nul_bytes[] = {0x41, 0x00, 0x42};
    char too_small[4];
    char zero[1];

    /* Keep the C locale: UTF-8 encoding must not depend on system locale. */
    assert(setlocale(LC_CTYPE, "C") != NULL);
    expect_utf8(ascii, 3, ascii_bytes, sizeof(ascii_bytes));
    expect_utf8(han, 4, han_bytes, sizeof(han_bytes));
    expect_utf8(extension_b, 1, extension_bytes, sizeof(extension_bytes));
    expect_utf8(invalid, 2, replacement_bytes, sizeof(replacement_bytes));
    expect_utf8(embedded_nul, 3, nul_bytes, sizeof(nul_bytes));

    assert(angdroid_encode_wide_utf8(han, 4, too_small, sizeof(too_small))
           == (size_t)-1);
    assert(angdroid_encode_wide_utf8(NULL, 1, too_small, sizeof(too_small))
           == (size_t)-1);
    assert(angdroid_encode_wide_utf8(NULL, 0, zero, sizeof(zero)) == 0);
    assert(zero[0] == '\0');
    assert(angdroid_encode_wide_utf8(NULL, 0, NULL, 0) == (size_t)-1);

    {
        const uint32_t mixed[] = {L'A', 0x4E2Du, L'B', 0x9F8Du};
        const uint32_t supplementary[] = {0x20000u};
        const uint32_t bad[] = {0xFFFDu, 0xFFFDu, L'A'};
        const uint32_t overlong[] = {0xFFFDu, 0xFFFDu};
        wchar_t short_output[2] = {0, 0};

        expect_wide("A\xE4\xB8\xAD""B\xE9\xBE\x8D", mixed, 4);
        expect_wide("\xF0\xA0\x80\x80", supplementary, 1);
        expect_wide("\xE4\x80""A", bad, 3);
        expect_wide("\xC0\xAF", overlong, 2);
        assert(angdroid_decode_utf8_wide(short_output,
               "A\xE4\xB8\xAD""B", 2) == 2);
        assert(short_output[0] == L'A' && short_output[1] == 0x4E2D);
        assert(angdroid_decode_utf8_wide(NULL, NULL, 0) == (size_t)-1);
        assert(angdroid_decode_utf8_wide(short_output, "A", -1) == (size_t)-1);
    }

    puts("PASS: 16 native UTF-8 encode/decode cases (C locale)");
    return 0;
}
