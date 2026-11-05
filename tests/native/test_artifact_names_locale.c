/* Identified artifact display names are looked up without modifying data. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-artifact-names.h"
static void check(const char *english, const char *chinese)
{
    const char *display = angdroid_zh_tw_artifact_name(english);
    assert(display != NULL);
    assert(strcmp(display, chinese) == 0);
}
int main(void)
{
    assert(ANGDROID_ZH_TW_ARTIFACT_NAMES == 138);
    check("'Sting'", "「刺針」");
    check("'Andúril'", "「安都瑞爾」");
    check("'Glamdring'", "「格蘭瑞」");
    check("'The One Ring'", "「至尊魔戒」");
    check("of Galadriel", "凱蘭崔爾之物");
    check("of Morgoth", "魔苟斯之物");
    assert(angdroid_zh_tw_artifact_name("of speed") == NULL);
    assert(angdroid_zh_tw_artifact_name("unknown artifact") == NULL);
    assert(angdroid_zh_tw_artifact_name(NULL) == NULL);
    puts("PASS: 138 canonical artifact names, unknown and NULL fallback");
    return 0;
}
