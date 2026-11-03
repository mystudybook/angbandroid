/* Exact original Angband item detail names; never mutate game data. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-object-info.h"
int main(void)
{
    assert(strcmp(angdroid_zh_tw_object_term("acid", 1), "強酸") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("lightning", 1), "閃電") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("disenchantment", 1), "消魔") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("strength", 1), "力量") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("searching skill", 1), "搜索能力") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("fear", 1), "恐懼") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("undead", 1), "不死生物") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("acid", 0), "acid") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("not-mapped", 1), "not-mapped") == 0);
    assert(angdroid_zh_tw_object_term(NULL, 1) == NULL);
    assert(strcmp(angdroid_zh_tw_object_term("rubble", 1), "碎石") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("magma veins", 1),
                  "岩漿礦脈") == 0);
    assert(strcmp(angdroid_zh_tw_object_term("granite", 0),
                  "granite") == 0);
    puts("PASS: object info original Angband term labels");
    return 0;
}
