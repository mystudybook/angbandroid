/* Original Angband artifact/ego story descriptions are UI-only.
 * Both dictionaries use exact canonical names; NULL means English fallback.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-artifact-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-ego-lore.h"

static void known_artifact(const char *english, const char *fragment)
{
    const char *zh = angdroid_zh_tw_artifact_flavor(english);
    assert(zh != NULL && strstr(zh, fragment) != NULL);
}

static void known_ego(const char *english, const char *fragment)
{
    const char *zh = angdroid_zh_tw_ego_flavor(english);
    assert(zh != NULL && strstr(zh, fragment) != NULL);
}

int main(void)
{
    assert(ANGDROID_ZH_TW_ARTIFACT_LORE == 43);
    assert(ANGDROID_ZH_TW_EGO_LORE == 5);
    known_artifact("of Galadriel", "埃蘭迪爾");
    known_artifact("of Thráin", "山之心");
    known_artifact("'Angrist'", "精靈寶鑽");
    known_artifact("'Sting'", "比爾博");
    known_artifact("'Andúril'", "西方之焰");
    known_artifact("'Glamdring'", "格蘭瑞");
    known_artifact("of Olórin", "甘道夫");
    known_ego("of Accuracy", "命中");
    known_ego("of Power", "傷害");
    known_ego("of Wounding", "彈藥");
    known_ego("of Slaying", "命中");
    known_ego("of Craftsmanship", "盔甲");
    /* An artifact outside the translated batch MUST retain English lore. */
    assert(angdroid_zh_tw_artifact_flavor("of Aulë") == NULL);
    assert(angdroid_zh_tw_artifact_flavor("unknown") == NULL);
    assert(angdroid_zh_tw_artifact_flavor(NULL) == NULL);
    assert(angdroid_zh_tw_ego_flavor("of Free Action") == NULL);
    assert(angdroid_zh_tw_ego_flavor(NULL) == NULL);
    puts("PASS: 43 known artifact stories, all 5 ego stories, exact fallbacks");
    return 0;
}
