/* Standalone C99 inventory/equipment subwindow translation smoke test. */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/inventory-list-display.h"
#include "../../app/src/main/cpp/angband/object-list-text.h"

static void check_detail(enum angdroid_inventory_detail_kind kind,
                         int first, int second, const char *en,
                         const char *zh, size_t reserved)
{
    char out[80];
    assert(angdroid_inventory_detail_text(out, sizeof(out),
                                          kind, first, second, false));
    assert(strcmp(out, en) == 0);
    assert(angdroid_utf8_display_columns(out) <= reserved);
    assert(angdroid_inventory_detail_text(out, sizeof(out),
                                          kind, first, second, true));
    assert(strcmp(out, zh) == 0);
    assert(angdroid_utf8_display_columns(out) <= reserved);
    assert(angdroid_object_list_detail_fits(9, 30, (int)reserved,
                                           angdroid_utf8_display_columns(out)));
}

int main(void)
{
    char out[80], tiny[3];
    check_detail(ANGDROID_INVEN_PRICE, 123, 0, "   123 au", "   123 金", 9);
    check_detail(ANGDROID_INVEN_DEVICE_FAIL, 75, 0,
                 "  75% fail", "  75% 失敗", 10);
    check_detail(ANGDROID_INVEN_DEVICE_UNKNOWN, 999, 0,
                 "    ? fail", "    ? 失敗", 10);
    check_detail(ANGDROID_INVEN_RECHARGE_FAIL, 7, 5,
                 " 7.5% fail", " 7.5% 失敗", 10);
    check_detail(ANGDROID_INVEN_WEIGHT, 12, 4,
                 "  12.4 lb", "  12.4 磅", 9);

    assert(angdroid_inventory_burden_text(out, sizeof(out), 125, 43, false));
    assert(strcmp(out, "Burden 12.5 lb (4.3 lb remaining) ") == 0);
    assert(angdroid_inventory_burden_text(out, sizeof(out), 125, 43, true));
    assert(strcmp(out, "負重 12.5 磅（剩餘 4.3 磅） ") == 0);
    assert(angdroid_inventory_burden_text(out, sizeof(out), 125, -43, true));
    assert(strcmp(out, "負重 12.5 磅（超出 4.3 磅） ") == 0);

    /* Old abs(INT_MIN) overflowed. The label must now remain complete. */
    assert(angdroid_inventory_burden_text(out, sizeof(out), 125, INT_MIN, true));
    assert(strstr(out, "214748364.8") != NULL);

    assert(angdroid_inventory_slot_text(out, sizeof(out), 3, false));
    assert(strcmp(out, "Slot 3        : ") == 0);
    assert(angdroid_inventory_slot_text(out, sizeof(out), 3, true));
    assert(strcmp(out, "格 3        : ") == 0);
    assert(angdroid_utf8_display_columns(out) <
           angdroid_utf8_display_columns("Slot 3        : "));

    assert(!angdroid_inventory_detail_text(tiny, sizeof(tiny),
                                          ANGDROID_INVEN_DEVICE_FAIL,
                                          75, 0, true));
    assert(tiny[0] == '\0');
    assert(!angdroid_inventory_burden_text(tiny, sizeof(tiny), 125, 43, true));
    assert(tiny[0] == '\0');
    assert(!angdroid_inventory_slot_text(tiny, sizeof(tiny), 3, true));
    assert(tiny[0] == '\0');
    assert(!angdroid_inventory_detail_text(NULL, 80,
                                          ANGDROID_INVEN_PRICE,
                                          123, 0, true));

    /* Oversized numbers cannot corrupt the adjacent 9/10-cell field. */
    assert(angdroid_inventory_detail_text(out, sizeof(out),
                                          ANGDROID_INVEN_PRICE,
                                          INT_MAX, 0, true));
    assert(!angdroid_object_list_detail_fits(0, 80, 9,
                                             angdroid_utf8_display_columns(out)));
    assert(angdroid_inventory_detail_text(out, sizeof(out),
                                          ANGDROID_INVEN_DEVICE_FAIL,
                                          1234567, 0, true));
    assert(!angdroid_object_list_detail_fits(0, 80, 10,
                                             angdroid_utf8_display_columns(out)));
    puts("PASS: bilingual inventory details, CJK columns and burden overflow guards");
    return 0;
}
