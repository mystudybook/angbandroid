/* Standalone C99 localization smoke test (no Android SDK required). */
#include <assert.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-activations.h"
#include "../../app/src/main/cpp/angband/zh-tw-item-abilities.h"
#include "../../app/src/main/cpp/angband/zh-tw-curse.h"

int main(void)
{
    const char *known = angdroid_zh_tw_activation_description("CURE_POISON", 1);
    assert(known != NULL && strcmp(known, "解除中毒") == 0);
    assert(angdroid_zh_tw_activation_description("CURE_POISON", 0) == NULL);
    assert(angdroid_zh_tw_activation_description("UNLISTED_EFFECT", 1) == NULL);
    assert(angdroid_zh_tw_activation_description(NULL, 1) == NULL);
    assert(strcmp(angdroid_zh_tw_item_ability("Grants telepathy", 1), "賦予心靈感應能力") == 0);
    assert(strcmp(angdroid_zh_tw_item_ability("Grants telepathy", 0), "Grants telepathy") == 0);
    assert(strcmp(angdroid_zh_tw_item_ability("Unknown property", 1), "Unknown property") == 0);
    assert(strcmp(angdroid_curse_description("makes you frail", 1), "使你的身體變得虛弱") == 0);
    assert(strcmp(angdroid_curse_description("makes you frail", 0), "makes you frail") == 0);
    return 0;
}
