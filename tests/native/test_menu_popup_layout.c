#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/menu-popup-layout.h"

static void check(int w, int h, int x, int y, int n, size_t length)
{
    angdroid_menu_popup_box b;
    int result = angdroid_menu_popup_bounds(w, h, x, y, length, n, &b);

    if (w <= 0 || h <= 0 || n <= 0) {
        assert(!result);
        assert(b.col == 0 && b.row == 0);
        assert(b.width == 0 && b.page_rows == 0);
        return;
    }

    assert(result);
    assert(b.col >= 0 && b.row >= 0);
    assert(b.width >= 1 && b.page_rows >= 1);
    assert(b.col <= w - b.width);
    assert(b.row <= h - b.page_rows);
    if (w >= 3) {
        assert(b.col >= 1);
        assert(b.col + b.width < w);
    }
    if (h >= 3) {
        assert(b.row >= 1);
        assert(b.row + b.page_rows < h);
    }
}

int main(void)
{
    angdroid_menu_popup_box b;

    assert(!angdroid_menu_popup_bounds(80, 24, 5, 5, 10, 4, NULL));
    check(0, 24, 5, 5, 5, 10);
    check(80, 0, 5, 5, 5, 10);
    check(80, 24, 5, 5, 0, 10);
    check(-3, 24, 5, 5, 5, 10);
    check(80, -4, 5, 5, 5, 10);

    assert(angdroid_menu_popup_bounds(80, 24, 10, 8, 5, 3, &b));
    assert(b.col == 11 && b.row == 9);
    assert(b.width == 10 && b.page_rows == 3);

    assert(angdroid_menu_popup_bounds(80, 24, 79, 23, 5, 3, &b));
    assert(b.col == 69 && b.row == 20);
    assert(b.width == 10 && b.page_rows == 3);

    assert(angdroid_menu_popup_bounds(80, 24, 5, 23, 50, 100, &b));
    assert(b.width == 55 && b.page_rows == 22 && b.row == 1);

    assert(angdroid_menu_popup_bounds(1, 1, INT_MAX, INT_MIN,
                                     SIZE_MAX, INT_MAX, &b));
    assert(b.col == 0 && b.row == 0 && b.width == 1 && b.page_rows == 1);

    assert(angdroid_menu_popup_bounds(2, 2, INT_MAX, INT_MAX,
                                     SIZE_MAX, INT_MAX, &b));
    assert(b.col == 0 && b.row == 0 && b.width == 2 && b.page_rows == 2);

    assert(angdroid_menu_popup_bounds(80, 24, INT_MAX, INT_MAX,
                                     SIZE_MAX, INT_MAX, &b));
    assert(b.col == 1 && b.row == 1);
    assert(b.width == 78 && b.page_rows == 22);

    /* Broad acceptance sweep for portrait, tiny and desktop-sized surfaces.
     * The selected menu is always wholly within the terminal. */
    {
        int w, h, n;
        for (w = 0; w <= 90; ++w) {
            for (h = 0; h <= 36; ++h) {
                for (n = 0; n <= 40; ++n) {
                    check(w, h, -5, -5, n, 0);
                    check(w, h, w / 2, h / 2, n, 5);
                    check(w, h, INT_MAX, INT_MAX, n, SIZE_MAX);
                }
            }
        }
    }

    puts("PASS: dynamic popup stays within all tested terminal sizes");
    return 0;
}
