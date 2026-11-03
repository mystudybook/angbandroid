#include <assert.h>
#include <stdio.h>
#include <wchar.h>

#include "../../app/src/main/cpp/curses/unicode_layout.h"

static void fit(const wchar_t *s, size_t max_length, size_t room,
                size_t expected_chars, size_t expected_columns, int clipped)
{
    angdroid_text_fit result =
        angdroid_fit_terminal_text(s, max_length, room, 1);
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
    {
        angdroid_text_fit tile_fit = angdroid_fit_terminal_text(wide, 1, 1, 0x80);
        assert(tile_fit.codepoints == 1 && tile_fit.columns == 1 &&
               !tile_fit.clipped); /* Tile metadata never consumes 2 columns. */
    }

    {
        const wchar_t cells[] = {
            L'A', 0x4E2D, ANGDROID_TEXT_CONTINUATION,
            L'B', 0x9F8D, ANGDROID_TEXT_CONTINUATION
        };
        size_t cursor = 0;
        size_t start = 0;
        size_t count;
        count = angdroid_next_visible_text_run(cells, 6, &cursor, &start);
        assert(count == 2 && start == 0 && cursor == 2);
        count = angdroid_next_visible_text_run(cells, 6, &cursor, &start);
        assert(count == 2 && start == 3 && cursor == 5);
        count = angdroid_next_visible_text_run(cells, 6, &cursor, &start);
        assert(count == 0 && start == 6 && cursor == 6);

        cursor = 2; /* Redraw begins on a continuation, not the lead. */
        count = angdroid_next_visible_text_run(cells, 6, &cursor, &start);
        assert(count == 2 && start == 3);

        cursor = 0;
        {
            const wchar_t pads[] = {ANGDROID_TEXT_CONTINUATION,
                                    ANGDROID_TEXT_CONTINUATION};
            count = angdroid_next_visible_text_run(pads, 2, &cursor, &start);
            assert(count == 0 && cursor == 2);
        }
    }
    puts("PASS: 22 native Unicode row-fit, attribute, marker and redraw cases");
    return 0;
}
