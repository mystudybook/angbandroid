/* Screen-prose wrap uses terminal display columns, not wchar_t count. */
#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include "../../app/src/main/cpp/curses/unicode_layout.h"

static int assertions = 0;
#define CHECK(expr) do { assert(expr); ++assertions; } while (0)

static int lines_for(const wchar_t *s, int columns, int indent, int pad)
{
    int x = indent + pad;
    int lines = 1;
    for (; *s; ++s) {
        int span;
        if (*s == L'\n') {
            ++lines;
            x = indent + pad;
            continue;
        }
        span = angdroid_is_wide_text(*s, 1) ? 2 : 1;
        if (angdroid_screen_text_needs_wrap(x, columns, *s, 1)) {
            ++lines;
            x = indent + pad;
        }
        CHECK(x + span <= columns - 1);
        x += span;
    }
    return lines;
}

int main(void)
{
    CHECK(!angdroid_screen_text_needs_wrap(6, 10, L'A', 1));
    CHECK(!angdroid_screen_text_needs_wrap(8, 10, L'A', 1));
    CHECK(angdroid_screen_text_needs_wrap(9, 10, L'A', 1));
    CHECK(!angdroid_screen_text_needs_wrap(7, 10, L'中', 1));
    CHECK(angdroid_screen_text_needs_wrap(8, 10, L'中', 1));
    CHECK(angdroid_screen_text_needs_wrap(9, 10, L'中', 1));
    CHECK(!angdroid_screen_text_needs_wrap(8, 10, L'中', 0x80));
    CHECK(angdroid_screen_text_needs_wrap(0, 0, L'A', 1));
    CHECK(lines_for(L"中文法術說明", 10, 0, 0) == 2);
    CHECK(lines_for(L"中文法術說明", 9, 0, 0) == 2);
    CHECK(lines_for(L"A中文B說明", 9, 0, 0) == 2);
    CHECK(lines_for(L"說明，14+1d20 回合。", 12, 0, 0) >= 2);
    CHECK(lines_for(L"中文法術說明", 16, 2, 1) == 1);
    CHECK(lines_for(L"法術說明\n傷害。", 10, 0, 0) == 2);
    CHECK(lines_for(L"ABCDEFGH", 10, 0, 0) == 1);
    CHECK(lines_for(L"ABCDEFGHI", 10, 0, 0) == 1);
    CHECK(lines_for(L"ABCDEFGHIJ", 10, 0, 0) == 2);
    CHECK(angdroid_is_wide_text(L'中', 1));
    CHECK(!angdroid_is_wide_text(L'A', 1));
    printf("PASS: CJK screen text wrapping (%d assertions)\n", assertions);
    return 0;
}
