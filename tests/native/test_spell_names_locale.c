#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-names.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"
int main(void)
{
    assert(strcmp(angdroid_zh_tw_spell_name("Magic Missile", 1),
                  "魔法飛彈") == 0);
    assert(strcmp(angdroid_zh_tw_spell_name("Find Traps, Doors & Stairs", 1),
                  "偵測陷阱、門與樓梯") == 0);
    assert(strcmp(angdroid_zh_tw_spell_name("Howl of the Damned", 1),
                  "受詛咒者的嚎叫") == 0);
    assert(strcmp(angdroid_zh_tw_spell_name("Word of Recall", 1),
                  "召回術") == 0);
    assert(strcmp(angdroid_zh_tw_spell_name("Magic Missile", 0),
                  "Magic Missile") == 0);
    assert(strcmp(angdroid_zh_tw_spell_name("not a canonical spell", 1),
                  "not a canonical spell") == 0);
    assert(angdroid_zh_tw_spell_name(NULL, 1) == NULL);
    assert(angdroid_utf8_display_columns("偵測陷阱、門與樓梯") == 20);
    puts("PASS: source Angband spell-name display dictionary");
    return 0;
}
