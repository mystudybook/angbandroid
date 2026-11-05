/* Ego suffix localization is exact-match, display-only and reversible. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ego-names.h"
static void check(const char *english, const char *chinese)
{
    const char *display = angdroid_zh_tw_ego_name(english);
    assert(display != NULL);
    assert(strcmp(display, chinese) == 0);
}
int main(void)
{
    assert(ANGDROID_ZH_TW_EGO_NAMES == 79);
    check("of Speed", "加速");
    check("of *Slay Dragon*", "強效屠龍");
    check("of Telepathy", "心靈感應");
    check("of Elvenkind", "精靈族");
    check("(Holy Avenger)", "（神聖復仇者）");
    check("of Resist Fire", "抗火");
    check("of Craftsmanship", "精工");
    assert(angdroid_zh_tw_ego_name("of speed") == NULL);
    assert(angdroid_zh_tw_ego_name("of Unknown") == NULL);
    assert(angdroid_zh_tw_ego_name(NULL) == NULL);
    puts("PASS: 79 canonical ego suffixes, fallback, and exact case");
    return 0;
}
