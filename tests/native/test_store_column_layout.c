#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/store-column-layout.h"

int main(void)
{
    angdroid_store_column_layout l;

    l = angdroid_store_columns(80, 1, 0);
    assert(l.width == 80 && l.price_col == 66);
    assert(l.weight_col == 56 && l.name_cols == 54);
    assert(l.gold_col == 54 && l.owner_end == 78);
    assert(angdroid_store_name_budget(l, 1, 79) == 54);
    assert(angdroid_store_name_budget(l, 1, 8) == 8);
    assert(angdroid_store_name_budget(l, 60, 10) == 5);
    assert(angdroid_store_name_budget(l, 1, -1) == 0);

    /* Phone widths: keep the name, then price, then optional weight. */
    l = angdroid_store_columns(20, 1, 0);
    assert(l.price_col == -1 && l.weight_col == -1);
    assert(l.name_cols == 19 && l.gold_col == 0);
    assert(angdroid_store_name_budget(l, 1, 19) == 19);
    l = angdroid_store_columns(27, 1, 0);
    assert(l.price_col == -1 && l.weight_col == -1);
    l = angdroid_store_columns(28, 1, 0);
    assert(l.price_col == 14 && l.weight_col == -1);
    assert(l.name_cols == 12);
    l = angdroid_store_columns(37, 1, 0);
    assert(l.price_col == 23 && l.weight_col == -1);
    l = angdroid_store_columns(38, 1, 0);
    assert(l.price_col == 24 && l.weight_col == 14);
    assert(l.name_cols == 12);
    assert(angdroid_store_name_budget(l, 1, 37) == 12);

    /* Home has no price column and can show weight earlier. */
    l = angdroid_store_columns(28, 1, 1);
    assert(l.price_col == -1 && l.weight_col == 14);
    assert(l.name_cols == 12);
    l = angdroid_store_columns(27, 1, 1);
    assert(l.weight_col == -1);

    /* No negative coordinates or subtraction wrapping on tiny terminals. */
    l = angdroid_store_columns(1, 1, 0);
    assert(l.width == 1 && l.name_cols == 0);
    assert(l.price_col == -1 && l.weight_col == -1);
    assert(angdroid_store_name_budget(l, 1, 1) == 0);
    l = angdroid_store_columns(0, 1, 0);
    assert(l.width == 0 && l.name_cols == 0);
    assert(l.price_col == -1 && l.weight_col == -1);
    l = angdroid_store_columns(INT_MAX, INT_MAX, 0);
    assert(l.width == 104 && l.name_cols == 0);
    assert(l.gold_col == 78);
    l = angdroid_store_columns(105, 1, 0);
    assert(l.width == 104 && l.price_col == 90 && l.weight_col == 80);

    assert(angdroid_store_safe_row(-9, 3) == 0);
    assert(angdroid_store_safe_row(0, 0) == 0);
    assert(angdroid_store_safe_row(8, 2) == 1);
    assert(angdroid_store_safe_row(4, 10) == 4);

    puts("PASS: store price and weight layout never overlaps names or wraps");
    return 0;
}
