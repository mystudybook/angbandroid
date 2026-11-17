/* FAangband inventory/selection cell geometry: independent host C99 tests.
 * No Android dependencies; production Angband terminal-column helpers are
 * now used by faangband/ui-object.c without touching its saved item names. */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/object-list-text.h"
#include "../../app/src/main/cpp/angband/inventory-equip-mention.h"
#include "../../app/src/main/cpp/angband/item-selection-layout.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void valid_utf8(const char *s)
{
    size_t len = strlen(s), offset = 0;
    while (offset < len) {
        uint32_t cp = 0;
        size_t n = angdroid_utf8_next_scalar(s + offset, &cp);
        CHECK(n > 0 && n <= len - offset);
        CHECK(cp != 0xFFFDu);
        offset += n;
    }
    CHECK(offset == len);
}

static void column_geometry(void)
{
    char output[128], tiny[3] = {'Q', 'Q', 'Q'};
    size_t n;
    CHECK(angdroid_utf8_display_columns("a) ") == 3);
    CHECK(angdroid_utf8_display_columns("頭部") == 4);
    CHECK(angdroid_utf8_display_columns("治療藥水") == 8);
    CHECK(strlen("治療藥水") == 12);
    CHECK(angdroid_utf8_display_columns("題為「術法」的卷軸") == 18);

    n = angdroid_inventory_equip_mention(
        output, sizeof(output), "雙手武器");
    CHECK(n == 14);
    CHECK(angdroid_utf8_display_columns(output) == 14);
    CHECK(strcmp(output, "雙手武器      ") == 0);
    n = angdroid_inventory_equip_mention(
        output, sizeof(output), "一二三四五六七八");
    CHECK(n == 14);
    CHECK(strcmp(output, "一二三四五六七") == 0);
    n = angdroid_menu_copy_columns(tiny, sizeof(tiny),
        "中文", 10);
    CHECK(n == 0 && tiny[0] == '\0');
    CHECK(angdroid_menu_copy_columns(output, sizeof(output),
        "天藍色藥水", 5) == 4);
    CHECK(strcmp(output, "天藍") == 0);
    CHECK(angdroid_menu_copy_columns(output, sizeof(output),
        "天藍色藥水", 6) == 6);
    CHECK(strcmp(output, "天藍色") == 0);
    valid_utf8(output);
}

static void clipped_rows(void)
{
    const char *original = "題為「Marmor Nitre」的卷軸（治療）";
    const char *label = "b) ";
    char name[256], equipment[80], clipped_label[80], clipped_equip[80];
    int width;
    const size_t original_bytes = strlen(original);
    CHECK(original_bytes > angdroid_utf8_display_columns(original));

    for (width = 0; width <= 120; ++width) {
        int offset = width >= 10 ? width - 9 : 0;
        size_t available = angdroid_object_list_cell_budget(
            offset, width);
        size_t tag, equip_cols, name_cols;
        strncpy(name, original, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        (void)angdroid_inventory_equip_mention(
            equipment, sizeof(equipment), "雙手");
        tag = angdroid_menu_copy_columns(
            clipped_label, sizeof(clipped_label), label, available);
        equip_cols = angdroid_menu_copy_columns(
            clipped_equip, sizeof(clipped_equip), equipment, available - tag);
        name_cols = angdroid_menu_copy_columns(
            name, sizeof(name), original, available - tag - equip_cols);
        CHECK(tag + equip_cols + name_cols <= available);
        CHECK(angdroid_utf8_display_columns(name) == name_cols);
        CHECK(angdroid_utf8_display_columns(clipped_label) == tag);
        CHECK(angdroid_utf8_display_columns(clipped_equip) == equip_cols);
        CHECK(strcmp(original, "題為「Marmor Nitre」的卷軸（治療）") == 0);
        CHECK(strlen(original) == original_bytes);
        valid_utf8(name);
    }
}

static void extras_and_rotation(void)
{
    const angdroid_object_list_extras small =
        angdroid_object_list_fit_extras(24, 12, true, true, true, true);
    const angdroid_object_list_extras medium =
        angdroid_object_list_fit_extras(40, 12, true, true, true, true);
    const angdroid_object_list_extras large =
        angdroid_object_list_fit_extras(80, 12, true, true, true, true);
    int w;
    CHECK(small.price && !small.fail && !small.recharge && !small.weight);
    CHECK(small.reserved == 9);
    CHECK(medium.price && medium.fail && !medium.recharge && !medium.weight);
    CHECK(medium.reserved == 19);
    CHECK(large.price && large.fail && large.recharge && large.weight);
    CHECK(large.reserved == 38);

    CHECK(angdroid_object_list_name_offset(20, 0, 40, 80) == 0);
    CHECK(angdroid_object_list_name_offset(40, 0, 10, 80) == 29);
    CHECK(angdroid_object_list_cell_budget(-9, 50) == 0);
    CHECK(angdroid_object_list_cell_budget(33, 16) == 16);
    CHECK(!angdroid_object_list_detail_fits(20, 24, 9, 4));
    CHECK(angdroid_object_list_detail_fits(14, 24, 9, 9));
    CHECK(!angdroid_object_list_detail_fits(14, 24, 9, 10));

    for (w = 1; w <= 140; ++w) {
        angdroid_item_selection_layout layout;
        bool works = angdroid_item_selection_region(
            w, 18, 34, 140, 74, 19, 50, true, &layout);
        CHECK(works);
        CHECK(layout.col >= 0 && layout.col < w);
        CHECK(layout.width >= 1 && layout.width <= w - layout.col);
        CHECK(layout.row == 1);
        CHECK(layout.page_rows > 0 && layout.page_rows <= 16);
        CHECK(angdroid_object_list_name_offset(
            w, layout.col, 19, 140) >= 0);
    }
    {
        angdroid_item_selection_layout layout;
        CHECK(!angdroid_item_selection_region(
            0, 18, 34, 100, 80, 9, 20, false, &layout));
        CHECK(layout.width == 0 && layout.page_rows == 0);
        CHECK(!angdroid_item_selection_region(
            20, 1, 34, 100, 80, 9, 20, false, &layout));
        CHECK(!angdroid_item_selection_region(
            20, 18, 34, 100, 80, 9, 0, false, &layout));
    }
}

int main(void)
{
    column_geometry();
    clipped_rows();
    extras_and_rotation();
    printf("PASS: FA inventory terminal columns, narrow rows, menu rotation (%lu checks)\n",
        checks);
    return 0;
}
