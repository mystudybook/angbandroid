/* Host C99 tests for original Angband equipment comparison touch mapping.
 * A touch chooses a filtered/sorted INDEX, not a persistent object ID.
 */
#include <assert.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/equip-compare-touch-guard.h"

static int checks;
static void touch(int x, int y, int width, int height, int row,
    int visible, int first, int direction, int total, bool ok, int expected)
{
    int index = 1234;
    bool hit = angdroid_equip_cmp_touch_index(x, y, width, height, row,
        visible, first, direction, total, &index);
    assert(hit == ok);
    assert(index == (ok ? expected : -1));
    checks += 2;
}
static void clamp(int selected, int first, int visible, int direction,
    int total, int expected)
{
    assert(angdroid_equip_cmp_clamp_visible_selection(selected, first,
        visible, direction, total) == expected);
    ++checks;
}
int main(void)
{
    touch(0, 4, 80, 24, 4, 5, 10, 1, 20, true, 10);
    touch(79, 8, 80, 24, 4, 5, 10, 1, 20, true, 14);
    touch(6, 5, 80, 24, 4, 5, 10, 1, 20, true, 11);
    touch(6, 5, 80, 24, 4, 4, 8, -1, 20, true, 7);
    touch(6, 7, 80, 24, 4, 4, 8, -1, 20, true, 5);
    touch(3, 4, 12, 6, 4, 2, 0, 1, 2, true, 0);

    touch(-1, 4, 80, 24, 4, 5, 10, 1, 20, false, -1);
    touch(80, 4, 80, 24, 4, 5, 10, 1, 20, false, -1);
    touch(0, -1, 80, 24, 4, 5, 10, 1, 20, false, -1);
    touch(0, 24, 80, 24, 4, 5, 10, 1, 20, false, -1);
    touch(0, 3, 80, 24, 4, 5, 10, 1, 20, false, -1);
    touch(0, 9, 80, 24, 4, 5, 10, 1, 20, false, -1);
    touch(0, 4, 0, 24, 4, 5, 10, 1, 20, false, -1);
    touch(0, 4, 80, 0, 4, 5, 10, 1, 20, false, -1);
    touch(0, 4, 80, 24, -1, 5, 10, 1, 20, false, -1);
    touch(0, 4, 80, 24, 4, 0, 10, 1, 20, false, -1);
    touch(0, 4, 80, 24, 4, 5, 10, 1, 0, false, -1);
    touch(0, 4, 80, 24, 4, 5, -1, 1, 20, false, -1);
    touch(0, 4, 80, 24, 4, 5, 20, 1, 20, false, -1);
    touch(0, 4, 80, 24, 4, 5, 10, 0, 20, false, -1);
    touch(0, 4, 80, 24, 4, 5, 10, 2, 20, false, -1);
    touch(0, 4, 80, 24, 4, 21, 0, 1, 20, false, -1);
    touch(0, 8, 80, 24, 4, 5, 18, 1, 20, false, -1);
    touch(0, 8, 80, 24, 4, 5, 1, -1, 20, false, -1);

    { int index = 4;
      assert(!angdroid_equip_cmp_touch_index(0, 4, 80, 24, 4, 5,
          10, 1, 20, NULL));
      assert(index == 4);
      checks += 2;
    }

    assert(angdroid_equip_cmp_selection_valid(0, 1));
    assert(angdroid_equip_cmp_selection_valid(19, 20));
    assert(!angdroid_equip_cmp_selection_valid(-1, 20));
    assert(!angdroid_equip_cmp_selection_valid(20, 20));
    assert(!angdroid_equip_cmp_selection_valid(0, 0));
    checks += 5;

    clamp(-1, 0, 4, 1, 10, -1);
    clamp(0, 0, 4, 1, 10, 0);
    clamp(3, 0, 4, 1, 10, 3);
    clamp(6, 0, 4, 1, 10, 3);
    clamp(-3, 0, 4, 1, 10, 0);
    clamp(7, 8, 4, -1, 10, 7);
    clamp(9, 8, 4, -1, 10, 8);
    clamp(3, 8, 4, -1, 10, 5);
    clamp(4, 8, 4, -1, 10, 5);
    clamp(2, 0, 0, 1, 10, -1);
    clamp(2, 0, 5, 1, 0, -1);
    clamp(2, 10, 5, 1, 10, -1);
    clamp(2, 8, 4, 1, 10, -1);
    clamp(2, 4, 5, 0, 10, -1);
    clamp(2, 4, 11, 1, 10, -1);

    printf("PASS: comparison touch mapping and resize selection (%d checks)\n",
           checks);
    return 0;
}
