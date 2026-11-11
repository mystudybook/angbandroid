#!/usr/bin/env python3
"""FAangband empty shop, stock refresh and modal purchase identity contracts."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
SHARED = ROOT / "app/src/main/cpp/angband"
STORE = (FA / "ui-store.c").read_text(encoding="utf-8")
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
MENU_H = (FA / "ui-menu.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_faangband_store_stock_lifecycle.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def section(text, begin, end):
    return text.split(begin, 1)[1].split(end, 1)[0]


assert '#include "../angband/menu-action-guard.h"' in MENU
assert "MN_ALLOW_EMPTY_INPUT = 0x80" in MENU_H
assert "angdroid_menu_allow_empty_input(" in (
    SHARED / "menu-action-guard.h").read_text(encoding="utf-8")
dispatch = section(MENU, "static bool menu_handle_action(", "\n/**\n * Handle navigation keypresses.")
assert "angdroid_fa_menu_source_index(" in dispatch
assert "if (oid < 0 || !is_valid_row(m, m->cursor))" in dispatch
assert "angdroid_menu_allow_empty_input(m->flags," in dispatch
assert "MN_ALLOW_EMPTY_INPUT" in dispatch
assert "in->type == EVT_KBRD || in->type == EVT_MOUSE" in dispatch
assert "row_handler(m, in, -1)" in dispatch
assert "row_handler(m, in, oid)" in dispatch
assert dispatch.index("if (oid < 0") < dispatch.index("row_handler(m, in, oid)")

assert '#include "../angband/store-stock-guard.h"' in STORE
assert '#include "../angband/store-menu-row.h"' in STORE
context = section(STORE, "struct store_context {", "\n};")
assert "bool stock_changed;" in context
initializer = section(STORE, "static void store_menu_init(", "\n/**\n * Display contents")
assert "ctx->stock_changed = false;" in initializer
assert "menu->flags |= MN_ALLOW_EMPTY_INPUT;" in initializer
handler = section(STORE, "static bool store_menu_handle(", "\n/* The native menu driver")
assert "ctx->stock_changed = false;" in handler
assert handler.index("ctx->stock_changed = false;") < handler.index("if (event->type == EVT_SELECT)")
refresh = section(STORE, "static void refresh_stock(", "\n/**\n * Enter a store.")
assert "ctx->stock_changed = true;" in refresh
assert "Term_flush();" in refresh
assert refresh.index("store_stock_list(") < refresh.index("ctx->stock_changed = true;")
assert refresh.index("ctx->stock_changed = true;") < refresh.index("Term_flush();")
assert refresh.index("Term_flush();") < refresh.index("store_menu_recalc(menu);")
recalc = section(STORE, "static void store_menu_recalc(", "\n/**\n * Process a command")
assert "MAX(0, MIN(ctx->store->stock_num," in recalc
assert "z_info->store_inven_max)), ctx);" in recalc
assert "angdroid_store_clear_stale_selection(ctx->stock_changed," in recalc
assert recalc.index("menu_setpriv(") < recalc.index("angdroid_store_clear_stale_selection(")
stock = section(STORE, "static int store_get_stock(", "\n/** Enum for context menu entries")
for fragment in ("ctx->store->stock_num <= 0", "angdroid_store_selected_index(",
                 "!ctx->stock_changed", "ctx->list[selected]", "e.type == EVT_SELECT"):
    assert fragment in stock, fragment
assert "return oid;" not in stock

buy = section(STORE, "static bool store_purchase(", "\n/**\n * Examine an item")
assert "angdroid_store_stock_index_valid(" in buy
assert buy.index("angdroid_store_stock_index_valid(") < buy.index("obj = ctx->list[item];")
assert buy.count("ctx->list[item] != obj") >= 2
assert "if (!response) {\n\t\t\tobject_delete(NULL, NULL, &dummy);" in buy
examine = section(STORE, "static void store_examine(", "\nstatic void store_menu_set_selections(")
assert examine.index("angdroid_store_stock_index_valid(") < examine.index("obj = ctx->list[item];")
popup = section(STORE, "static bool context_menu_store_item(", "\n/**\n * Handle store menu input")
assert popup.index("angdroid_store_stock_index_valid(") < popup.index("m = menu_dynamic_new();")
assert "ctx->list[oid] != obj" in popup
assert "ctx->stock_changed || !ctx->list" in popup

for f in ("empty_shop_commands();", "mutated_stock_requires_new_selection();",
          "bounds_and_popup_after_reorder();"):
    assert f in NATIVE
assert "test_faangband_store_stock_lifecycle.c" in RUN
assert "test_faangband_store_stock_lifecycle_source.py" in VERIFY
print("PASS: FA shop stock generation, empty commands and purchase guards")
