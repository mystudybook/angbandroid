#!/usr/bin/env python3
"""Modal item pickers discard their soft keys on every exit path.

Verify both engine variants, with the real curses buffer replacement as a
separate C99 regression. Source checks are not APK/device acceptance tests.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
CURSES = CPP / "curses"
DROID = (CURSES / "droid.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_soft_kbd_replace.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

replace = DROID.split("void soft_kbd_replace(", 1)[1].split(
    "void soft_kbd_linger(", 1
)[0]
assert "angdroid_soft_kbd_replace_buffer(" in replace
assert "flashing_keys = true;" in replace
assert 'Term_control_keys("${clear}");' in replace
assert "if (!length)" in replace

for variant, early_returns in (("angband", 3), ("faangband", 2)):
    code = (CPP / variant / "ui-menu.c").read_text(encoding="utf-8")
    select = code.split("ui_event menu_select(", 1)[1].split(
        "/* ================== MENU ACCESSORS", 1
    )[0]

    # The old item's buttons must not briefly remain active while the
    # next allowed inventory/equipment/floor picker is being created.
    switch = select.split("/* Switch key */", 1)[1].split(
        "menu_handle_keypress(menu, &in, &out);", 1
    )[0]
    assert "menu_handle_action(menu, &in);" in switch, variant
    assert 'if (menu->skin == &menu_skin_object)' in switch, variant
    assert 'soft_kbd_replace("");' in switch, variant
    assert switch.index('soft_kbd_replace("");') < switch.index(
        "return in;"
    ), variant

    # Selection, Escape and direction-switch notification return through
    # this branch, which must clear the old modal's keys exactly once.
    notify = select.split("if (notify & out.type) {", 1)[1].split(
        "return out;", 1
    )[0]
    assert 'if (menu->skin == &menu_skin_object)' in notify, variant
    assert 'soft_kbd_replace("");' in notify, variant

    # Also cover the loop's regular return after the input event loop
    # terminates (for example, an intercepted menu action).
    final = select.rsplit("if (popup)\n\t\tscreen_load();", 1)[1]
    assert 'if (menu->skin == &menu_skin_object)' in final, variant
    assert 'soft_kbd_replace("");' in final, variant
    assert final.index('soft_kbd_replace("");') < final.index(
        "return in;"
    ), variant

    # An invalid initial or rotated viewport was handled previously;
    # retain that cleanup and do not change non-object menus.
    before_escape = re.findall(
        r'if \(menu->skin == &menu_skin_object\)\s*'
        r'soft_kbd_replace\(""\);\s*'
        r'in\.type = EVT_ESCAPE;\s*return in;',
        select
    )
    assert len(before_escape) == early_returns, (
        variant, len(before_escape)
    )

    hints = code.split("static void keys_to_ui(", 1)[1].split(
        "/**\n * Run a menu.", 1
    )[0]
    assert 'soft_kbd_replace(buf);' in hints
    assert 'soft_kbd_replace("");' in hints
    assert select.index("keys_to_ui(menu);") < select.index("in = inkey_ex();")

for marker in (
    "picker_exit_then_game_prompt();",
    "replaces_previous_hints();",
    "bounded_and_deduplicated();",
    "transitions_and_full_capacity();",
    'char keys[64] = "";',
    'CHECK(keys[0] == \'\\0\');',
):
    assert marker in NATIVE, marker
assert "test_soft_kbd_replace.c" in RUN
assert "test_item_picker_soft_key_exit_source.py" in VERIFY
print("PASS: both engines clear item soft keys on switch, select and cancel")
