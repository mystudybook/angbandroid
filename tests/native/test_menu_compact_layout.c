#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/menu-compact-layout.h"

int main(void)
{
    angdroid_menu_compact_layout out;

    /* Desktop layout keeps decorations without painting outside the term. */
    assert(angdroid_menu_compute_layout(80, 25, 0, 0, 80, 25, 1, 1, 1, &out));
    assert(out.col == 4 && out.width == 76);
    assert(out.row == 3 && out.page_rows == 21);
    assert(out.title_row == 0 && out.header_row == 2 && out.prompt_row == 24);

    /* 7x2 portrait: skip decorations, keep one selectable row. */
    assert(angdroid_menu_compute_layout(7, 2, 0, 0, 99, 99, 1, 1, 1, &out));
    assert(out.col == 0 && out.width == 7);
    assert(out.row == 1 && out.page_rows == 1);
    assert(out.title_row == -1 && out.header_row == 0 && out.prompt_row == -1);

    /* Tiny terminal only has room for game options. */
    assert(angdroid_menu_compute_layout(3, 1, 0, 0, 30, 30, 1, 1, 1, &out));
    assert(out.col == 0 && out.width == 3 && out.page_rows == 1);
    assert(out.title_row == -1 && out.header_row == -1 && out.prompt_row == -1);

    /* A titled menu must shrink its available width after indentation. */
    assert(angdroid_menu_compute_layout(10, 6, 3, 0, 7, 6, 1, 0, 0, &out));
    assert(out.col == 7 && out.width == 3);
    assert(out.row == 2 && out.page_rows == 4);

    /* Indentation is skipped instead of producing a zero-width view. */
    assert(angdroid_menu_compute_layout(4, 6, 0, 0, 4, 6, 1, 0, 0, &out));
    assert(out.col == 0 && out.width == 4);

    /* Rotation may shrink both the width and the height after layout. */
    assert(angdroid_menu_compute_layout(11, 8, 7, 5, 20, 50, 1, 1, 1, &out));
    assert(out.col == 7 && out.width == 4);
    assert(out.row == 7 && out.page_rows == 1);

    assert(!angdroid_menu_compute_layout(0, 6, 0, 0, 1, 1, 0, 0, 0, &out));
    assert(!angdroid_menu_compute_layout(3, 0, 0, 0, 1, 1, 0, 0, 0, &out));
    assert(!angdroid_menu_compute_layout(3, 1, 3, 0, 1, 1, 0, 0, 0, &out));
    assert(!angdroid_menu_compute_layout(3, 1, 0, 0, 0, 1, 0, 0, 0, &out));
    assert(!angdroid_menu_compute_layout(3, 1, 0, 0, 3, 1, 0, 0, 0, NULL));

    /* Enough columns in landscape; gracefully fall back in portrait. */
    assert(!angdroid_menu_use_compact_columns(20, 5, 64, 12));
    assert(angdroid_menu_use_compact_columns(20, 5, 40, 12));
    assert(angdroid_menu_use_compact_columns(20, 5, 12, 12));
    assert(!angdroid_menu_use_compact_columns(4, 5, 10, 12));
    assert(!angdroid_menu_use_compact_columns(0, 0, 0, 12));
    assert(!angdroid_menu_use_compact_columns(INT_MAX, 0, INT_MAX, 12));

    puts("PASS: native menu compact layout preserves clickable cells on rotation");
    return 0;
}
