/* Angband shopkeeper greeting translations retain the canonical printf grammar. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-store-greetings.h"

static int count_string_formats(const char *fmt)
{
    int count = 0;
    const char *p;
    for (p = fmt; *p; ++p) {
        if (*p != '%') continue;
        ++p;
        if (*p == '%') continue;
        assert(*p == 's');
        ++count;
    }
    return count;
}

int main(void)
{
    static const char *const original[10] = {
        "",
        "%s nods to you.",
        "%s says hello.",
        "%s: \"See anything you like, adventurer?\"",
        "%s: \"How may I help you, %s?\"",
        "%s: \"Welcome back, %s.\"",
        "%s: \"A pleasure to see you again, %s.\"",
        "%s: \"How may I be of assistance, good %s?\"",
        "%s: \"You do honour to my humble store, noble %s.\"",
        "%s: \"I and my family are entirely at your service, %s.\""
    };
    size_t i;
    assert(angdroid_store_greeting_format(NULL, 1) == NULL);
    assert(strcmp(angdroid_store_greeting_format("", 1), "") == 0);
    for (i = 1; i < 10; ++i) {
        const char *localized = angdroid_store_greeting_format(original[i], 1);
        assert(localized != original[i]);
        assert(strcmp(angdroid_store_greeting_format(original[i], 0),
                      original[i]) == 0);
        assert(count_string_formats(localized) ==
               count_string_formats(original[i]));
        assert(count_string_formats(localized) == (i < 4 ? 1 : 2));
        assert(strstr(localized, "「") || i < 3);
    }
    {
        const char *unknown = "An unknown shop greeting %s";
        assert(angdroid_store_greeting_format(unknown, 1) == unknown);
        assert(angdroid_store_greeting_format(unknown, 0) == unknown);
    }
    puts("PASS: 9 shop greetings, matching format placeholders, fallback");
    return 0;
}
