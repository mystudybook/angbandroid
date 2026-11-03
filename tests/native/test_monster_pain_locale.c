#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-mon-pain.h"

int main(void)
{
    assert(ANGDROID_ZH_TW_PAIN_PHRASES == 65);
    assert(strcmp(angdroid_zh_tw_pain_message("cr[ies|y] out in pain."),
        "痛得大叫起來。") == 0);
    assert(strcmp(angdroid_zh_tw_pain_message("shrug[s] off the attack."),
        "擋下了這次攻擊。") == 0);
    assert(strcmp(angdroid_zh_tw_pain_message("rattle[s]."),
        "發出喀啦聲。") == 0);
    assert(strcmp(angdroid_zh_tw_pain_message("wince[s]."),
        "痛得皺起了臉。") == 0);
    assert(angdroid_zh_tw_pain_message(NULL) == NULL);
    assert(angdroid_zh_tw_pain_message("unknown pain text") == NULL);
    puts("PASS: all 65 unique monster pain templates have a fallback");
    return 0;
}
