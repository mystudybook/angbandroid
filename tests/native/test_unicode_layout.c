#include <assert.h>
#include <stdio.h>
#include <wchar.h>

#include "../../app/src/main/cpp/curses/unicode_layout.h"

static void fit(const wchar_t *s, size_t max_length, size_t room,
                size_t expected_chars, size_t expected_columns, int clipped)
{
    angdroid_text_fit result =
        angdroid_fit_terminal_text(s, max_length, room);
    assert(result.codepoints == expected_chars);
    assert(result.columns == expected_columns);
    assert(result.clipped == clipped);
}

int main(void)
{
    const wchar_t mixed[] = {L'A', 0x4E2D, L'B', 0x9F8D, 0};
    const wchar_t wide[] = {0x4E2D, 0};
    const wchar_t ascii[] = {L'X', L'Y', 0};

    fit(mixed, 20, 6, 4, 6, 0);
    fit(mixed, 20, 5, 3, 4, 1);
    fit(mixed, 20, 4, 3, 4, 1);
    fit(mixed, 20, 3, 2, 3, 1);
    fit(mixed, 20, 2, 1, 1, 1);
    fit(mixed, 20, 1, 1, 1, 1);
    fit(mixed, 2, 10, 2, 3, 0);
    fit(wide, 10, 1, 0, 0, 1);
    fit(wide, 10, 2, 1, 2, 0);
    fit(ascii, 10, 2, 2, 2, 0);
    fit(ascii, 10, 0, 0, 0, 1);
    fit(NULL, 1, 10, 0, 0, 1);

    assert(angdroid_is_wide_text(0x4E2D, 1));
    assert(!angdroid_is_wide_text(L'A', 1));
    assert(!angdroid_is_wide_text(0x4E2D, 0x80)); /* graphical tile */
    assert(!angdroid_is_wide_text(ANGDROID_TEXT_CONTINUATION, 1));

    puts("PASS: 16 native Unicode row-fit and continuation cases");
    return 0;
}
