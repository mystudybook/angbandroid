#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/zh-tw-death.h"

int main(void)
{
    const char *english_labels[] = {
        "Information", "Messages", "File dump", "View scores",
        "Examine items", "History", "Spoilers", "New Game", "Quit"
    };
    int i;
    assert(ANGDROID_DEATH_MENU_ITEMS == 9);
    for (i = 0; i < ANGDROID_DEATH_MENU_ITEMS; ++i) {
        const char *en = angdroid_death_menu_label(i, 0);
        const char *zh = angdroid_death_menu_label(i, 1);
        assert(en && zh && strlen(en) && strlen(zh));
        assert(strcmp(en, english_labels[i]) == 0);
        assert(strcmp(en, zh) != 0);
    }
    assert(angdroid_death_menu_label(-1, 1) == NULL);
    assert(angdroid_death_menu_label(9, 1) == NULL);
    assert(ANGDROID_DEATH_PROMPT_COUNT == 11);
    for (i = 0; i < ANGDROID_DEATH_PROMPT_COUNT; ++i) {
        const char *en = angdroid_death_prompt(i, 0);
        const char *zh = angdroid_death_prompt(i, 1);
        assert(en && zh && *en && *zh);
        assert(strcmp(en, zh) != 0);
        if (i == ANGDROID_DEATH_HOME_PAGE) {
            /* The page-number format token must survive translation. */
            assert(strstr(en, "%d") != NULL);
            assert(strstr(zh, "%d") != NULL);
        }
    }
    assert(angdroid_death_prompt(-1, 1) == NULL);
    assert(angdroid_death_prompt(ANGDROID_DEATH_PROMPT_COUNT, 1) == NULL);
    puts("PASS: 9 native death menu labels, 11 translated prompts and fallback");
    return 0;
}
