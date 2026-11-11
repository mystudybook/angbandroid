#!/usr/bin/env python3
"""A stock tap needs raw EVT_MOUSE; an object tap needs one EVT_SELECT.

Static source integration checks. These do not replace Android device tests.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1] / "app/src/main/cpp"

for game in ("angband", "faangband"):
    folder = ROOT / game
    menu_h = (folder / "ui-menu.h").read_text(encoding="utf-8")
    ui = (folder / "ui-menu.c").read_text(encoding="utf-8")
    store = (folder / "ui-store.c").read_text(encoding="utf-8")
    items = (folder / "ui-object.c").read_text(encoding="utf-8")

    assert "MN_MOUSE_ROW_ACTION = 0x100" in menu_h
    select = ui.split("ui_event menu_select(", 1)[1].split(
        "/* ================== MENU ACCESSORS", 1)[0]
    mouse = select.split("if (in.type == EVT_MOUSE) {", 1)[1].split(
        "} else if (in.type == EVT_KBRD)", 1)[0]
    active = mouse.split("if (region_inside(&menu->active, &in)) {", 1)[1].split(
        "} else {", 1)[0]

    assert "menu_handle_mouse(menu, &in, &out);" in active
    assert "out.type == EVT_SELECT && !no_act &&" in active
    assert "(menu->flags & MN_MOUSE_ROW_ACTION)" in active
    assert "touch_select_dispatched = true;" in active
    assert "menu_handle_action(menu, &in)" in active
    assert active.index("menu_handle_mouse(menu, &in, &out);") < active.index(
        "(menu->flags & MN_MOUSE_ROW_ACTION)")
    assert active.index("(menu->flags & MN_MOUSE_ROW_ACTION)") < active.index(
        "menu_handle_action(menu, &in)")
    assert "!touch_select_dispatched &&" in select
    assert "menu_handle_action(menu, &out)" in select
    assert select.index("!touch_select_dispatched &&") < select.index(
        "menu_handle_action(menu, &out)")
    assert "if (!no_act && menu_handle_action(menu, &in))" in mouse

    store_init = store.split("static void store_menu_init(", 1)[1].split(
        "void textui_store_knowledge(", 1)[0]
    assert "menu->flags |= MN_ALLOW_EMPTY_INPUT | MN_MOUSE_ROW_ACTION;" in store_init
    handler = store.split("static bool store_menu_handle(", 1)[1].split(
        "static void store_menu_resize(", 1)[0]
    assert "event->type == EVT_MOUSE" in handler
    assert "event->type == EVT_SELECT" in handler
    assert "angdroid_store_menu_screen_row(" in handler

    picker = items.split("static struct object *item_menu(", 1)[1].split(
        "/* Deal with menu switch */", 1)[0]
    assert "MN_PVT_TAGS | MN_INSCRIP_TAGS" in picker
    assert "MN_MOUSE_ROW_ACTION" not in picker
    callback = items.split("static bool get_item_action(", 1)[1].split(
        "static void item_menu_browser(", 1)[0]
    assert "event->type == EVT_SELECT" in callback
    assert "selection = choice[oid].object;" in callback
    assert "event->type == EVT_MOUSE" not in callback

print("PASS: Angband/FA stock taps use mouse, object taps use single SELECT")
