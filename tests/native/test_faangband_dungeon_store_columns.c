/* FAangband dungeon object-list + shop display on narrow CJK terminals.
 * Reuse the production display-only headers, not duplicate formatting rules.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/object-list-name-display.h"
#include "../../app/src/main/cpp/angband/store-column-layout.h"
#include "../../app/src/main/cpp/angband/store-detail-text.h"
#include "../../app/src/main/cpp/angband/menu-text-fit.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void check_scalars(const char *s)
{
    size_t offset = 0, len = strlen(s);
    while (offset < len) {
        uint32_t cp;
        size_t n = angdroid_utf8_next_scalar(s + offset, &cp);
        CHECK(n > 0 && n <= len - offset && cp != 0xFFFDu);
        offset += n;
    }
    CHECK(offset == len);
}

static void names_and_prefixes(void)
{
    char b[256];
    CHECK(angdroid_object_list_name_copy(
        b, sizeof(b), "2 長劍", 1) == strlen("  2 長劍"));
    CHECK(strcmp(b, "  2 長劍") == 0);
    CHECK(angdroid_object_list_name_copy(
        b, sizeof(b), "a Scroll", 1) == strlen("  a Scroll"));
    CHECK(strcmp(b, "  a Scroll") == 0);
    CHECK(angdroid_object_list_name_copy(
        b, sizeof(b), "治療藥水", 1) == strlen("    治療藥水"));
    CHECK(strcmp(b, "    治療藥水") == 0);
    CHECK(angdroid_object_list_name_copy(
        b, sizeof(b), "銀色 魔杖", 1) == strlen("    銀色 魔杖"));
    CHECK(strcmp(b, "    銀色 魔杖") == 0);
    CHECK(angdroid_object_list_name_copy(
        b, sizeof(b), "1 治療藥水", 0) == strlen("    1 治療藥水"));
    CHECK(strcmp(b, "    1 治療藥水") == 0);
    CHECK(angdroid_object_list_name_copy(b, sizeof(b), NULL, 1) == 0);
    CHECK(b[0] == '\0');
}

static void dungeon_tiny_columns(void)
{
    const char *original = "題為「Rerum Lacrimosa」的卷軸";
    size_t cap;
    CHECK(angdroid_utf8_display_columns("題為「術法」的卷軸") == 18);
    for (cap = 0; cap <= 90; ++cap) {
        char guard[96], *dst = guard + 1;
        size_t n;
        memset(guard, 'X', sizeof(guard));
        n = angdroid_object_list_name_copy(dst, cap, original, 1);
        CHECK(guard[0] == 'X');
        CHECK(guard[cap + 1] == 'X');
        CHECK(guard[95] == 'X');
        if (!cap) {
            CHECK(n == 0 && dst[0] == 'X');
            continue;
        }
        CHECK(n == strlen(dst));
        CHECK(n < cap);
        check_scalars(dst);
        if (cap > 5) {
            CHECK(strncmp(dst, "    ", 4) == 0);
        }
    }
    CHECK(strcmp(original, "題為「Rerum Lacrimosa」的卷軸") == 0);
}

static void shop_columns(void)
{
    int width, home;
    for (home = 0; home <= 1; ++home) {
        for (width = 0; width <= 140; ++width) {
            angdroid_store_column_layout l =
                angdroid_store_columns(width, 1, home);
            int available = angdroid_store_name_budget(l, 1, width - 1);
            char name[256];
            size_t displayed;
            CHECK(l.width >= 0 && l.width <= 104);
            CHECK(l.price_col == -1 ||
                (l.price_col > 0 && l.price_col < l.width));
            CHECK(l.weight_col == -1 ||
                (l.weight_col > 0 && l.weight_col < l.width));
            CHECK(available >= 0 && available <= l.width);
            if (home) CHECK(l.price_col == -1);
            if (l.price_col >= 0)
                CHECK(available <= l.price_col - 2);
            if (l.weight_col >= 0)
                CHECK(available <= l.weight_col - 2);

            displayed = angdroid_menu_copy_columns(name, sizeof(name),
                "神秘的古代卷軸「傳送」", (size_t)available);
            CHECK(displayed <= (size_t)available);
            CHECK(angdroid_utf8_display_columns(name) == displayed);
            check_scalars(name);
        }
    }
}

static void shop_values(void)
{
    char b[128];
    CHECK(angdroid_store_price_text(b, sizeof(b), 123, true, true));
    CHECK(strcmp(b, "      123 均價") == 0);
    CHECK(angdroid_utf8_display_columns(b) <= 14);
    CHECK(angdroid_store_price_text(
        b, sizeof(b), 1000000000L, true, true));
    CHECK(strcmp(b, "1000000000") == 0);
    CHECK(angdroid_store_weight_text(b, sizeof(b), 165, true));
    CHECK(strcmp(b, " 16.5 磅") == 0);
    CHECK(angdroid_utf8_display_columns(b) <= 10);
    CHECK(angdroid_store_weight_text(b, sizeof(b), 165, false));
    CHECK(strcmp(b, " 16.5 lb") == 0);
    CHECK(angdroid_store_safe_row(-10, 8) == 0);
    CHECK(angdroid_store_safe_row(40, 8) == 7);
    CHECK(angdroid_store_safe_row(0, 0) == 0);
}

int main(void)
{
    names_and_prefixes();
    dungeon_tiny_columns();
    shop_columns();
    shop_values();
    printf("PASS: FA dungeon prefixes and bilingual store column safety (%lu checks)\n",
        checks);
    return 0;
}
