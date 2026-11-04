#ifndef ANGDROID_STORE_STOCK_GUARD_H
#define ANGDROID_STORE_STOCK_GUARD_H

/* A shop's displayed count may change after buy/sell and must not exceed
 * the allocated ctx->list capacity. Validate before indexing the list. */
static inline int angdroid_store_stock_index_valid(
    int index, int stock_count, int capacity)
{
    return index >= 0 && stock_count > 0 && capacity > 0 &&
        index < stock_count && index < capacity;
}

/* An explicit SELECT event is required to perform a purchase or examine
 * action. ESCAPE, resize exit and any other event must cancel, not fall
 * back to the stale cursor or a previously focused item. */
static inline int angdroid_store_selected_index(
    int is_select, int cursor, int stock_count, int capacity)
{
    return is_select &&
        angdroid_store_stock_index_valid(cursor, stock_count, capacity)
        ? cursor : -1;
}

#endif
