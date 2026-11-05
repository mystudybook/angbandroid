/* Known shape-effect descriptors exactly match gamedata source keys. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-shape-effects.h"

int main(void)
{
    static const struct {
        const char *en;
        const char *zh;
    } cases[] = {
        { "poisoning", "中毒" },
        { "stunning", "暈眩" },
        { "fear", "恐懼" },
        { "berserk rage", "狂暴" },
        { "temporary hitpoint siphoning", "暫時吸取生命" },
        { "causes monsters to flee", "使視線內怪物逃跑" },
    };
    size_t i;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        assert(strcmp(angdroid_shape_effect_term(cases[i].en),
            cases[i].zh) == 0);
        assert(angdroid_shape_effect_known(cases[i].en));
    }
    assert(strcmp(angdroid_shape_effect_term("new future effect"),
        "new future effect") == 0);
    assert(!angdroid_shape_effect_known("new future effect"));
    assert(angdroid_shape_effect_term(NULL) == NULL);
    assert(!angdroid_shape_effect_known(NULL));
    printf("PASS: six shape-change effect terms and safe fallback\n");
    return 0;
}
