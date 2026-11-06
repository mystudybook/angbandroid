/* Shop title and !key / !* inscription safety prompt display-only C99. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-store-names.h"
#include "../../app/src/main/cpp/angband/zh-tw-inscription-confirm.h"

static int checked = 0;
static void shop(const char *english, const char *chinese)
{
    assert(strcmp(angdroid_store_display_name(english, true), chinese) == 0);
    assert(strcmp(angdroid_store_display_name(english, false), english) == 0);
    checked += 2;
}
static void verb(const char *english, const char *chinese)
{
    char prompt[160], expected[160];
    assert(angdroid_inscription_confirm_prefix(
        prompt, sizeof(prompt), english, true));
    assert(snprintf(expected, sizeof(expected), "確定要%s", chinese) > 0);
    assert(strcmp(prompt, expected) == 0);
    assert(angdroid_inscription_confirm_prefix(
        prompt, sizeof(prompt), english, false));
    assert(snprintf(expected, sizeof(expected), "Really %s", english) > 0);
    assert(strcmp(prompt, expected) == 0);
    checked += 4;
}
int main(void)
{
    char prompt[160], tiny[3] = "AB";
    const char *unknown = "a custom mod shop";

    shop("General Store", "雜貨店");
    shop("Armoury", "防具店");
    shop("Weapon Smiths", "武器店");
    shop("Bookseller", "書店");
    shop("Alchemy Shop", "鍊金商店");
    shop("Magic Shop", "魔法商店");
    shop("Black Market", "黑市");
    shop("Home", "家");
    assert(angdroid_store_display_name(unknown, true) == unknown);
    assert(angdroid_store_display_name(NULL, true) == NULL);

    verb("wear or wield", "裝備");
    verb("take off", "卸下");
    verb("drop", "丟棄");
    verb("eat", "食用");
    verb("quaff", "飲用");
    verb("zap", "啟動魔棒");
    verb("use", "使用");
    verb("aim", "瞄準魔杖");
    verb("read", "閱讀");
    verb("activate", "啟動");
    verb("refuel with", "補充燃料");
    verb("fire", "發射");
    verb("throw", "投擲");
    verb("inscribe", "刻字");
    verb("un-inscribe", "清除刻字");
    verb("study", "研讀");
    verb("cast", "施法");
    verb("sell", "出售");
    verb("stash", "存放");
    verb("buy", "購買");
    verb("retrieve", "取出");

    assert(angdroid_inscription_confirm_prefix(
        prompt, sizeof(prompt), "custom mod verb", true));
    assert(strcmp(prompt, "Really custom mod verb") == 0);
    assert(angdroid_inscription_confirm_prefix(
        prompt, sizeof(prompt), NULL, true));
    assert(strcmp(prompt, "Really do that with") == 0);
    assert(!angdroid_inscription_confirm_prefix(
        tiny, sizeof(tiny), "wear or wield", true));
    assert(tiny[0] == '\0');
    assert(!angdroid_inscription_confirm_prefix(
        NULL, sizeof(prompt), "wear or wield", true));
    assert(angdroid_inscription_confirm_prefix(
        prompt, sizeof(prompt), "wear or wield", true));
    assert(strstr(prompt, "裝備") != NULL);
    printf("PASS: %d bilingual shop and safety-prompt checks\n", checked);
    return 0;
}
