#!/usr/bin/env python3
"""Empty inventory/equipment pages must keep tab switching without fake items.

Source-level integration checks; the pure layout helper and blank row hit
behavior are verified by tests/native/test_item_selection_layout.c.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
LAYOUT = (CPP / "angband/item-selection-layout.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_selection_layout.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert "static inline int angdroid_item_selection_viewport_count(" in LAYOUT
assert "if (actual_count > 0) return actual_count;" in LAYOUT
assert "return has_switch_target ? 1 : 0;" in LAYOUT
assert "count <= 0" in LAYOUT  # Normal item geometry still rejects zero rows.

for game in ("angband", "faangband"):
    ui = (CPP / game / "ui-object.c").read_text(encoding="utf-8")
    menu = (CPP / game / "ui-menu.c").read_text(encoding="utf-8")

    picker = ui.split("static struct object *item_menu(", 1)[1].split(
        "/**\n * Let the user select an object", 1
    )[0]
    assert "menu_setpriv(m, num_obj, items);" in picker
    assert "m->switch_keys = switch_keys[0] ? switch_keys : NULL;" in picker
    assert "int selectable_objects = 0;" in picker
    assert "for (row = 0; row < num_obj; ++row)" in picker
    assert "if (items[row].object)" in picker
    assert "++selectable_objects;" in picker
    assert "angdroid_item_picker_allow_blank_switch(" in picker
    assert "selectable_objects, m->switch_keys))" in picker
    assert "m->flags |= MN_ALLOW_EMPTY_INPUT;" in picker
    assert "if (num_obj <= 0 && m->switch_keys)" not in picker
    assert "menu_setpriv(m, 1," not in picker
    assert "evt = menu_select(m, 0, true);" in picker

    resize_name = ("item_menu_resize" if game == "angband"
                   else "fa_item_menu_resize")
    resize = ui.split(f"static void {resize_name}(", 1)[1].split(
        "static struct object *item_menu(", 1
    )[0]
    assert "angdroid_item_selection_region(width, height," in resize
    assert "angdroid_item_selection_viewport_count(" in resize
    assert "m->count, m->switch_keys && m->switch_keys[0]" in resize
    assert "menu_layout(m, &area);" in resize

    # Unselected row is allowed to receive switch keyboard events but
    # must not be dereferenced or reported as an actual item choice.
    action = ui.split("static bool get_item_action(", 1)[1].split(
        "static void item_menu_browser(", 1
    )[0]
    assert "if (event->type == EVT_SELECT)" in action
    assert "if (event->type == EVT_KBRD)" in action
    select, keyboard = action.split("if (event->type == EVT_KBRD)", 1)
    assert "choice[oid].object" in select
    assert "angdroid_item_picker_row_in_range(oid, menu->count)" in select
    tag = ui.split("static char get_item_tag(", 1)[1].split(
        "static int get_item_validity(", 1
    )[0]
    assert "if (!angdroid_item_picker_row_in_range(oid, menu->count))" in tag
    assert "return choice[oid].object ? choice[oid].key : 0;" in tag
    valid = ui.split("static int get_item_validity(", 1)[1].split(
        "static void get_item_display(", 1
    )[0]
    assert "if (!angdroid_item_picker_row_in_range(oid, menu->count))" in valid
    assert "return (choice[oid].object != NULL) ? 1 : 0;" in valid
    assert "choice[oid].object" not in keyboard
    for key in ("key == '/'", "key == '|'", "key == '-'"):
        assert key in keyboard
    assert "newmenu = true;" in keyboard

    dispatch = menu.split("static bool menu_handle_action(", 1)[1].split(
        "bool menu_handle_keypress(", 1
    )[0]
    assert "MN_ALLOW_EMPTY_INPUT" in dispatch
    assert "in->type == EVT_KBRD || in->type == EVT_MOUSE" in dispatch
    assert "row_handler(m, in, -1)" in dispatch
    select_loop = menu.split("ui_event menu_select(", 1)[1].split(
        "void menu_setpriv(", 1
    )[0]
    assert "menu->switch_keys &&" in select_loop
    assert "menu_handle_action(menu, &in);" in select_loop
    assert "return in;" in select_loop

    # C strchr() matches the string terminator for keycode 0. A
    # Unicode/special key might also alias an ASCII control when cast
    # to char. Only actual one-byte nonzero codes can use shortcut lists.
    assert menu.count("angdroid_menu_shortcut_is_byte(in.key.code)") == 2
    key_scan = menu.split("static int get_cursor_key(", 1)[1].split(
        "static menu_row_style_t", 1
    )[0]
    assert "if (!angdroid_menu_shortcut_is_byte(key.code))" in key_scan
    assert menu.index("if (!angdroid_menu_shortcut_is_byte(key.code))") < (
        menu.index("if (menu->flags & MN_CASELESS_TAGS)",
                   menu.index("static int get_cursor_key("))
    )

    # An empty inventory is displayed with zero actual selectable rows.
    # Left/right arrows must still become EVT_SWITCH if another tab exists.
    keyboard = menu.split("bool menu_handle_keypress(", 1)[1].split(
        "static void keys_to_ui(", 1
    )[0]
    empty = keyboard.split("} else if (count <= 0) {", 1)[1].split(
        "} else if (in->key.code == ' ')", 1
    )[0]
    for marker in (
        "(menu->flags & MN_ALLOW_EMPTY_INPUT)",
        "menu->skin == &menu_skin_object",
        "menu->switch_keys",
        "int dir = target_dir(in->key);",
        "dir && angdroid_item_picker_blank_arrow_switch(",
        "menu->flags, MN_ALLOW_EMPTY_INPUT, true,",
        "menu->switch_keys != NULL, ddx[dir], ddy[dir]",
        "*out = menu->skin->process_dir(menu, dir);",
        "eat = true;",
    ):
        assert marker in empty, (game, marker)
    assert "m->cursor" not in empty, game

    # SHOW_EMPTY equipment has count > 0, but each visible row may
    # contain a NULL object. Its left/right arrow must still switch
    # to another allowed source; invalid rows remain unselectable.
    directions = keyboard.split("} else {", 1)[-1]
    assert "if (dir && !no_valid_row(menu, count))" in keyboard
    assert "} else if (dir && angdroid_item_picker_blank_arrow_switch(" in keyboard
    assert "menu->skin == &menu_skin_object" in directions
    assert "menu->switch_keys != NULL, ddx[dir], ddy[dir]" in directions
    assert "*out = menu->skin->process_dir(menu, dir);" in directions

    # When neither arrow can reach a different source, do not rebuild
    # the identical empty menu in an unbounded loop.
    switch = picker.split("/* Deal with menu switch */", 1)[1].split(
        "/* Result */", 1
    )[0]
    assert "int original_source = player->upkeep->command_wrk;" in switch
    assert "newmenu = (player->upkeep->command_wrk != original_source);" in switch
    assert "newmenu = true;" not in switch

for marker in (
    "angdroid_item_selection_viewport_count(0, false) == 0",
    "angdroid_item_selection_viewport_count(0, true) == 1",
    "angdroid_item_selection_viewport_count(INT_MAX, true) == INT_MAX",
    "l.page_rows == 1",
    "l.row, l.row, l.page_rows, 0, 0) == -1",
):
    assert marker in NATIVE, marker
assert "test_item_selection_layout.c" in RUNNER
assert "test_empty_item_picker_source.py" in VERIFY

print("PASS: empty picker tabs retain switch controls without selectable rows")
