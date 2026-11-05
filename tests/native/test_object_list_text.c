#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/object-list-text.h"

int main(void)
{
    const char *object_name = "傳說長劍";
    char name[80];
    char label[80];
    char equipment[80];
    size_t label_cols, equipment_cols;
    size_t budget;

    assert(angdroid_object_list_name_offset(80, 4, 9, 80) == 66);
    assert(angdroid_object_list_name_offset(80, 4, 9, 20) == 20);
    assert(angdroid_object_list_name_offset(8, 0, 9, 80) == 0);
    assert(angdroid_object_list_name_offset(10, 4, 5, 80) == 0);
    assert(angdroid_object_list_name_offset(-1, 0, 0, 80) == 0);
    assert(angdroid_object_list_name_offset(80, -1, 0, 80) == 0);
    assert(angdroid_object_list_name_offset(80, 0, -1, 80) == 0);
    assert(angdroid_object_list_name_offset(INT_MAX, 0, 0,
        (size_t)INT_MAX) == INT_MAX - 1);
    assert(angdroid_object_list_name_offset(80, 0, 0, 0) == 0);

    assert(angdroid_object_list_cell_budget(20, 5) == 5);
    assert(angdroid_object_list_cell_budget(5, 20) == 5);
    assert(angdroid_object_list_cell_budget(0, 20) == 0);
    assert(angdroid_object_list_cell_budget(20, -1) == 0);

    /* Chinese labels and item names consume two cells, not three bytes.
     * The original item data must remain unchanged after a narrow render. */
    budget = angdroid_object_list_cell_budget(11, 8);
    label_cols = angdroid_menu_copy_columns(label, sizeof(label),
        "a) ", budget);
    equipment_cols = angdroid_menu_copy_columns(equipment,
        sizeof(equipment), "主手", budget - label_cols);
    angdroid_menu_copy_columns(name, sizeof(name), object_name,
        budget - label_cols - equipment_cols);
    assert(label_cols == 3);
    assert(equipment_cols == 4);
    assert(strcmp(name, "") == 0);

    budget = angdroid_object_list_cell_budget(16, 16);
    label_cols = angdroid_menu_copy_columns(label, sizeof(label),
        "a) ", budget);
    equipment_cols = angdroid_menu_copy_columns(equipment,
        sizeof(equipment), "主手", budget - label_cols);
    assert(angdroid_menu_copy_columns(name, sizeof(name),
        object_name, budget - label_cols - equipment_cols) == 8);
    assert(strcmp(name, object_name) == 0);

    budget = angdroid_object_list_cell_budget(5, 5);
    assert(angdroid_menu_copy_columns(name, sizeof(name),
        object_name, budget) == 4);
    assert(strcmp(name, "傳說") == 0);
    assert(strcmp(object_name, "傳說長劍") == 0);
    assert(angdroid_utf8_display_columns(name) == 4);
    assert(angdroid_utf8_display_columns(object_name) == 8);

    /* Reserve optional columns without swallowing item selection labels. */
    {
        angdroid_object_list_extras e;
        int screen, flags;
        e = angdroid_object_list_fit_extras(20, 12, true, true, true, true);
        assert(!e.price && !e.fail && !e.recharge && !e.weight);
        assert(e.reserved == 0);
        e = angdroid_object_list_fit_extras(21, 12, true, true, true, true);
        assert(!e.price && !e.fail && !e.recharge && !e.weight);
        e = angdroid_object_list_fit_extras(22, 12, true, true, true, true);
        assert(e.price && !e.fail && !e.recharge && !e.weight);
        assert(e.reserved == 9);
        e = angdroid_object_list_fit_extras(32, 12, true, true, true, true);
        assert(e.price && e.fail && !e.recharge && !e.weight);
        assert(e.reserved == 19);
        e = angdroid_object_list_fit_extras(41, 12, true, true, true, true);
        assert(e.price && e.fail && !e.recharge && e.weight);
        assert(e.reserved == 28);
        e = angdroid_object_list_fit_extras(51, 12, true, true, true, true);
        assert(e.price && e.fail && e.recharge && e.weight);
        assert(e.reserved == 38);
        e = angdroid_object_list_fit_extras(23, 12, false, false, true, false);
        assert(!e.price && !e.fail && e.recharge && !e.weight);
        assert(e.reserved == 10);

        assert(!angdroid_object_list_detail_fits(12, 20, 9, 9));
        assert(angdroid_object_list_detail_fits(12, 21, 9, 9));
        assert(!angdroid_object_list_detail_fits(12, 21, 9, 10));
        assert(!angdroid_object_list_detail_fits(-1, 40, 9, 9));
        assert(!angdroid_object_list_detail_fits(0, 20, 0, 0));
        assert(!angdroid_object_list_detail_fits(INT_MAX, 40, 9, 3));

        for (screen = 0; screen <= 160; ++screen) {
            for (flags = 0; flags < 16; ++flags) {
                int expected = 0;
                e = angdroid_object_list_fit_extras(screen, 12,
                    (flags & 1) != 0, (flags & 2) != 0,
                    (flags & 4) != 0, (flags & 8) != 0);
                if (e.price) expected += 9;
                if (e.fail) expected += 10;
                if (e.recharge) expected += 10;
                if (e.weight) expected += 9;
                assert(e.reserved == expected);
                assert(e.reserved >= 0);
                assert(screen <= 12 ? e.reserved == 0 :
                    e.reserved <= screen - 13);
            }
        }
    }

    puts("PASS: inventory text remains unchanged and clips by screen cells");
    return 0;
}
