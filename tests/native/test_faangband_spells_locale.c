/* Regression on the actual supplemental FAangband spell display header. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-faangband-spells.h"

static unsigned long checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static void check_spell(const char *name, const char *english,
    const char *zh_name, const char *zh_description)
{
    const char *actual_name = angdroid_zh_tw_faangband_spell_name(name);
    const char *actual_description =
        angdroid_zh_tw_faangband_spell_description(name, english);
    char altered[256];
    CHECK(actual_name != NULL);
    CHECK(actual_description != NULL);
    CHECK(strcmp(actual_name, zh_name) == 0);
    CHECK(strcmp(actual_description, zh_description) == 0);
    CHECK(strlen(actual_name) == strlen(zh_name));
    CHECK(strlen(actual_description) == strlen(zh_description));
    CHECK(strlen(english) < sizeof(altered));
    strcpy(altered, english);
    altered[0] = altered[0] == 'X' ? 'Y' : 'X';
    CHECK(angdroid_zh_tw_faangband_spell_description(name, altered) == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_description(name, "") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_description(name, NULL) == NULL);
}

int main(void)
{
    CHECK(ANGDROID_ZH_TW_FAANGBAND_SPELL_COUNT == 2);
    check_spell("Timeless Blink",
        "Teleports you to a square of your choice within your line of sight.  This spell takes no time.",
        "無時瞬移", "傳送至視線內你所選擇的位置。此法術不消耗時間。");
    check_spell("Fume of Thangorodrim",
        "Darkens, maps out, and detects all objects on the complete current dungeon level.",
        "桑戈洛錐姆之煙", "使目前整個地城樓層陷入黑暗、繪製地圖，並偵測該樓層的所有物品。");
    CHECK(angdroid_zh_tw_faangband_spell_name("Timeless Blink ") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_name("Fume of Mordor") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_name("Magic Missile") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_name(NULL) == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_description(NULL, "") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_description("Unknown", "") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_description(
        "Timeless Blink", "Darkens, maps out, and detects all objects on the complete current dungeon level.") == NULL);
    CHECK(angdroid_zh_tw_faangband_spell_description(
        "Fume of Thangorodrim", "Teleports you to a square of your choice within your line of sight.  This spell takes no time.") == NULL);
    printf("PASS: two FAangband-only translated spells (%lu assertions)\n", checks);
    return 0;
}
