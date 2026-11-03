#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-class-titles.h"
int main(void)
{
    assert(strcmp(angdroid_zh_tw_class_title("Rookie", 1), "新兵") == 0);
    assert(strcmp(angdroid_zh_tw_class_title("Arch-Mage", 1), "大法師") == 0);
    assert(strcmp(angdroid_zh_tw_class_title("Ranger Lord", 1), "遊俠領主") == 0);
    assert(strcmp(angdroid_zh_tw_class_title("Night Lord", 1), "暗夜領主") == 0);
    assert(strcmp(angdroid_zh_tw_class_title("Arch-Mage", 0), "Arch-Mage") == 0);
    assert(strcmp(angdroid_zh_tw_class_title("***WINNER***", 1), "***WINNER***") == 0);
    assert(angdroid_zh_tw_class_title(NULL, 1) == NULL);
    puts("PASS: original Angband class progression titles");
    return 0;
}
