#!/usr/bin/env python3
"""Native Angband and FAangband column-major direction consistency.

Source contract; production helper behavior is verified separately by C99.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
SHARED = (CPP / "angband/menu-column-navigation.h").read_text(encoding="utf-8")
C99 = (ROOT / "tests/native/test_menu_column_navigation.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "menu-valid-cursor.h"' in SHARED
assert "static inline int angdroid_menu_column_next_valid(" in SHARED
for part in (
    "if (count <= 0 || page_rows <= 0 || !is_valid",
    "columns = 1 + (count - 1) / page_rows;",
    "base = next_column * page_rows;",
    "row < available ? row : available - 1",
    "if (candidate != cursor && is_valid(context, candidate))",
    "int base = column * page_rows;",
    "if (is_valid(context, base + next_row))",
):
    assert part in SHARED, part
assert "cursor += dx * page_rows" not in SHARED

for folder in ("angband", "faangband"):
    ui = (CPP / folder / "ui-menu.c").read_text(encoding="utf-8")
    include = ('#include "menu-column-navigation.h"' if folder == "angband"
               else '#include "../angband/menu-column-navigation.h"')
    assert include in ui
    action = ui.split("static ui_event column_process_direction(", 1)[1].split(
        "/* Virtual function table for multi-column menu skin */", 1
    )[0]
    assert "angdroid_menu_column_next_valid(" in action
    assert "if (count <= 0 || m->active.page_rows <= 0)" in action
    assert "if (next < 0 || next == m->cursor)" in action
    assert "m->cursor = next;" in action
    assert "out.type = EVT_MOVE;" in action
    assert "m->cursor += ddx[dir] * rows_per_page" not in action
    assert "m->cursor = n - 1" not in action
    assert action.index("angdroid_menu_column_next_valid(") < action.index(
        "m->cursor = next;")
    callback = ("menu_navigation_is_valid" if folder == "angband"
                else "fa_menu_navigation_is_valid")
    assert f"m, {callback});" in action
    nav = ui.split(f"static int {callback}(void *context, int row)\n{{", 1)[1].split(
        "\n}\n", 1)[0]
    assert "MN_ROW_VALID" in nav
    assert "menu->row_funcs->valid_row(menu, oid)" in nav
    assert "oid < 0" in nav
    keyboard = ui.split("bool menu_handle_keypress(", 1)[1].split(
        "static void keys_to_ui(", 1)[0]
    assert "angdroid_menu_find_valid_cursor(" in keyboard
    assert callback in keyboard

for marker in (
    "small_grids_all_visibility_masks();",
    "short_final_column_and_hidden_rows();",
    "extreme_indices_and_empty_cursor();",
    "INT_MAX",
    "INT_MIN",
):
    assert marker in C99, marker
assert "test_menu_column_navigation.c" in RUN
assert "test_menu_column_navigation_source.py" in VERIFY
print("PASS: both variants use grid-aligned navigation with hidden rows excluded")
