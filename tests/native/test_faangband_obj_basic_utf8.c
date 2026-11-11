/* C99: FAangband early object_desc labels, money and narrow flavor copies. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/obj-desc-basic-utf8.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void basic_cases(void)
{
    char out[256];
    CHECK(angdroid_faangband_basic_item_text(
        out, sizeof(out), false, false, false) == strlen("(nothing)"));
    CHECK(strcmp(out, "(nothing)") == 0);
    CHECK(angdroid_faangband_basic_item_text(
        out, sizeof(out), false, true, true) == strlen("（空）"));
    CHECK(strcmp(out, "（空）") == 0);
    CHECK(angdroid_faangband_basic_item_text(
        out, sizeof(out), true, true, false) == strlen("an unknown item"));
    CHECK(strcmp(out, "an unknown item") == 0);
    CHECK(angdroid_faangband_basic_item_text(
        out, sizeof(out), true, false, false) == strlen("unknown item"));
    CHECK(strcmp(out, "unknown item") == 0);
    CHECK(angdroid_faangband_basic_item_text(
        out, sizeof(out), true, true, true) == strlen("未知物品"));
    CHECK(strcmp(out, "未知物品") == 0);

    CHECK(angdroid_faangband_copy_utf8(
        out, sizeof(out), "金幣😀") == strlen("金幣😀"));
    CHECK(strcmp(out, "金幣😀") == 0);
    /* This must copy the original flavor verbatim, not interpret '&'. */
    CHECK(angdroid_faangband_copy_utf8(
        out, sizeof(out), "& # ~ |") == strlen("& # ~ |"));
    CHECK(strcmp(out, "& # ~ |") == 0);
    CHECK(angdroid_faangband_copy_utf8(
        out, sizeof(out), NULL) == 0);
    CHECK(out[0] == '\0');

    CHECK(angdroid_faangband_money_text(out, sizeof(out),
        12, "gold", true, false) ==
        strlen("12 gold pieces worth of gold {ignore}"));
    CHECK(strcmp(out, "12 gold pieces worth of gold {ignore}") == 0);
    CHECK(angdroid_faangband_money_text(out, sizeof(out),
        12, "gold", true, true) ==
        strlen("價值 12 金幣的金幣 {忽略}"));
    CHECK(strcmp(out, "價值 12 金幣的金幣 {忽略}") == 0);
}

static void money_catalog(void)
{
    static const char *kinds[][2] = {
        { "copper", "銅幣" }, { "silver", "銀幣" },
        { "garnets", "石榴石" }, { "gold", "金幣" },
        { "opals", "蛋白石" }, { "sapphires", "藍寶石" },
        { "rubies", "紅寶石" }, { "diamonds", "鑽石" },
        { "emeralds", "祖母綠" }, { "mithril", "秘銀" },
        { "adamantite", "精金" }
    };
    char out[256];
    size_t i;
    CHECK(sizeof(kinds)/sizeof(kinds[0]) == 11);
    for (i = 0; i < sizeof(kinds)/sizeof(kinds[0]); ++i) {
        CHECK(angdroid_faangband_money_text(out, sizeof(out),
            31, kinds[i][0], true, true) > 0);
        CHECK(strstr(out, kinds[i][1]) != NULL);
        CHECK(strstr(out, " {忽略}") != NULL);
        CHECK(angdroid_faangband_money_text(out, sizeof(out),
            31, kinds[i][0], false, false) > 0);
        CHECK(strncmp(out, "31 gold pieces worth of ",
            strlen("31 gold pieces worth of ")) == 0);
    }
    CHECK(angdroid_faangband_money_text(out, sizeof(out),
        31, "mystery ore", false, true) > 0);
    CHECK(strcmp(out, "價值 31 金幣的mystery ore") == 0);
    CHECK(!angdroid_faangband_money_text(out, sizeof(out),
        31, NULL, false, true));
    CHECK(out[0] == '\0');
}

static void boundaries(void)
{
    const char *flavors[] = { "金幣😀", "天藍色", "Damascene Steel", "& # ~ |" };
    size_t i, cap;
    for (i = 0; i < sizeof(flavors)/sizeof(flavors[0]); ++i) {
        for (cap = 0; cap <= 64; ++cap) {
            char storage[68], *out = storage + 1;
            size_t len, pos = 0;
            memset(storage, '#', sizeof(storage));
            len = angdroid_faangband_copy_utf8(out, cap, flavors[i]);
            CHECK(storage[0] == '#');
            CHECK(storage[cap + 1] == '#');
            CHECK(storage[67] == '#');
            if (!cap) {
                CHECK(len == 0 && out[0] == '#');
                continue;
            }
            CHECK(len < cap);
            CHECK(out[len] == '\0');
            CHECK(strlen(out) == len);
            while (pos < len) {
                uint32_t scalar;
                size_t width = angdroid_utf8_next_scalar(
                    out + pos, &scalar);
                CHECK(width > 0 && scalar != 0xFFFDu);
                CHECK(pos + width <= len);
                pos += width;
            }
            CHECK(pos == len);
        }
    }
    for (cap = 0; cap <= 64; ++cap) {
        char storage[68], *out = storage + 1;
        size_t count;
        memset(storage, '#', sizeof(storage));
        count = angdroid_faangband_money_text(
            out, cap, 7, "adamantite", true, true);
        CHECK(storage[0] == '#');
        CHECK(storage[cap + 1] == '#');
        CHECK(storage[67] == '#');
        if (cap == 0) CHECK(out[0] == '#');
        else if (!count) CHECK(out[0] == '\0');
        else {
            CHECK(count < cap);
            CHECK(out[count] == '\0');
            CHECK(strcmp(out, "價值 7 金幣的精金 {忽略}") == 0);
        }
    }
}

int main(void)
{
    basic_cases();
    money_catalog();
    boundaries();
    printf("PASS: FAangband final basic/unknown/money UTF-8 (%lu checks)\n",
        checks);
    return 0;
}
