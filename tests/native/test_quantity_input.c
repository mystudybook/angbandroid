#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/quantity-input.h"
static void accepted(const char *s, int max, int wanted)
{
    int value = -1;
    assert(angdroid_parse_quantity(s, max, &value));
    assert(value == wanted);
}
static void rejected(const char *s, int max)
{
    int value = 77;
    assert(!angdroid_parse_quantity(s, max, &value));
    assert(value == 0);
}
int main(void)
{
    static const char *bad[] = {"", " ", "\t", "a", "A", "all", "x5",
        "1a", "12abc", "*abc", "**", "* 2", "2*", "-1", "+1",
        "1.5", "0x10", "12 34", "999999999999999999999999999999",
        "\xEF\xBC\x91", "\xD9\xA1", "\xC0\xAF"};
    size_t i;
    int n;
    char buf[32];
    accepted("0", 10, 0);
    accepted("1", 10, 1);
    accepted("25", 10, 10);
    accepted("*", 10, 10);
    accepted(" * ", 4, 4);
    accepted("\t*\t", 8, 8);
    accepted(" 003 \t", 10, 3);
    accepted("2147483647", INT_MAX, INT_MAX);
    accepted("2147483647", 1, 1);
    accepted("2147483646", INT_MAX, INT_MAX-1);
    rejected("2147483648", INT_MAX);
    rejected("*", 0);
    rejected("1", -1);
    for (i = 0; i < sizeof(bad)/sizeof(bad[0]); ++i)
        rejected(bad[i], 20);
    {
        int value = 123;
        assert(!angdroid_parse_quantity(NULL, 10, &value));
        assert(value == 0);
        assert(!angdroid_parse_quantity("1", 10, NULL));
    }
    for (n = 0; n <= 10000; ++n) {
        int value;
        snprintf(buf, sizeof(buf), "%d", n);
        assert(angdroid_parse_quantity(buf, 37, &value));
        assert(value == (n > 37 ? 37 : n));
    }
    for (n = 1; n < 256; ++n) {
        int value;
        if ((n >= '0' && n <= '9') || n == '*' || n == ' ' || n == '\t') continue;
        buf[0] = (char)n;
        buf[1] = '\0';
        assert(!angdroid_parse_quantity(buf, 12, &value));
        assert(value == 0);
    }
    puts("PASS: only digits or explicit * represent quantities");
    return 0;
}
