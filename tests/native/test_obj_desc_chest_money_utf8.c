/* Host C99 tests for chest status and money display without game engine. */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/obj-desc-chest-money-utf8.h"

static unsigned long checks = 0;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void examples(void)
{
    const char *chests[][2] = {
        {"locked", "已上鎖"}, {"unlocked", "已解鎖"},
        {"disarmed", "已解除陷阱"}, {"empty", "空箱"},
        {"multiple traps", "多重陷阱"}, {"gas trap", "氣體陷阱"},
        {"poison needle", "毒針"}, {"summoning runes", "召喚符文"},
        {"explosion device", "爆炸裝置"}, {"a strange rune", "奇異符文"},
        {"gleaming black runes", "閃亮的黑色符文"}
    };
    const char *money[][2] = {
        {"copper", "銅幣"}, {"silver", "銀幣"},
        {"garnets", "石榴石"}, {"gold", "金幣"}, {"opals", "蛋白石"},
        {"sapphires", "藍寶石"}, {"rubies", "紅寶石"},
        {"diamonds", "鑽石"}, {"emeralds", "祖母綠"},
        {"mithril", "秘銀"}, {"adamantite", "精金"}
    };
    char out[256];
    size_t i, end;
    for (i = 0; i < sizeof(chests) / sizeof(chests[0]); ++i) {
        strcpy(out, "寶箱");
        end = strlen(out);
        CHECK(strcmp(angdroid_zh_tw_chest_status_name(chests[i][0]),
            chests[i][1]) == 0);
        CHECK(angdroid_obj_desc_append_chest_status(out, sizeof(out),
            &end, chests[i][0], true));
        CHECK(strncmp(out, "寶箱（", strlen("寶箱（")) == 0);
        CHECK(end == strlen(out));
        CHECK(strcmp(out + end - strlen("）"), "）") == 0);
        strcpy(out, "Chest");
        end = strlen(out);
        CHECK(angdroid_obj_desc_append_chest_status(out, sizeof(out),
            &end, chests[i][0], false));
        CHECK(strncmp(out, "Chest (", strlen("Chest (")) == 0);
        CHECK(end == strlen(out) && out[end - 1] == ')');
    }
    for (i = 0; i < sizeof(money) / sizeof(money[0]); ++i) {
        CHECK(strcmp(angdroid_zh_tw_money_kind_name(money[i][0]),
            money[i][1]) == 0);
        CHECK(angdroid_obj_desc_money_text(out, sizeof(out), 27,
            money[i][0], false, true) > 0);
        CHECK(strncmp(out, "價值 27 金幣的", strlen("價值 27 金幣的")) == 0);
        CHECK(strstr(out, money[i][1]) != NULL);
    }
    CHECK(angdroid_obj_desc_money_text(out, sizeof(out), 12,
        "copper", true, false) == strlen("12 gold pieces worth of copper {ignore}"));
    CHECK(strcmp(out, "12 gold pieces worth of copper {ignore}") == 0);
    CHECK(angdroid_obj_desc_money_text(out, sizeof(out), -2147483647-1,
        "gold", false, false) == strlen("-2147483648 gold pieces worth of gold"));
    CHECK(strcmp(out, "-2147483648 gold pieces worth of gold") == 0);
    CHECK(angdroid_obj_desc_money_text(out, sizeof(out), INT_MAX,
        "mithril", true, true) > 0);
    CHECK(strcmp(out, "價值 2147483647 金幣的秘銀 {忽略}") == 0);
    CHECK(angdroid_obj_desc_money_text(out, sizeof(out), 1,
        "unknown-metal", false, true) > 0);
    CHECK(strcmp(out, "價值 1 金幣的unknown-metal") == 0);
    CHECK(!angdroid_obj_desc_money_text(out, sizeof(out), 1, NULL, false, true));
    CHECK(!angdroid_obj_desc_money_text(out, sizeof(out), 1, "", false, true));
    CHECK(!angdroid_obj_desc_money_text(NULL, sizeof(out), 1, "gold", false, true));
    CHECK(!angdroid_obj_desc_append_chest_status(out, sizeof(out),
        NULL, "locked", true));
}

static void capacities(void)
{
    const char *statuses[] = {
        "unlocked", "multiple traps", "gas trap", "a strange rune",
        "gleaming black runes", "未知狀態", "中文😀", ""
    };
    const char *kinds[] = {"copper", "garnets", "adamantite", "ruby😀", ""};
    size_t cap, i, lang, ignored;
    for (cap = 0; cap <= 140; ++cap) {
        for (i = 0; i < sizeof(statuses) / sizeof(statuses[0]); ++i)
            for (lang = 0; lang < 2; ++lang) {
                char mem[160], original[160], *out = mem + 1;
                size_t end = strlen("箱"), start = end;
                bool ok;
                memset(mem, '!', sizeof(mem));
                if (cap > end) {
                    memcpy(out, "箱", end + 1);
                }
                memcpy(original, mem, sizeof(mem));
                ok = angdroid_obj_desc_append_chest_status(
                    out, cap, &end, statuses[i], lang != 0);
                CHECK(mem[0] == '!' && mem[cap + 1] == '!' && mem[159] == '!');
                if (!ok) {
                    CHECK(end == start);
                    CHECK(memcmp(original, mem, sizeof(mem)) == 0);
                } else {
                    CHECK(end > start && end < cap && out[end] == 0);
                    CHECK(strlen(out) == end);
                    CHECK(strncmp(out, "箱", strlen("箱")) == 0);
                    CHECK(strcmp(out + end - (lang ? strlen("）") : 1),
                        lang ? "）" : ")") == 0);
                }
            }
        for (i = 0; i < sizeof(kinds) / sizeof(kinds[0]); ++i)
            for (lang = 0; lang < 2; ++lang)
                for (ignored = 0; ignored < 2; ++ignored) {
                    char mem[160], *out = mem + 1;
                    size_t count;
                    memset(mem, '!', sizeof(mem));
                    count = angdroid_obj_desc_money_text(out, cap,
                        12345, kinds[i], ignored != 0, lang != 0);
                    CHECK(mem[0] == '!' && mem[cap + 1] == '!' && mem[159] == '!');
                    if (!cap) CHECK(mem[1] == '!');
                    else if (count) {
                        CHECK(count < cap && out[count] == '\0');
                        CHECK(strlen(out) == count);
                        CHECK(strncmp(out, lang ? "價值 " : "12345 gold pieces worth of ",
                            lang ? strlen("價值 ") : strlen("12345 gold pieces worth of ")) == 0);
                        if (ignored) CHECK(strcmp(out + count -
                            (lang ? strlen(" {忽略}") : strlen(" {ignore}")),
                            lang ? " {忽略}" : " {ignore}") == 0);
                    } else CHECK(out[0] == '\0');
                }
    }
}

int main(void)
{
    examples();
    capacities();
    printf("PASS: chest and money UTF-8 descriptions (%lu assertions)\n", checks);
    return 0;
}
