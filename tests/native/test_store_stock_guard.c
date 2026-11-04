#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "../../app/src/main/cpp/angband/store-stock-guard.h"

int main(void)
{
    /* Ordinary stock indices are bounded by both the current stock count
     * and the fixed capacity of the backing pointer array. */
    assert(angdroid_store_stock_index_valid(0, 1, 10));
    assert(angdroid_store_stock_index_valid(9, 10, 10));
    assert(!angdroid_store_stock_index_valid(-1, 10, 10));
    assert(!angdroid_store_stock_index_valid(10, 10, 10));
    assert(!angdroid_store_stock_index_valid(0, 0, 10));
    assert(!angdroid_store_stock_index_valid(0, -1, 10));
    assert(!angdroid_store_stock_index_valid(0, 10, 0));
    assert(!angdroid_store_stock_index_valid(0, 10, -1));
    assert(!angdroid_store_stock_index_valid(3, 10, 3));
    assert(angdroid_store_stock_index_valid(2, 10, 3));

    /* Transaction and examine operations must only start after an
     * explicit SELECT. Cancel, rotate, and background events cancel. */
    assert(angdroid_store_selected_index(1, 0, 1, 10) == 0);
    assert(angdroid_store_selected_index(1, 2, 4, 3) == 2);
    assert(angdroid_store_selected_index(0, 2, 4, 3) == -1);
    assert(angdroid_store_selected_index(0, 0, 1, 10) == -1);
    assert(angdroid_store_selected_index(1, -1, 10, 10) == -1);
    assert(angdroid_store_selected_index(1, 10, 10, 10) == -1);
    assert(angdroid_store_selected_index(1, 2, 2, 10) == -1);
    assert(angdroid_store_selected_index(1, 3, 10, 3) == -1);
    assert(angdroid_store_selected_index(1, 0, 0, 10) == -1);

    /* Large/stale indices never need a backing-list dereference. */
    assert(angdroid_store_selected_index(1, INT_MAX, INT_MAX, 20) == -1);
    assert(angdroid_store_selected_index(1, 0, INT_MAX, 20) == 0);
    assert(angdroid_store_selected_index(1, INT_MIN, 20, 20) == -1);

    puts("PASS: store cancel, zero-stock and bounded purchase index");
    return 0;
}
