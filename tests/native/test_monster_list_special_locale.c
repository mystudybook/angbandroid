/* Monster-list status strings are UI-only; native gameplay stays untouched. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-monster-list-ui.h"

int main(void)
{
    const char *en = angdroid_monster_list_hallucination_text(0);
    const char *zh = angdroid_monster_list_hallucination_text(1);
    assert(strcmp(en, "Your hallucinations are too wild to see things clearly.\n") == 0);
    assert(strcmp(zh, "幻覺太強烈，無法清楚辨識怪物。\n") == 0);
    assert(en[strlen(en) - 1] == '\n');
    assert(zh[strlen(zh) - 1] == '\n');
    assert(angdroid_monster_list_hallucination_columns(0) == strlen(en));
    assert(angdroid_monster_list_hallucination_columns(1) ==
        angdroid_utf8_display_columns(zh));
    assert(angdroid_monster_list_hallucination_columns(1) < strlen(zh));
    assert(strcmp(angdroid_monster_list_sort_hint(0, 0),
        "Press 'x' to turn ON 'sort by exp'") == 0);
    assert(strcmp(angdroid_monster_list_sort_hint(0, 1),
        "Press 'x' to turn OFF 'sort by exp'") == 0);
    assert(strcmp(angdroid_monster_list_sort_hint(1, 0),
        "按 x 開啟依經驗值排序") == 0);
    assert(strcmp(angdroid_monster_list_sort_hint(1, 1),
        "按 x 關閉依經驗值排序") == 0);
    puts("PASS: monster-list hallucination and sort hint locales");
    return 0;
}
