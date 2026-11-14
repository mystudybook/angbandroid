#!/usr/bin/env python3
"""Decorative quiver footer survives empty inventory and Android rotations.

These are source integration checks. C99 test_item_selection_layout
separately verifies the actual bounds and footer slot arithmetic.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
LAYOUT = (CPP / "angband/item-selection-layout.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_selection_layout.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")

for variant in ("angband", "faangband"):
    folder = CPP / variant
    menu_h = (folder / "ui-menu.h").read_text(encoding="utf-8")
    menu = (folder / "ui-menu.c").read_text(encoding="utf-8")
    ui = (folder / "ui-object.c").read_text(encoding="utf-8")

    assert "MN_BROWSE_WITHOUT_SELECTION = 0x200" in menu_h
    refresh = menu.split("void menu_refresh(", 1)[1].split(
        "/*** MENU RUNNING AND INPUT HANDLING CODE", 1
    )[0]
    assert "(oid >= 0 || (menu->flags & MN_BROWSE_WITHOUT_SELECTION))" in refresh
    assert "menu->browse_hook(oid, menu->menu_data, loc);" in refresh
    assert "menu->skin->display_list(menu, menu->cursor, &menu->top, loc);" in refresh
    assert refresh.index("menu->browse_hook(oid, menu->menu_data, loc);") < (
        refresh.index("menu->skin->display_list(")
    )

    picker = ui.split("static struct object *item_menu(", 1)[1].split(
        "/* Deal with menu switch */", 1
    )[0]
    assert "m->browse_hook = item_menu_browser;" in picker
    assert "(mode & OLIST_QUIVER)" in picker
    assert "player->upkeep->command_wrk == USE_INVEN" in picker
    assert "player->upkeep->quiver_cnt > 0" in picker
    assert "m->flags |= MN_BROWSE_WITHOUT_SELECTION;" in picker
    assert "MN_MOUSE_ROW_ACTION" not in picker

    resize_name = ("item_menu_resize" if variant == "angband"
                   else "fa_item_menu_resize")
    resize = ui.split("static void " + resize_name + "(", 1)[1].split(
        "static struct object *item_menu(", 1
    )[0]
    assert "footer = (olist_mode & OLIST_QUIVER)" in resize
    assert "player->upkeep->quiver_cnt > 0" in resize
    assert "angdroid_item_selection_region(width, height," in resize

    browser = ui.split("static void item_menu_browser(", 1)[1].split(
        "static void " + resize_name + "(", 1
    )[0]
    assert "angdroid_item_quiver_footer_slots(" in browser
    assert "if (quiver_slots <= 0)" in browser
    assert "angdroid_item_selection_footer_row(" in browser
    assert "if (line < 0 || i >= (int)strlen(all_letters_nohjkl))" in browser
    assert "local_area->row + i" not in browser

assert "static inline int angdroid_item_quiver_footer_slots(" in LAYOUT
assert "if (item_count <= 0 || slot_size <= 0)" in LAYOUT
assert "return item_count / slot_size + (item_count % slot_size != 0);" in LAYOUT

for fragment in (
    "angdroid_item_selection_viewport_count(0, true), true, &l",
    "angdroid_item_selection_footer_row(l.row, l.page_rows, 12, 0) == 2",
    "2, l.row, l.page_rows, 0, 0) == -1",
    "angdroid_item_quiver_footer_slots(INT_MAX, 1) == INT_MAX",
    "angdroid_item_quiver_footer_slots(INT_MAX, INT_MAX) == 1",
    "for (count = -2; count <= 256; ++count)",
):
    assert fragment in NATIVE, fragment
assert "test_item_selection_layout.c" in NATIVE_RUNNER
assert "test_empty_quiver_footer_source.py" in VERIFY
print("PASS: empty quiver footer refresh, rotation geometry and no phantom taps")
