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
    {
        const wchar_t first_row[] = {
            L'A', 0x4E2D, ANGDROID_TEXT_CONTINUATION,
            L'B', 0x9F8D, ANGDROID_TEXT_CONTINUATION
        };
        const wchar_t second_row[] = {L'A', L'B', L'C', L'D', L'E', L'F'};
        const wchar_t erased_row[] = {L'A', L' ', L' ', L'B', L' ', L' '};
        const int normal_attr[] = {1, 1, 1, 1, 1, 1};
        const int tile_attr[] = {1, 1, 1, 255, 1, 1};
        int first;
        int last;

        first = 2; last = 2;
        angdroid_expand_redraw_columns(6, &first, &last,
            first_row, normal_attr, first_row, normal_attr);
        assert(first == 1 && last == 2); /* Redraw a CJK trailing column. */

        first = 1; last = 1;
        angdroid_expand_redraw_columns(6, &first, &last,
            first_row, normal_attr, first_row, normal_attr);
        assert(first == 1 && last == 2); /* Redraw a CJK leading column. */

        first = 2; last = 4;
        angdroid_expand_redraw_columns(6, &first, &last,
            first_row, normal_attr, first_row, normal_attr);
        assert(first == 1 && last == 5); /* Both boundaries are cut. */

        first = 2; last = 2;
        angdroid_expand_redraw_columns(6, &first, &last,
            erased_row, normal_attr, first_row, normal_attr);
        assert(first == 1 && last == 2); /* Previous glyph owns area. */

        first = 1; last = 1;
        angdroid_expand_redraw_columns(6, &first, &last,
            erased_row, normal_attr, first_row, normal_attr);
        assert(first == 1 && last == 2); /* Previous glyph extends right. */

        first = 3; last = 3;
        angdroid_expand_redraw_columns(6, &first, &last,
            second_row, tile_attr, second_row, tile_attr);
        assert(first == 2 && last == 3); /* Preserve big tile pad rule. */

        first = 2; last = 2; /* Start a new row from caller's original bounds. */
        angdroid_expand_redraw_columns(6, &first, &last,
            second_row, normal_attr, second_row, normal_attr);
        assert(first == 2 && last == 2); /* No cross-row boundary leakage. */

        first = 0; last = 5;
        angdroid_expand_redraw_columns(6, &first, &last,
            first_row, normal_attr, first_row, normal_attr);
        assert(first == 0 && last == 5); /* Full-width redraw. */
    }
    {
        const wchar_t row[] = {L'A', 0x4E2D, ANGDROID_TEXT_CONTINUATION,
                               L'B', 0x9F8D, ANGDROID_TEXT_CONTINUATION};
        const wchar_t orphan[] = {ANGDROID_TEXT_CONTINUATION, L'X'};
        const wchar_t not_wide[] = {L'A', 0x4E2D, L' '};
        const int attrs[] = {1, 1, 1, 1, 1, 1};
        const int tile_attrs[] = {1, 0x80, 1, 1, 1, 1};
        const int short_attrs[] = {1, 1};

        assert(angdroid_wide_partner_column(6, 1, row, attrs) == 2);
        assert(angdroid_wide_partner_column(6, 2, row, attrs) == 1);
        assert(angdroid_wide_partner_column(6, 4, row, attrs) == 5);
        assert(angdroid_wide_partner_column(6, 5, row, attrs) == 4);
        assert(angdroid_wide_partner_column(6, 0, row, attrs) == -1);
        assert(angdroid_wide_partner_column(6, 3, row, attrs) == -1);
        assert(angdroid_wide_partner_column(6, -1, row, attrs) == -1);
        assert(angdroid_wide_partner_column(6, 6, row, attrs) == -1);
        assert(angdroid_wide_partner_column(2, 0, orphan, short_attrs) == -1);
        assert(angdroid_wide_partner_column(3, 1, not_wide, attrs) == -1);
        assert(angdroid_wide_partner_column(6, 1, row, tile_attrs) == -1);
        assert(angdroid_wide_partner_column(6, 2, row, tile_attrs) == -1);
        assert(angdroid_wide_partner_column(6, 1, NULL, attrs) == -1);
        assert(angdroid_wide_partner_column(6, 1, row, NULL) == -1);
    }
    puts("PASS: native Unicode width, redraw and wide-cell partner tests");
    return 0;
}
