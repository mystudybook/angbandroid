#!/usr/bin/env python3
"""FAangband object-list rows and store resize must use cell-safe geometry."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
LIST = (CPP / "faangband/ui-obj-list.c").read_text(encoding="utf-8")
STORE = (CPP / "faangband/ui-store.c").read_text(encoding="utf-8")
ENGINE_MENU = (CPP / "faangband/ui-menu.c").read_text(encoding="utf-8")
MENU_HEADER = (CPP / "faangband/ui-menu.h").read_text(encoding="utf-8")
LAYOUT = (CPP / "angband/object-list-compact-layout.h").read_text(
    encoding="utf-8")
LOCATION = (CPP / "angband/zh-tw-list-location.h").read_text(
    encoding="utf-8")
SCALARS = (CPP / "curses/unicode_display.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_object_list_location.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for header in ("object-list-compact-layout.h", "zh-tw-list-location.h"):
    assert f'#include "../angband/{header}"' in LIST
assert '#include "../curses/unicode_display.h"' in LIST
assert '#include "droid.h"' in LIST

section = LIST.split("static void object_list_format_section(", 1)[1].split(
    "static bool object_list_format_special(", 1)[0]
assert "const bool zh_tw = angdroid_zh_tw_enabled();" in section
assert '"%s沒有物品。\\n"' in section
assert '"%s有 %d 件%s物品%s\\n"' in section
assert "angdroid_utf8_display_columns(line_buffer)" in section
assert "angdroid_list_relative_location(location, sizeof(location)," in section
assert "angdroid_object_row_fit(max_width," in section
assert "angdroid_utf8_display_columns(location)" in section
assert "if (!budget.show_location) location[0] = '\\0';" in section
assert "angdroid_utf8_clip_columns(line_buffer, full_width);" in section
assert "angdroid_utf8_printf_field_bytes(" in section
assert "object_list_format_name(&list->entries[entry_index]" in section
assert "abs(list->entries[entry_index].dy)" not in section
assert "utf8_strlen(location)" not in section
assert "utf8_clipto(line_buffer, full_width)" not in section
assert "full_width = max_width - 2" not in section
assert "strlen(line_buffer) - utf8_strlen(line_buffer)" not in section
assert "…另有" in section or "……另有" in section

formatall = LIST.split("static void object_list_format_textblock(", 1)[1].split(
    "void object_list_show_subwindow(", 1)[0]
assert 'angdroid_zh_tw_enabled() ? "視野內" : "You can see"' in formatall
assert 'angdroid_zh_tw_enabled() ? "感知範圍內" :' in formatall
assert "ODESC_ALTNUM" not in formatall  # Engine count lives in obj-list.c

subwindow = LIST.split("void object_list_show_subwindow(", 1)[1].split(
    "void object_list_show_interactive(", 1)[0]
popup = LIST.split("void object_list_show_interactive(", 1)[1]
assert "if (height < 1 || width < 7)" in subwindow
assert "if (height < 7 || width < 12)" in popup
assert "angdroid_object_list_popup_fit(width, height," in popup
for field in ("col", "row", "width", "height"):
    assert f"popup.{field}" in popup
assert "r.col = -safe_width;" not in popup
assert "safe_width = MIN(width - 13" not in popup
assert "textblock_free(tb);" in popup
assert "object_list_free(list);" in popup

assert "angdroid_object_row_fit(" in LAYOUT
assert "angdroid_object_list_popup_fit(" in LAYOUT
assert "if (screen_width < 12 || screen_height < 7)" in LAYOUT
assert "available - location_columns >= 8" in LAYOUT
assert "if (!out || capacity == 0) return;" in LOCATION
assert "int64_t offset_y = (int64_t)dy;" in LOCATION
assert "int64_t offset_x = (int64_t)dx;" in LOCATION
assert "length < 0 || (size_t)length >= sizeof formatted" in LOCATION
assert "angdroid_utf8_copy_complete(out, capacity, formatted)" in LOCATION
assert "angdroid_utf8_printf_field_bytes(" in SCALARS

resize = STORE.split("static void store_menu_resize(", 1)[1].split(
    "\n}\n", 1)[0]
assert "struct store_context *ctx = menu_priv(m);" in resize
assert "store_display_recalc(ctx);" in resize
assert "ctx->flags |= STORE_FRAME_CHANGE | STORE_GOLD_CHANGE;" in resize
assert "store_redraw(ctx);" in resize
assert "store_menu_handle,\n\tstore_menu_resize" in STORE
assert "void (*resize)(struct menu *m);" in MENU_HEADER
assert "menu->row_funcs->resize(menu);" in ENGINE_MENU
assert "in.type == EVT_RESIZE" in ENGINE_MENU

for function in (
    "directions_and_distances();",
    "narrow_rows();",
    "chinese_name_and_position();",
    "popup_rotation();",
):
    assert function in C_TEST
assert "INT_MIN" in C_TEST and "INT_MAX" in C_TEST
assert "width <= 160" in C_TEST
assert "h <= 60" in C_TEST
assert "cap <= sizeof(short_text)" in C_TEST
assert "check_utf8(name);" in C_TEST
assert "test_faangband_object_list_location.c" in RUN
assert "test_faangband_object_list_location_source.py" in VERIFY
print("PASS: FA object list CJK distances, narrow popup, store resize wired")
