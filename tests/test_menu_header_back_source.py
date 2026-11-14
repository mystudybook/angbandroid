#!/usr/bin/env python3
"""Shopkeeper header and empty-shop Back/Exit behavior in both Angband variants.

The C99 test verifies actual terminal cell hit-testing. These source checks
verify the real native event routing, not emulator touch automation.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ANG = ROOT / "app/src/main/cpp/angband"
FA = ROOT / "app/src/main/cpp/faangband"
HIT = (ANG / "menu-hit-test.h").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_menu_header_back.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert "static inline int angdroid_menu_back_margin_hit(" in HIT
assert "col < origin_col && row >= origin_row &&" in HIT
assert "(int64_t)row - origin_row < page_rows" in HIT
assert "test_menu_header_back.c" in RUN
assert "test_menu_header_back_source.py" in VERIFY
assert "menu_header_and_left_margin();" in TEST
assert "extreme_native_coordinates();" in TEST

for folder in (ANG, FA):
    menu = (folder / "ui-menu.c").read_text(encoding="utf-8")
    store = (folder / "ui-store.c").read_text(encoding="utf-8")
    header = (folder / "ui-menu.h").read_text(encoding="utf-8")
    mouse = menu.split("bool menu_handle_mouse(", 1)[1].split(
        "static bool menu_handle_action(", 1)[0]
    assert "angdroid_menu_back_margin_hit(" in mouse
    assert "in->mouse.button == 2" in mouse
    assert "(*menu->context_hook)(menu, in, out)" in mouse
    assert "in->mouse.x < menu->active.col" not in mouse

    assert "MN_ALLOW_EMPTY_INPUT" in header
    assert "bool exit_requested;" in store
    header_menu = store.split("static int context_menu_store(", 1)[1].split(
        "static bool context_menu_store_item(", 1
    )[0]
    assert "case ACT_EXIT:" in header_menu
    assert "ctx->exit_requested = true;" in header_menu
    assert "return false;" in header_menu

    hook = store.split("static bool store_menu_context_exit(", 1)[1].split(
        "static region store_menu_region", 1
    )[0]
    assert "ctx->exit_requested" in hook
    assert "out->type = EVT_ESCAPE;" in hook
    assert "ctx->exit_requested = false;" in hook
    assert "in->type != EVT_MOUSE" in hook

    initialize = store.split("static void store_menu_init(", 1)[1].split(
        "void textui_store_knowledge(", 1
    )[0]
    assert "ctx->exit_requested = false;" in initialize
    assert "menu->context_hook = store_menu_context_exit;" in initialize

    handle = store.split("static bool store_menu_handle(", 1)[1].split(
        "static void store_menu_resize(", 1
    )[0]
    assert "event->mouse.button == 2" in handle
    assert "if ((event->mouse.y == 0) || (event->mouse.y == 1))" in handle
    assert "context_menu_store(ctx, oid, event->mouse.x, event->mouse.y) == false" in handle

# Angband also needs the same stock-identity checks already present in FA.
angstore = (ANG / "ui-store.c").read_text(encoding="utf-8")
popup = angstore.split("static bool context_menu_store_item(", 1)[1].split(
    "/**\n * Handle store menu input", 1
)[0]
assert "ctx->stock_changed || !ctx->list" in popup
assert "ctx->list[oid] != obj" in popup
stock = angstore.split("static int store_get_stock(", 1)[1].split(
    "/** Enum for context menu entries */", 1
)[0]
assert "ctx->stock_changed || selected < 0" in stock
buy = angstore.split("static bool store_purchase(", 1)[1].split(
    "/**\n * Examine an item", 1
)[0]
assert buy.count("ctx->list[item] != obj") >= 2

print("PASS: shop title, explicit Exit, right-click, Back margin and stock identity")
