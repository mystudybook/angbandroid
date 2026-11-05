#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-gear-messages.h"
int main(void) {
    assert(strcmp(angdroid_gear_message("Mushrooms for breakfast!", 1), "蘑菇早餐真美味！") == 0);
    assert(strcmp(angdroid_gear_message("Mushrooms for breakfast!", 0), "Mushrooms for breakfast!") == 0);
    assert(strcmp(angdroid_gear_message("Oops! It feels deathly cold!", 1), "糟糕！裝備傳來一股令人發寒的冰冷感！") == 0);
    assert(strcmp(angdroid_gear_message("Oops! It feels deathly cold!", 0), "Oops! It feels deathly cold!") == 0);
    assert(strcmp(angdroid_gear_message("You combine some items in your pack.", 1), "你把背包中的部分物品合併了。") == 0);
    assert(strcmp(angdroid_gear_message("You combine some items in your pack.", 0), "You combine some items in your pack.") == 0);
    assert(strcmp(angdroid_gear_message("Your pack overflows!", 1), "你的背包裝不下了！") == 0);
    assert(strcmp(angdroid_gear_message("Your pack overflows!", 0), "Your pack overflows!") == 0);
    const char *unknown = "Other gear message";
    assert(angdroid_gear_message(unknown, 1) == unknown);
    assert(angdroid_gear_message(NULL, 1) == NULL);
    puts("PASS: native gear messages and English fallback");
    return 0;
}
