/* Independent C99 safety checks for failed equipment comparison layouts.
 * These paths must not read a partially initialized summary or use invalid
 * terminal coordinates, including while a phone is being rotated.
 */
#include <assert.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/equip-compare-state-guard.h"

static int checks;
static void summary(bool failed, bool exists, bool expect)
{
    assert(angdroid_equip_cmp_summary_readable(failed, exists) == expect);
    ++checks;
}
static void region(int width, int height, int row, int col, bool expect)
{
    assert(angdroid_equip_cmp_label_region_valid(
        width, height, row, col) == expect);
    ++checks;
}
int main(void)
{
    summary(false, true, true);
    summary(false, false, false);
    summary(true, true, false);
    summary(true, false, false);

    /* Minimal valid grids; the upper left and final column are allowed. */
    region(1, 1, 0, 0, true);
    region(20, 5, 0, 0, true);
    region(20, 5, 4, 19, true);
    region(20, 5, 0, 19, true);
    region(20, 5, 4, 0, true);

    /* A zero-height/zero-width layout cannot render even the BAIL prompt. */
    region(0, 0, 0, 0, false);
    region(0, 5, 0, 0, false);
    region(20, 0, 0, 0, false);
    region(20, 5, -1, 0, false);
    region(20, 5, 5, 0, false);
    region(20, 5, 0, -1, false);
    region(20, 5, 0, 20, false);
    region(-4, 5, 0, 0, false);
    region(20, -4, 0, 0, false);
    region(1, 1, 1, 0, false);
    region(1, 1, 0, 1, false);

    /* Shrink below the footer boundary, then enlarge in-place. */
    region(80, 24, 23, 0, true);
    region(80, 0, -1, 0, false);
    region(40, 10, 9, 0, true);
    region(40, 10, 10, 0, false);
    region(80, 24, 23, 0, true);

    printf("PASS: equipment comparison failed-layout and geometry guards (%d cases)\n",
           checks);
    return 0;
}
