/* Fixed CJK object-context popup location, plus transactional quantity retry.
 * Host C99: no Android runtime or Angband main engine required.
 */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/quantity-input.h"
#include "../../app/src/main/cpp/angband/menu-popup-layout.h"

static unsigned long checked;

static void quantity(const char *raw, int max, int previous,
    bool valid, int expected)
{
    int amount = previous;
    bool success = angdroid_parse_quantity_preserving_default(
        raw, max, &amount);
    assert(success == valid);
    assert(amount == expected);
    checked += 2;
}

static void popup(int width, int height, size_t label, int count)
{
    angdroid_menu_popup_box box;
    int result = angdroid_menu_popup_bounds(width, height, width - 1, -1,
        label, count, &box);
    assert(result);
    assert(box.width >= 1 && box.width <= width - 2);
    assert(box.page_rows >= 1 && box.page_rows <= height - 2);
    assert(box.col == width - 1 - box.width);
    assert(box.row == 1);
    checked += 5;
}

int main(void)
{
    int w, h, j;
    const size_t labels[] = { 0, 1, 6, 19, 48, SIZE_MAX };
    const int rows[] = { 1, 3, 36 };

    quantity("oops", 25, 7, false, 7);
    quantity("5x", 25, 7, false, 7);
    quantity("-1", 25, 7, false, 7);
    quantity("9999999999999", 25, 7, false, 7);
    quantity("", 25, 7, false, 7);
    quantity(NULL, 25, 7, false, 7);
    quantity("*abc", 25, 7, false, 7);
    quantity("9", 25, 7, true, 9);
    quantity("*", 25, 7, true, 25);
    quantity("100", 25, 7, true, 25);
    quantity("0", 25, 7, true, 0);
    quantity("  3  ", 25, 7, true, 3);
    quantity("1", 0, 7, false, 7);
    quantity("2", -1, 7, false, 7);
    {
        assert(!angdroid_parse_quantity_preserving_default("1", 25, NULL));
        ++checked;
    }
    /* The original parser still clears output to zero on bad input.
     * Only the interactive wrapper preserves the previous selection. */
    {
        int original = 7;
        assert(!angdroid_parse_quantity("bad", 25, &original));
        assert(original == 0);
        checked += 2;
    }
    for (w = 8; w <= 80; ++w) {
        for (h = 4; h <= 32; ++h) {
            for (j = 0; j < (int)(sizeof(labels)/sizeof(labels[0])); ++j) {
                size_t k;
                for (k = 0; k < sizeof(rows)/sizeof(rows[0]); ++k)
                    popup(w, h, labels[j], rows[k]);
            }
        }
    }
    printf("PASS: item context right-edge CJK layout and quantity retry (%lu checks)\n",
        checked);
    return 0;
}
