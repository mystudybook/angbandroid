#!/usr/bin/env python3
"""FAangband short object-list count accuracy and store resize repaint."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
SOURCE = (FA / "ui-obj-list.c").read_text(encoding="utf-8")
ROWS = (FA / "obj-list-row-budget.h").read_text(encoding="utf-8")
STORE = (FA / "ui-store.c").read_text(encoding="utf-8")
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_faangband_object_list_row_budget.c").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "obj-list-row-budget.h"' in SOURCE
assert "angdroid_fa_list_row_budget(" in ROWS
assert "angdroid_fa_list_take_rows(" in ROWS
assert "angdroid_fa_list_undisplayed(" in ROWS
assert "if (max_lines <= 0) return out;" in ROWS
assert "if (near_count < 0) near_count = 0;" in ROWS
assert "if (distant_count < 0) distant_count = 0;" in ROWS
assert "max_lines >= 1 + (near_count > 0 ? 1 : 0) + 3" in ROWS
assert "available - (out.show_distant ? 1 : 0)" in ROWS
assert "out.near_rows = angdroid_fa_list_take_rows(" in ROWS
assert "out.distant_rows = angdroid_fa_list_take_rows(" in ROWS

section = SOURCE.split("static void object_list_format_section(", 1)[1].split(
    "static bool object_list_format_special(", 1)[0]
assert "bool show_overflow, size_t *max_width_result)" in section
assert "angdroid_fa_list_undisplayed(" in section
assert "list->total_entries[section], line_count" in section
assert "if (!show_overflow || remaining_object_total <= 0) return;" in section
assert "total - entry_index" not in section
assert "textblock_append(tb, zh_tw ?" in section
assert "object_list_format_name(&list->entries[entry_index]" in section
assert "angdroid_utf8_clip_columns(line_buffer, full_width);" in section
assert "angdroid_list_relative_location(location, sizeof(location)," in section

formatall = SOURCE.split("static void object_list_format_textblock(", 1)[1].split(
    "void object_list_show_subwindow(", 1)[0]
assert "angdroid_fa_list_row_budget(" in formatall
assert "budget.near_rows" in formatall
assert "budget.distant_rows" in formatall
assert "budget.near_overflow" in formatall
assert "budget.distant_overflow" in formatall
assert "if (max_lines <= 0)" in formatall
assert "if (budget.show_distant)" in formatall
assert "if (tb) textblock_append(tb, \"\\n\");" in formatall
assert "lines_remaining" not in formatall
assert "abs(lines_remaining)" not in formatall
assert "if (list == NULL || list->entries == NULL) return;" in formatall
assert "object_list_format_special(" in formatall
assert "angdroid_zh_tw_enabled() ? \"視野內\"" in formatall
assert "angdroid_zh_tw_enabled() ? \"感知範圍內\"" in formatall

# A resize clears the store before drawing the frame. Previously rows did
# not reappear unless the selection cursor also moved (menu driver behavior).
resize = STORE.split("static void store_menu_resize(", 1)[1].split(
    "\n}\n", 1)[0]
assert "store_display_recalc(ctx);" in resize
assert "STORE_FRAME_CHANGE | STORE_GOLD_CHANGE" in resize
assert "store_redraw(ctx);" in resize
assert "menu_refresh(m, false);" in resize
assert resize.index("store_redraw(ctx);") < resize.index("menu_refresh(m, false);")
assert "m->active.page_rows > 0" in resize
assert "m->active.width > 0" in resize
assert "m->active.page_rows <= Term->hgt - m->active.row" in resize
assert "m->active.width <= Term->wid - m->active.col" in resize
assert "store_menu_handle,\n\tstore_menu_resize" in STORE
assert "menu->row_funcs->resize(menu);" in MENU
assert "if (cursor != menu->cursor)" in MENU

recalc = STORE.split("static void store_display_recalc(", 1)[1].split(
    "\n}\n", 1)[0]
assert "angdroid_store_safe_row(hgt - 2, hgt);" in recalc
assert "angdroid_store_safe_row(content_height - 1, hgt);" in recalc
assert "angdroid_store_safe_row(hgt - 1, hgt);" not in recalc
# Gold uses a separate final row in compact mode; help is the row above.
assert "ctx->scr_places_y[LOC_HELP_PROMPT]" in recalc
assert "ctx->scr_places_y[LOC_AU]" in recalc
redraw = STORE.split("static void store_redraw(", 1)[1].split(
    "\n}\n", 1)[0]
assert "else if (Term->hgt >= 7)" in redraw
assert "ctx->scr_places_y[LOC_HELP_PROMPT]" in redraw
assert "ctx->scr_places_y[LOC_AU]" in redraw
assert "fa_store_put_clipped(COLOUR_WHITE, gold_text," in redraw
assert "Gold Remaining: %9ld" in redraw
assert "剩餘金幣：%9ld" in redraw

assert "exhaustive_small_layouts();" in TEST
assert "known_tight_screens();" in TEST
assert "exact_section_counts();" in TEST
assert "large_values();" in TEST
assert "for (h = -2; h <= 24; ++h)" in TEST
assert "for (near = 0; near <= 36; ++near)" in TEST
assert "for (far = 0; far <= 36; ++far)" in TEST
assert "angdroid_fa_list_row_budget(INT_MAX, INT_MAX, INT_MAX)" in TEST
assert "angdroid_fa_list_undisplayed(9, 1) == 8" in TEST
assert "test_faangband_object_list_row_budget.c" in NATIVE
assert "test_faangband_object_list_row_budget_source.py" in VERIFY
print("PASS: FA short list counts, row bounds and store resize/footers")
