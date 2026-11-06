/* C99, no game dependencies: quiver @tag bounds and fallback semantics. */
#include <assert.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/item-picker-tag-guard.h"

static int checks;
static void test_tag(int tag, int slots, int expected)
{
    assert(angdroid_item_picker_quiver_index(tag, slots) == expected);
    ++checks;
}
int main(void)
{
    int i, cap;

    for (i = 0; i < 10; i++)
        test_tag('0' + i, 10, i);
    /* Different runtime quiver capacities, including an empty quiver. */
    for (cap = 0; cap <= 11; cap++)
        for (i = 0; i < 10; i++)
            test_tag('0' + i, cap, i < cap ? i : -1);
    test_tag('0', -1, -1);
    test_tag('0', 0, -1);
    test_tag('9', 9, -1);
    test_tag('9', 10, 9);
    test_tag('0' - 1, 10, -1);
    test_tag('9' + 1, 10, -1);
    test_tag('@', 10, -1);
    test_tag('a', 10, -1);
    test_tag('Z', 10, -1);
    test_tag(0, 10, -1);
    test_tag(-1, 10, -1);
    test_tag(255, 10, -1);

    printf("PASS: item picker quiver tag bounds (%d checks)\n",
        checks);
    return 0;
}
