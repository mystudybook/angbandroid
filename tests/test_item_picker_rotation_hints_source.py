#!/usr/bin/env python3
"""Prevent stale Android shortcut hints during invalid item picker rotation.

Integration contract for Angband and FAangband. This is a source check,
not a substitute for Android NDK or device testing.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for variant, early_exits in (("angband", 3), ("faangband", 2)):
    code = (CPP / variant / "ui-menu.c").read_text(encoding="utf-8")
    hints = code.split("static void keys_to_ui(struct menu *menu)", 1)[1].split(
        "/**\n * Run a menu.", 1
    )[0]
    select = code.split("ui_event menu_select(", 1)[1].split(
        "/* ================== MENU ACCESSORS", 1
    )[0]

    # Before reading any visible item tag, clear keys from the previous
    # orientation if its region is no longer drawable.
    if variant == "angband":
        assert "menu->preferred_skin == &menu_skin_object" in hints
        for guard in (
            "!Term", "menu->active.col < 0", "menu->active.row < 0",
            "menu->active.width <= 0", "menu->active.page_rows <= 0",
            "menu->active.col >= Term->wid",
            "menu->active.row >= Term->hgt",
            "menu->active.width > Term->wid - menu->active.col",
            "menu->active.page_rows > Term->hgt - menu->active.row",
        ):
            assert guard in hints, (variant, guard)
    else:
        assert "menu->skin == &menu_skin_object" in hints
        assert "!angdroid_fa_menu_region_visible(" in hints
        assert "bool paged;" in hints
        assert "paged = fa_menu_relative_tags_paged(menu);" in hints
        assert hints.index('soft_kbd_replace("");') < hints.index(
            "paged = fa_menu_relative_tags_paged(menu);"
        )

    assert 'soft_kbd_replace("");' in hints
    assert "return;" in hints
    assert hints.index('soft_kbd_replace("");') < hints.index(
        "for (cursor = first; cursor < end; "
    )
    assert "soft_kbd_replace(buf);" in hints
    assert select.index("menu_refresh(menu, popup);") < select.index(
        "keys_to_ui(menu);"
    )

    # Every invalid-layout early escape clears stale picker-only keys.
    # Valid item menus and non-item menus retain their standard key strip.
    cleared = re.findall(
        r'if \(menu->skin == &menu_skin_object\)\s*'
        r'soft_kbd_replace\(""\);\s*'
        r'in\.type = EVT_ESCAPE;\s*return in;',
        select,
    )
    assert len(cleared) == early_exits, (variant, len(cleared))
    assert "menu->skin->display_list(" in code

assert "test_item_picker_rotation_hints_source.py" in VERIFY
print("PASS: invalid item viewport clears stale soft keys in both engines")
