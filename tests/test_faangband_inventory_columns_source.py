#!/usr/bin/env python3
"""FAangband inventory/selection CJK column geometry and clipping guards."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
UI = (CPP / "faangband/ui-object.c").read_text(encoding="utf-8")
MENU = (CPP / "angband/menu-text-fit.h").read_text(encoding="utf-8")
LAYOUT = (CPP / "angband/item-selection-layout.h").read_text(encoding="utf-8")
EQUIP = (CPP / "angband/inventory-equip-mention.h").read_text(encoding="utf-8")
COLUMNS = (CPP / "angband/object-list-text.h").read_text(encoding="utf-8")
UNICODE = (CPP / "curses/unicode_display.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_inventory_columns.c").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for path in ("object-list-text.h", "inventory-equip-mention.h",
             "item-selection-layout.h"):
    assert f'#include "../angband/{path}"' in UI

# Item name generation now has enough bytes for the base, Flavor, artifact
# and suffix. Screen text is clipped only in temporary row-local buffers.
assert "char o_name[256];" in UI
helpers = UI[UI.index("static olist_detail_t fa_object_list_visible_extras("):
             UI.index("static void wipe_obj_list(")]
first = UI[UI.index("static void show_obj("):
           UI.index("static void wipe_obj_list(")]
assert "int row_width)" in first
assert "char name[sizeof(items[obj_num].o_name)];" in first
assert "available = angdroid_object_list_cell_budget(ex_offset, row_width);" in first
assert "angdroid_menu_copy_columns(label, sizeof(label)," in first
assert "angdroid_menu_copy_columns(" in first
assert "name, sizeof(name), items[obj_num].o_name," in first
assert "col + (int)(label_size + equip_label_size)" in first
assert "c_put_str(attr, name, row + obj_num," in first
assert "items[obj_num].o_name[truncate]" not in UI
assert "label_size + equip_label_size + strlen(items[obj_num].o_name)" not in UI
assert "row_width = MIN(row_width, Term->wid - col);" in first
assert "if (row_width <= 0) return;" in first
assert "fa_object_list_put_detail(buf, row + obj_num, col," in first
assert "angdroid_object_list_detail_fits(" in helpers
assert "angdroid_utf8_display_columns(text)" in helpers
assert "ex_offset_ctr += 9;" in first
assert "ex_offset_ctr += 10;" in first
assert "fa_object_list_visible_extras(" in helpers
assert "angdroid_object_list_fit_extras(" in helpers

build = UI[UI.index("static void build_obj_list("):
           UI.index("static void set_obj_names(")]
assert "angdroid_inventory_equip_mention(" in build
assert "equip_mention(player, i)" in build
assert "utf8_clipto(" not in build
assert "utf8_strlen(" not in build
assert "items[num_obj].key = (items[num_obj].label)[0];" in build
assert "items[num_obj].object = obj;" in build

names = UI[UI.index("static void set_obj_names("):
           UI.index("static void show_obj_list(")]
for expr in ("items[i].label", "items[i].equip_label", "items[i].o_name"):
    assert f"angdroid_utf8_display_columns({expr})" in names
assert "strlen(items[i].o_name)" not in names
assert "ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW" in names
assert '#include "droid.h"' in UI
assert "if (terse && !angdroid_zh_tw_enabled()) {" in names
assert "ODESC_PREFIX | ODESC_FULL | ODESC_TERSE" in names
assert "if (terse && !angdroid_zh_tw_enabled())" in names
assert "if (Term->wid < 50) terse = true;" in UI

list_body = UI[UI.index("static void show_obj_list("):
               UI.index("void show_inven(", UI.index("static void show_obj_list("))]
assert "mode = fa_object_list_visible_extras(mode, Term->wid);" in list_body
assert "if (mode & OLIST_RECHARGE) ex_width += 10;" in list_body
assert "ex_offset = angdroid_object_list_name_offset(" in list_body
assert "show_obj(i, row, col, false, mode, Term->wid - col);" in list_body
assert "ex_offset = MIN(max_len, (size_t)" not in UI

# Menu iterator now participates in resize. It clamps rows, name width and
# optional columns using the same code as standard Angband, not byte lengths.
resize = UI[UI.index("static void fa_item_menu_resize("):
            UI.index("static struct object *item_menu(")]
assert "Term_get_size(&width, &height);" in resize
assert "fa_object_list_visible_extras(olist_mode, width)" in resize
assert "angdroid_item_selection_region(width, height," in resize
assert "angdroid_utf8_display_columns(header)" in resize
assert "ex_offset = angdroid_object_list_name_offset(" in resize
assert "menu_layout(m, &area);" in resize
assert "area.page_rows = layout.page_rows;" in resize
assert "if (olist_render_mode & OLIST_RECHARGE) ex_width += 10;" in resize
assert "fa_item_menu_resize };" in UI
assert "fa_item_menu_resize(m);" in UI
assert "show_obj(oid, row - oid, col, cursor, olist_render_mode, width);" in UI

# Renderer width classifier and clipper must agree on CJK two-cell glyphs.
assert "angdroid_terminal_columns(codepoint)" in MENU
assert "if (width > max_columns - columns) break;" in MENU
assert "angdroid_utf8_next_scalar(source, &codepoint)" in MENU
assert "angdroid_utf8_display_columns(" in UNICODE
assert "angdroid_inventory_equip_mention(" in EQUIP
assert "angdroid_item_selection_region(" in LAYOUT
assert "angdroid_object_list_cell_budget(" in COLUMNS
assert "angdroid_object_list_detail_fits(" in COLUMNS

assert "column_geometry();" in C_TEST
assert "clipped_rows();" in C_TEST
assert "extras_and_rotation();" in C_TEST
assert "for (width = 0; width <= 120; ++width)" in C_TEST
assert "for (w = 1; w <= 140; ++w)" in C_TEST
assert "valid_utf8(name);" in C_TEST
assert 'CHECK(strcmp(original, "題為「Marmor Nitre」的卷軸（治療）") == 0);' in C_TEST
assert "test_faangband_inventory_columns.c" in NATIVE
assert "test_faangband_inventory_columns_source.py" in VERIFY
print("PASS: FA inventory clips by terminal columns without mutating item names")
