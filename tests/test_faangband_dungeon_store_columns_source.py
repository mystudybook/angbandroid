#!/usr/bin/env python3
"""FAangband object-list and store render using terminal columns, not bytes."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
DUNGEON = (CPP / "faangband/obj-list.c").read_text(encoding="utf-8")
STORE = (CPP / "faangband/ui-store.c").read_text(encoding="utf-8")
NAME = (CPP / "angband/object-list-name-display.h").read_text(encoding="utf-8")
COLUMNS = (CPP / "angband/store-column-layout.h").read_text(encoding="utf-8")
DETAIL = (CPP / "angband/store-detail-text.h").read_text(encoding="utf-8")
MENU = (CPP / "angband/menu-text-fit.h").read_text(encoding="utf-8")
LOCALE = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
SHOP_NAMES = (CPP / "angband/zh-tw-store-names.h").read_text(
    encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_dungeon_store_columns.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

# The original dungeon list split mutable UTF-8 names via strtok() and
# restricted the first word to three bytes (not three display columns).
assert '#include "../angband/object-list-name-display.h"' in DUNGEON
start = DUNGEON.index("void object_list_format_name(")
dungeon = DUNGEON[start:]
assert "char name[256];" in dungeon
assert "if (!line_buffer || !size) return;" in dungeon
assert "line_buffer[0] = '\\0';" in dungeon
assert "ODESC_ZH_TW | ODESC_ALTNUM" in dungeon
assert "(entry->count[field] << 16)" in dungeon
assert "has_singular_prefix || entry->count[field] != 1" in dungeon
assert "angdroid_object_list_name_copy(line_buffer, size, name," in dungeon
assert "strtok(" not in dungeon
assert 'strnfmt(line_buffer, size, "%3.3s "' not in dungeon
assert "my_strcat(line_buffer, chunk, size)" not in dungeon
assert "angdroid_utf8_append_complete(" in NAME
assert "angdroid_utf8_copy_complete(" in NAME
assert "ascii_prefix" in NAME

# Shop owner, row and numeric cells are all bounded by screen geometry.
for header in (
    "store-column-layout.h", "store-detail-text.h", "menu-text-fit.h",
    "zh-tw-ui.h", "zh-tw-store-names.h", "zh-tw-store-text.h",
):
    assert f'#include "../angband/{header}"' in STORE
assert '#include "droid.h"' in STORE

painter = STORE.split("static void fa_store_put_clipped(", 1)[1].split(
    "\n}\n", 1)[0]
assert "row < 0 || row >= Term->hgt" in painter
assert "col < 0 || col >= Term->wid" in painter
assert "angdroid_menu_copy_columns(clipped, sizeof(clipped)," in painter
assert "columns = MIN(columns, Term->wid - col);" in painter

recalc = STORE.split("static void store_display_recalc(", 1)[1].split(
    "\n}\n", 1)[0]
for part in (
    "angdroid_store_columns(", "columns.price_col >= 0",
    "columns.weight_col >= 0", "angdroid_store_safe_row(",
    "wid >= 70 && hgt >= 16", "menu_layout(m, &loc);",
):
    assert part in recalc, part
for old in ("= wid - 14;", "= wid - 26;", "hgt -= 3;"):
    assert old not in recalc

entry = STORE.split("static void store_display_entry(", 1)[1].split(
    "\n}\n", 1)[0]
assert "char o_name[256];" in entry
assert "oid < 0 || oid >= store->stock_num" in entry
assert "row < 0 || row >= Term->hgt" in entry
assert "width = MIN(width, Term->wid - col);" in entry
assert "angdroid_store_name_budget(columns, col, width)" in entry
assert "ODESC_PREFIX | ODESC_ZH_TW" in entry
assert "desc |= ODESC_FULL | ODESC_STORE;" in entry
assert "desc |= ODESC_FULL;" in entry
assert "object_desc(o_name, sizeof(o_name), obj, desc, player);" in entry
assert "fa_store_put_clipped(obj->kind->base->attr," in entry
assert "angdroid_store_weight_text(" in entry
assert "angdroid_store_price_text(" in entry
assert "x = price_item(store, obj, false, 1);" in entry
assert "if (player->au < x)" in entry
assert entry.count("angdroid_utf8_display_columns(out_val)") == 2
assert "columns.weight_col >= 0" in entry
assert "columns.price_col >= 0" in entry
assert "c_put_str(obj->kind->base->attr, o_name" not in entry

frame = STORE.split("static void store_display_frame(", 1)[1].split(
    "\n}\n", 1)[0]
assert "angdroid_store_display_name(" in frame
assert 'angdroid_game_ui_text("Your Home"' in frame
assert 'angdroid_game_ui_text("Store Inventory"' in frame
assert "owner_cols = angdroid_utf8_display_columns(buf);" in frame
assert "summary_cols = angdroid_utf8_display_columns(buf);" in frame
assert "summary_cols + owner_cols + 4" in frame
assert "fa_store_put_clipped(" in frame
assert "strlen(buf)" not in frame
assert "if (layout.weight_col >= 0)" in frame
assert "if (layout.price_col >= 0)" in frame

redraw = STORE.split("static void store_redraw(", 1)[1].split(
    "\n}\n", 1)[0]
assert "Term->wid >= 70 && Term->hgt >= 16" in redraw
assert "fa_store_put_clipped(COLOUR_WHITE," in redraw
assert "剩餘金幣：" in redraw
assert "Gold Remaining: %9ld" in redraw
assert "按 ? 查看說明" in redraw

welcome = STORE.split("static void prt_welcome(", 1)[1].split(
    "\n}\n", 1)[0]
assert "angdroid_utf8_next_scalar(owner_name + used, &cp)" in welcome
assert "width > sizeof(short_name) - used - 1" in welcome
assert "short_name[used] = '\\0';" in welcome
assert "short_name[j] =" not in welcome

help_text = STORE.split("static void store_display_help(", 1)[1].split(
    "\n}\n", 1)[0]
assert "bool zh_tw = angdroid_zh_tw_enabled();" in help_text
assert help_text.count("angdroid_store_ui_text(") >= 12
assert 'text_out(" examines")' not in help_text
assert 'text_out(" exits the building.")' not in help_text
sell = STORE.split("static bool store_sell(", 1)[1].split(
    "\n}\n", 1)[0]
assert "angdroid_store_ui_text(prompt, angdroid_zh_tw_enabled())" in sell
assert "angdroid_store_ui_text(reject, angdroid_zh_tw_enabled())" in sell
assert "CMD_DROP, tester, get_mode" in sell

for en in ("Your Home", "Home Inventory", "Store Inventory", "Weight", "Price"):
    assert '{ "' + en + '",' in LOCALE or '{"' + en + '",' in LOCALE
assert '"General Store"' in SHOP_NAMES
assert "angdroid_store_name_budget(" in COLUMNS
assert "angdroid_store_price_text(" in DETAIL
assert "angdroid_store_weight_text(" in DETAIL
assert "angdroid_menu_copy_columns(" in MENU

assert "names_and_prefixes();" in TEST
assert "dungeon_tiny_columns();" in TEST
assert "shop_columns();" in TEST
assert "shop_values();" in TEST
assert "for (cap = 0; cap <= 90; ++cap)" in TEST
assert "for (width = 0; width <= 140; ++width)" in TEST
assert "check_scalars(dst);" in TEST
assert "test_faangband_dungeon_store_columns.c" in RUN
assert "test_faangband_dungeon_store_columns_source.py" in VERIFY
print("PASS: FA dungeon prefix and store name/price terminal columns safe")
