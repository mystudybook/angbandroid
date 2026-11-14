#!/usr/bin/env python3
"""Ensure native menu input, on-screen tags and keyboard hints share bounds.

The C99 helper behavior is checked by test_menu_visible_tags.c.
This is a source integration guard, not an Android emulator test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
tag_header = '#include "menu-visible-tags.h"'
if tag_header not in source:
    raise SystemExit("FAIL: visible menu tag helpers are not included")

selection = source.split("static int get_cursor_key(", 1)[1].split(
    "static menu_row_style_t", 1
)[0]
hints = source.split("static void keys_to_ui(", 1)[1].split(
    "\n/**\n * Run a menu.", 1
)[0]

for label, body in (("keypress selection", selection), ("soft keyboard hints", hints)):
    for expression in (
        "angdroid_menu_visible_tag_first(",
        "angdroid_menu_visible_tag_end(",
        "menu->skin != &menu_skin_column",
        "menu->skin->get_tag(menu, i)" if label == "keypress selection"
        else "menu->skin->get_tag(menu, cursor)",
    ):
        if expression not in body:
            raise SystemExit(f"FAIL: {label} lacks {expression}")

# Only an actually drawn relative label can activate a row. A hidden
# row keeps its explicit (absolute) tag, but is absent from the soft keyboard
# and cannot be invoked by a relative shortcut.
relative = selection.split("} else if (menu->flags & MN_REL_TAGS) {", 1)[1].split(
    "} else if (!(menu->flags & MN_PVT_TAGS)", 1
)[0]
if "if (!menu_navigation_is_valid(menu, i))" not in relative:
    raise SystemExit("FAIL: a hidden relative tag can trigger an action")
if relative.index("if (!menu_navigation_is_valid(menu, i))") > relative.index(
    "c = menu->skin->get_tag(menu, i);"
):
    raise SystemExit("FAIL: relative key was read before the visibility check")
if "menu->inscriptions &&" not in selection:
    raise SystemExit("FAIL: digit alias lookup can dereference a missing map")

if "return i + menu->top;" in selection:
    raise SystemExit("FAIL: relative selection incorrectly adds page offset twice")
if "for (i = 0; menu->selections[i]; i++)" in selection:
    raise SystemExit("FAIL: nonrelative tags may select an item beyond menu count")
if "for (cursor = 0; cursor < n; cursor++)" in hints:
    raise SystemExit("FAIL: keyboard still lists offscreen relative shortcuts")

for skin in ("scroll_get_tag", "object_skin_get_tag", "column_get_tag"):
    fragment = source.split(f"static char {skin}(", 1)[1].split("\n}", 1)[0]
    if "angdroid_menu_tag_at(" not in fragment:
        raise SystemExit(f"FAIL: {skin} may read beyond the tag string")

if "sel = menu->selections[pos];" in source:
    raise SystemExit("FAIL: displayed menu tag is not bounds-checked")

print("PASS: native relative menu keypress, display and keyboard hints share safe ranges")
