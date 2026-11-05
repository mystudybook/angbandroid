/* Identified Angband object kind lore: UI text, not item identity. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-kind-lore.h"
static void assert_story(const char *english, const char *part)
{
    const char *zh = angdroid_zh_tw_kind_flavor(english);
    assert(zh != NULL && strstr(zh, part) != NULL);
}
int main(void)
{
    assert(ANGDROID_ZH_TW_KIND_LORE == 141);
    assert_story("& Flask~ of oil", "提燈");
    assert_story("& Honey-cake~", "比翁");
    assert_story("& Piece~ of Elvish Waybread", "蘭巴斯");
    assert_story("& Dagger~", "突刺");
    assert_story("& Quarterstaff~", "橡木棍");
    assert_story("& Mithril Arrow~", "秘銀箭");
    assert_story("& Jewel Encrusted Crown~", "寶石");
    assert_story("& Ethereal Cloak~", "毫無重量");
    assert_story("Slaying", "傷害");
    assert_story("Power Dragon Scale Mail~", "以太力量");
    assert(angdroid_zh_tw_kind_flavor("& Unknown Thing~") == NULL);
    assert(angdroid_zh_tw_kind_flavor(NULL) == NULL);
    puts("PASS: 141 canonical identified item stories and English fallback");
    return 0;
}
