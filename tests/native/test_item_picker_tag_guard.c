/* C99, no game dependencies: quiver @tag bounds and fallback semantics. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/item-picker-tag-guard.h"
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"

static int checks;
static void test_tag(int tag, int slots, int expected)
{
    assert(angdroid_item_picker_quiver_index(tag, slots) == expected);
    ++checks;
}
int main(void)
{
    int i, cap;
    const char *en, *zh;

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

    en = angdroid_game_ui_text(" ESC", 0);
    zh = angdroid_game_ui_text(" ESC", 1);
    assert(en && strcmp(en, " ESC") == 0);
    assert(zh && strcmp(zh, " Esc 取消") == 0);
    checks += 2;
    assert(strcmp(angdroid_game_ui_text("Unknown picker hint", 1),
        "Unknown picker hint") == 0);
    ++checks;
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    ++checks;
    assert(angdroid_game_ui_text(NULL, 0) == NULL);
    ++checks;
    printf("PASS: item picker quiver tags and bilingual escape hint (%d checks)\n",
        checks);
    return 0;
}
