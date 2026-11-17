/* C99: exercise FAangband equipment/status output without Android/JNI. */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/obj-desc-details-utf8.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void status_dictionary(void)
{
    static const struct { const char *english, *zh; } entries[] = {
        { "empty", "耗盡" }, { "tried", "已嘗試" },
        { "cursed", "受詛咒" }, { "ignore", "忽略" },
        { "unseen", "未見過" }, { "??", "??" }
    };
    size_t i;
    for (i = 0; i < sizeof(entries)/sizeof(entries[0]); ++i) {
        CHECK(strcmp(angdroid_faangband_status_ui_name(
            entries[i].english, true), entries[i].zh) == 0);
        CHECK(strcmp(angdroid_faangband_status_ui_name(
            entries[i].english, false), entries[i].english) == 0);
    }
    CHECK(angdroid_faangband_status_ui_name(NULL, true) == NULL);
    CHECK(strcmp(angdroid_faangband_status_ui_name(
        "玩家文字", true), "玩家文字") == 0);
}

static void annotated_group(void)
{
    const char *entries[] = { "玩家註記「測試」", "已嘗試", "受詛咒", "??" };
    const char *english[] = { "my note", "tried", "cursed" };
    const char *bad[] = { "OK", NULL };
    const char invalid[] = { (char)0xE4, 'X', 0 };
    const char *bad_utf8[] = { invalid };
    char out[256] = "長劍";
    size_t end = strlen(out);
    CHECK(angdroid_faangband_append_inscription(
        out, sizeof(out), &end, entries, 4));
    CHECK(strcmp(out, "長劍 {玩家註記「測試」, 已嘗試, 受詛咒, ??}") == 0);
    CHECK(end == strlen(out));

    strcpy(out, "Sword");
    end = strlen(out);
    CHECK(angdroid_faangband_append_inscription(
        out, sizeof(out), &end, english, 3));
    CHECK(strcmp(out, "Sword {my note, tried, cursed}") == 0);
    CHECK(!angdroid_faangband_append_inscription(
        out, sizeof(out), &end, bad, 2));
    CHECK(strcmp(out, "Sword {my note, tried, cursed}") == 0);
    CHECK(!angdroid_faangband_append_inscription(
        out, sizeof(out), &end, bad_utf8, 1));
    CHECK(strcmp(out, "Sword {my note, tried, cursed}") == 0);
    CHECK(!angdroid_faangband_append_inscription(
        out, sizeof(out), &end, entries, 0));
    CHECK(!angdroid_faangband_append_inscription(
        NULL, sizeof(out), &end, entries, 4));
    CHECK(!angdroid_faangband_append_inscription(
        out, 0, &end, entries, 4));
}

static void annotation_capacities(void)
{
    static const char *labels[] = { "旅途筆記", "受詛咒" };
    const char *whole = " {旅途筆記, 受詛咒}";
    size_t cap;
    for (cap = 0; cap <= 96; ++cap) {
        char guard[130];
        char *out = guard + 1;
        size_t end = 0;
        bool inserted;
        memset(guard, 'Z', sizeof(guard));
        if (cap) out[0] = '\0';
        inserted = angdroid_faangband_append_inscription(
            out, cap, &end, labels, 2);
        CHECK(guard[0] == 'Z');
        CHECK(guard[cap + 1] == 'Z');
        CHECK(guard[129] == 'Z');
        CHECK(inserted == (cap > strlen(whole)));
        if (inserted) {
            CHECK(strcmp(out, whole) == 0);
            CHECK(strlen(out) == end);
        } else {
            CHECK(end == 0);
            if (cap) CHECK(out[0] == '\0');
            else CHECK(out[0] == 'Z');
        }
    }
}

static void modifiers_and_combat(void)
{
    const int mods[] = { 3, -2, 2147483647 };
    int long_mods[128];
    char out[256];
    size_t end;
    size_t i, cap;

    strcpy(out, "劍");
    end = strlen(out);
    CHECK(angdroid_faangband_append_modifiers(
        out, sizeof(out), &end, mods, 3));
    CHECK(strcmp(out, "劍 <+3, -2, +2147483647>") == 0);
    CHECK(angdroid_faangband_append_detailf(
        out, sizeof(out), &end, " (%+d,%+d)", 5, -3));
    CHECK(strcmp(out, "劍 <+3, -2, +2147483647> (+5,-3)") == 0);
    CHECK(angdroid_faangband_append_detailf(
        out, sizeof(out), &end, " [%d,%+d]", 12, 4));
    CHECK(strcmp(out, "劍 <+3, -2, +2147483647> (+5,-3) [12,+4]") == 0);
    CHECK(angdroid_faangband_append_detailf(
        out, sizeof(out), &end, " (%d charging)", 3));
    CHECK(angdroid_faangband_append_detailf(
        out, sizeof(out), &end, " (%d turns)", 50));
    CHECK(angdroid_faangband_append_detailf(
        out, sizeof(out), &end, " (剩餘%d回合)", 9));
    CHECK(strstr(out, "(剩餘9回合)") != NULL);
    CHECK(end == strlen(out));

    for (i = 0; i < 128; ++i) long_mods[i] = 2147483647;
    strcpy(out, "劍");
    end = strlen(out);
    CHECK(!angdroid_faangband_append_modifiers(
        out, sizeof(out), &end, long_mods, 128));
    CHECK(strcmp(out, "劍") == 0);
    CHECK(!angdroid_faangband_append_modifiers(
        out, sizeof(out), &end, mods, 0));

    for (cap = 0; cap <= 80; ++cap) {
        char guard[100];
        char *dest = guard + 1;
        size_t used = 0;
        bool inserted;
        memset(guard, 'X', sizeof(guard));
        if (cap) dest[0] = '\0';
        inserted = angdroid_faangband_append_detailf(
            dest, cap, &used, " [%d,%+d]", 12, 4);
        CHECK(guard[0] == 'X');
        CHECK(guard[cap + 1] == 'X');
        CHECK(inserted == (cap > strlen(" [12,+4]")));
        if (inserted) {
            CHECK(strcmp(dest, " [12,+4]") == 0);
            CHECK(used == strlen(dest));
        } else {
            CHECK(used == 0);
            if (cap) CHECK(dest[0] == '\0');
        }
    }
    CHECK(!angdroid_faangband_append_detailf(
        out, sizeof(out), &end, "%0300d", 7));
    CHECK(strcmp(out, "劍") == 0);
    CHECK(!angdroid_faangband_append_detailf(
        NULL, 20, &end, "%d", 2));
}

int main(void)
{
    status_dictionary();
    annotated_group();
    annotation_capacities();
    modifiers_and_combat();
    printf("PASS: FAangband UTF-8 equipment/status groups (%lu checks)\n", checks);
    return 0;
}
