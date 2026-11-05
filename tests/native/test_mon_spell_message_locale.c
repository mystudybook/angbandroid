/* Run all 226 original monster spell templates through the locale lookup.
 * Verify canonical dynamic tags and English/NULL/unknown fallback.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-text.h"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-lash.h"
static const struct { const char *en; const char *zh; } cases[] = {
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-0.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-1.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-2.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-3.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-alt.inc"
};
int main(void)
{
    static const char *required_tags[] = {
        "{name}", "{target}", "{type}", "{oftype}"
    };
    size_t i, j;
    assert(sizeof(cases) / sizeof(cases[0]) == 226);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        assert(strcmp(angdroid_mon_spell_text(cases[i].en, 1),
            cases[i].zh) == 0);
        assert(angdroid_mon_spell_text(cases[i].en, 0) == cases[i].en);
        assert(strchr(cases[i].zh, '%') == NULL);
        for (j = 0; j < sizeof(required_tags) / sizeof(required_tags[0]); ++j)
            assert((strstr(cases[i].en, required_tags[j]) != NULL) ==
                   (strstr(cases[i].zh, required_tags[j]) != NULL));
        assert(strstr(cases[i].zh, "{pronoun}") == NULL);
    }

    assert(strcmp(angdroid_mon_spell_lash("fire", 1), "火焰") == 0);
    assert(strcmp(angdroid_mon_spell_lash("holy power", 1),
        "神聖力量") == 0);
    assert(strcmp(angdroid_mon_spell_lash("ice", 1), "ice") == 0);
    assert(strcmp(angdroid_mon_spell_lash("fire", 0), "fire") == 0);
    {
        const char *future = "A new monster spell.";
        assert(angdroid_mon_spell_text(future, 1) == future);
        assert(angdroid_mon_spell_text(NULL, 1) == NULL);
        assert(angdroid_mon_spell_lash(NULL, 1) == NULL);
    }
    puts("PASS: 226 source monster spells, dynamic tags and English fallback");
    return 0;
}
