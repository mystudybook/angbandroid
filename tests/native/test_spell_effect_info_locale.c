/* Test the display-only spell summary translations without Angband/Android. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-effect-info.h"

static int checks;

static void check(const char *english, const char *chinese)
{
    const char *translated = angdroid_zh_tw_spell_effect_info(english, 1);
    const char *untranslated = angdroid_zh_tw_spell_effect_info(english, 0);
    assert(strcmp(translated, chinese) == 0);
    assert(untranslated == english);
    checks += 2;
}

int main(void)
{
    const char *unknown = "future effect";
    char formatted[48];

    check("hurt", "自身傷害");
    check("heal", "治療");
    check("dam", "傷害");
    check("dur", "持續");
    check("power", "威力");
    check("range", "距離");
    check("blows", "攻擊次數");
    check("random", "隨機");
    check(", rad %d", ", 半徑 %d");
    check(", rad 2", ", 半徑 2");
    check("rad 2", "半徑 2");
    check(", len %d", ", 長度 %d");

    assert(angdroid_zh_tw_spell_effect_info(unknown, 1) == unknown);
    assert(angdroid_zh_tw_spell_effect_info(unknown, 0) == unknown);
    assert(angdroid_zh_tw_spell_effect_info(NULL, 1) == NULL);
    assert(angdroid_zh_tw_spell_effect_info(NULL, 0) == NULL);
    assert(strcmp(angdroid_zh_tw_spell_effect_info("", 1), "") == 0);
    checks += 5;

    /* Dynamic values are substituted after translating the fixed template. */
    (void)snprintf(formatted, sizeof(formatted),
        angdroid_zh_tw_spell_effect_info(", rad %d", 1), 7);
    assert(strcmp(formatted, ", 半徑 7") == 0);
    (void)snprintf(formatted, sizeof(formatted),
        angdroid_zh_tw_spell_effect_info(", len %d", 0), 12);
    assert(strcmp(formatted, ", len 12") == 0);
    checks += 2;

    printf("PASS: spell effect info bilingual display (%d checks)\n", checks);
    return 0;
}
