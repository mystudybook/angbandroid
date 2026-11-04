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

    puts("PASS: inventory text remains unchanged and clips by screen cells");
    return 0;
}
