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
    assert(ANGDROID_ZH_TW_ARTIFACT_LORE == 138);
    assert(ANGDROID_ZH_TW_EGO_LORE == 5);
    known_artifact("of Galadriel", "埃蘭迪爾");
    known_artifact("of Thráin", "山之心");
    known_artifact("'Angrist'", "精靈寶鑽");
    known_artifact("'Sting'", "比爾博");
    known_artifact("'Andúril'", "西方之焰");
    known_artifact("'Glamdring'", "痛擊者");
    known_artifact("of Olórin", "甘道夫");
    known_artifact("of Aulë", "奧力");
    known_artifact("of Dor-Lómin", "龍盔");
    known_artifact("'Narya'", "火之戒");
    known_artifact("'The One Ring'", "至尊魔戒");
    known_artifact("'Grond'", "巨錘");
    known_artifact("of Morgoth", "精靈寶鑽");
    known_ego("of Accuracy", "命中");
    known_ego("of Power", "傷害");
    known_ego("of Wounding", "彈藥");
    known_ego("of Slaying", "命中");
    known_ego("of Craftsmanship", "盔甲");
    /* Unknown keys must fall back to canonical English story paragraphs. */
    assert(angdroid_zh_tw_artifact_flavor("unmapped artifact") == NULL);
    assert(angdroid_zh_tw_artifact_flavor("unknown") == NULL);
    assert(angdroid_zh_tw_artifact_flavor(NULL) == NULL);
    assert(angdroid_zh_tw_ego_flavor("of Free Action") == NULL);
    assert(angdroid_zh_tw_ego_flavor(NULL) == NULL);
    puts("PASS: 138 canonical artifact stories, all 5 ego stories, fallbacks");
    return 0;
}
