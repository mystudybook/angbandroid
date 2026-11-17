#!/usr/bin/env python3
"""FA multi-column repaint, CJK popup geometry and filtered choice wiring."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MENU = (ROOT / "app/src/main/cpp/faangband/ui-menu.c").read_text(encoding="utf-8")
POPUP = (ROOT / "app/src/main/cpp/angband/menu-popup-layout.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_faangband_menu_popup_filter.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "../angband/menu-popup-layout.h"' in MENU
columns = MENU.split("static void display_columns(", 1)[1].split(
    "static char column_get_tag(", 1)[0]
assert "for (r = 0; r < rows; ++r)" in columns
assert "fa_menu_erase_clipped(row + r, col, loc->width);" in columns
assert "if (n <= 0) return;" in columns
assert columns.index("fa_menu_erase_clipped(row + r, col, loc->width);") < (
    columns.index("if (n <= 0) return;"))
assert "display_scrolling(menu, cursor, top, loc);" in columns

length = MENU.split("size_t menu_dynamic_longest_entry(", 1)[1].split(
    "void menu_dynamic_calc_location(", 1)[0]
assert "angdroid_utf8_display_columns(entry->text)" in length
assert "strlen(entry->text)" not in length

layout = MENU.split("void menu_dynamic_calc_location(", 1)[1].split(
    "int menu_dynamic_select(", 1)[0]
assert "angdroid_menu_popup_bounds(" in layout
assert "angdroid_menu_popup_box popup;" in layout
assert "if (!Term || !angdroid_menu_popup_bounds(" in layout
assert "m->active = r;" in layout
assert "r.width = popup.width;" in layout
assert "r.page_rows = popup.page_rows;" in layout
assert "m->count, &popup" in layout
assert "menu_layout(m, &r);" in layout
assert "r.width = menu_dynamic_longest_entry(m) + 3 + 2;" not in layout
assert "longest_label_columns" in POPUP

choice = MENU.split("int menu_dynamic_select(", 1)[1].split(
    "void menu_dynamic_free(", 1)[0]
assert "e.type != EVT_SELECT || !is_valid_row(m, m->cursor)" in choice
assert "angdroid_fa_menu_source_index(" in choice
assert "m->filter_list, m->cursor," in choice
assert "if (cursor < 0) return -1;" in choice
assert "if (!entry) return -1;" in choice
assert "return entry ? entry->value : -1;" in choice

assert "popup_viewports();" in NATIVE
assert "filtered_choice_mapping();" in NATIVE
assert "INT_MIN" in NATIVE and "SIZE_MAX" in NATIVE
assert "test_faangband_menu_popup_filter.c" in RUN
assert "test_faangband_menu_repaint_popup_source.py" in VERIFY
print("PASS: FA filtered columns repaint and CJK popup/choice safety")
