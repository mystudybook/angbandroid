/* FAangband player-facing projection descriptions and monster lash text. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-projection-terms.h"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-lash.h"
static unsigned checks;
#define CHECK(cond) do { assert(cond); ++checks; } while (0)
static void verify(const char *english, const char *traditional)
{
    const char *actual = angdroid_projection_description(english, 1);
    CHECK(actual != NULL);
    CHECK(strcmp(actual, traditional) == 0);
    CHECK(angdroid_projection_description(english, 0) == english);
}
int main(void)
{
    static const struct { const char *en, *zh; } projection[] = {
        { "frost", "冰霜" },
        { "poison gas", "毒氣" },
        { "storm", "暴風" },
        { "dragonfire", "龍焰" },
        { "hellfire", "地獄火" },
        { "meteors", "流星群" },
        { "meteor", "流星" },
        { "magic missiles", "魔法飛彈" },
        { "magical energy", "魔法能量" },
        { "the elements", "元素之力" },
        { "holy power", "神聖力量" },
        { "arrows", "箭矢" },
        { "weak light", "微光" },
        { "wind", "狂風" },
        { "damages demons", "傷害惡魔" },
        { "damages dragons", "傷害龍族" },
        { "attempts to put any monster to sleep", "試圖使任何怪物沉睡" },
        { "attempts to hold undead still", "試圖使不死生物無法行動" },
    };
    static const struct { const char *en, *zh; } lash[] = {
        { "dragonfire", "龍焰" },
        { "hellfire", "地獄火" },
    };
    size_t i;
    CHECK(sizeof(projection) / sizeof(projection[0]) == 18);
    CHECK(sizeof(lash) / sizeof(lash[0]) == 2);
    for (i = 0; i < sizeof(projection) / sizeof(projection[0]); ++i)
        verify(projection[i].en, projection[i].zh);
    for (i = 0; i < sizeof(lash) / sizeof(lash[0]); ++i) {
        CHECK(strcmp(angdroid_mon_spell_lash(lash[i].en, 1),
            lash[i].zh) == 0);
        CHECK(angdroid_mon_spell_lash(lash[i].en, 0) == lash[i].en);
    }
    /* Keep prior standard Angband translations unmodified. */
    verify("acid", "酸液");
    verify("an arrow", "箭矢");
    verify("damages undead", "傷害不死生物");
    CHECK(strcmp(angdroid_mon_spell_lash("frost", 1), "冰霜") == 0);
    CHECK(angdroid_projection_description(NULL, 1) == NULL);
    CHECK(angdroid_mon_spell_lash(NULL, 1) == NULL);
    CHECK(strcmp(angdroid_projection_description("unknown effect", 1),
        "unknown effect") == 0);
    CHECK(strcmp(angdroid_mon_spell_lash("unknown lash", 1),
        "unknown lash") == 0);
    printf("PASS: FAangband projections and lash descriptions (%u assertions)\n",
        checks);
    return 0;
}
