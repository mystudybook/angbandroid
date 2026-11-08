/* Display-only monster list count / unique labels and UTF-8 edge cases. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-monster-list-render.h"
#include "../../app/src/main/cpp/angband/zh-tw-monsters.h"
#include "../../app/src/main/cpp/angband/zh-tw-unique-monsters.h"

int main(void)
{
    char out[64];
    char broken[] = { (char)0xE4, 'X', '\0' };
    const char *regular = angdroid_zh_tw_monster_list_name("kobold");
    const char *unique = angdroid_zh_tw_unique_monster_name(
        "Morgoth, Lord of Darkness");
    assert(regular && strcmp(regular, "狗頭人") == 0);
    assert(unique && strcmp(unique, "魔苟斯，黑暗之王") == 0);
    assert(angdroid_zh_tw_monster_list_name("unknown monster") == NULL);
    assert(angdroid_zh_tw_unique_monster_name("unknown unique") == NULL);
    assert(angdroid_zh_tw_monster_list_copy(
        out, sizeof(out), regular, 0, 2) == strlen("  2 狗頭人"));
    assert(strcmp(out, "  2 狗頭人") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, sizeof(out), unique, 1, 99) == strlen("[U] 魔苟斯，黑暗之王"));
    assert(strcmp(out, "[U] 魔苟斯，黑暗之王") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, 8, regular, 0, 2) == strlen("  2 狗"));
    assert(strcmp(out, "  2 狗") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, 7, regular, 0, 2) == strlen("  2 "));
    assert(strcmp(out, "  2 ") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, 4, regular, 0, 2) == 0);
    assert(strcmp(out, "") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, 8, unique, 1, 0) == strlen("[U] 魔"));
    assert(strcmp(out, "[U] 魔") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, sizeof(out), broken, 0, 1) == strlen("  1 ?X"));
    assert(strcmp(out, "  1 ?X") == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        out, sizeof(out), NULL, 0, 1) == 0 && !out[0]);
    assert(angdroid_zh_tw_monster_list_copy(
        out, 0, regular, 0, 1) == 0);
    assert(angdroid_zh_tw_monster_list_copy(
        NULL, sizeof(out), regular, 0, 1) == 0);
    puts("PASS: Chinese monster list labels, small buffers, [U], count and fallback");
    return 0;
}
