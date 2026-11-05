#!/usr/bin/env python3
"""Guard transaction selection bounds and cancellation cleanup in ui-store.c.

Pure C99 helper tests cover index math. This file checks source integration
but is not a substitute for testing full purchases in an Android APK.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-store.c").read_text(encoding="utf-8")

if '#include "store-stock-guard.h"' not in source:
    raise SystemExit("FAIL: missing shared store stock index guard")

def block(start, stop):
    return source.split(start, 1)[1].split(stop, 1)[0]

purchase = block("static bool store_purchase(", "\n/**\n * Examine an item in a store")
for item in (
    "if (ctx->inspect_only || !ctx->list ||",
    "angdroid_store_stock_index_valid(item, store->stock_num,",
    "obj = ctx->list[item];",
    "if (!obj)",
    "if (!response) {\n\t\t\tobject_delete(NULL, NULL, &dummy);",
):
    if item not in purchase:
        raise SystemExit(f"FAIL: store purchase unsafe or cancellation leak: {item}")
if purchase.index("angdroid_store_stock_index_valid(") > purchase.index(
    "obj = ctx->list[item];"
):
    raise SystemExit("FAIL: purchase dereferences stock before bounds check")
if purchase.count("object_delete(NULL, NULL, &dummy);") != 3:
    raise SystemExit("FAIL: all dummy object cleanup paths not present")

examine = block("static void store_examine(", "\nstatic void store_menu_set_selections(")
for part in ("angdroid_store_stock_index_valid(", "if (!obj)", "obj = ctx->list[item];"):
    if part not in examine:
        raise SystemExit(f"FAIL: examine stale item access: {part}")

get_stock = block("static int store_get_stock(", "\n/** Enum for context menu entries */")
for part in (
    "ctx->store->stock_num <= 0",
    "angdroid_store_selected_index(",
    "e.type == EVT_SELECT",
    "if (selected < 0 || !ctx->list[selected])",
    "return -1;",
):
    if part not in get_stock:
        raise SystemExit(f"FAIL: stock selection may execute after cancellation: {part}")
if "return oid;" in get_stock:
    raise SystemExit("FAIL: stale index still used as fallback selection")

popup = block("static bool context_menu_store_item(", "\n/**\n * Handle store menu input")
if "angdroid_store_stock_index_valid(" not in popup:
    raise SystemExit("FAIL: item popup is not guarded against stock shrink")
if popup.index("angdroid_store_stock_index_valid(") > popup.index(
    "m = menu_dynamic_new();"
):
    raise SystemExit("FAIL: invalid popup entry should be rejected before allocating")

sell = block("static bool store_sell(", "\n/**\n * Buy an object from a store")
if "if (ctx->inspect_only)" not in sell:
    raise SystemExit("FAIL: knowledge-only shop can submit sales")

handler = block("static bool store_menu_handle(", "\n/* Recalculate price/weight")
if handler.count("store_get_stock(m);") != 2:
    raise SystemExit("FAIL: purchase/examine still permit stale fallback item IDs")
if handler.count("angdroid_store_stock_index_valid(") < 2:
    raise SystemExit("FAIL: direct mouse/Enter stock dispatch bypasses bounds")

# After any store reorder, the next command must make a fresh selection.
recalc = block("static void store_menu_recalc(", "\n/**\n * Process a command")
if "angdroid_store_clear_stale_selection(ctx->stock_changed," not in recalc:
    raise SystemExit("FAIL: changed stock may silently keep a different product")
if "ctx->stock_changed = true;" not in source:
    raise SystemExit("FAIL: stock mutation does not invalidate pending selection")
if "flush(type, unused, user);" not in source:
    raise SystemExit("FAIL: queued fast touches can act on reordered products")

print("PASS: cancel cleanup, changed stock and purchase index guards")
