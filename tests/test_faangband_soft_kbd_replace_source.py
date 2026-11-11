#!/usr/bin/env python3
"""FA modal menu hints replace old keys across paging, filtering and rotation."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CURSES = ROOT / "app/src/main/cpp/curses"
FA = ROOT / "app/src/main/cpp/faangband"
HEADER = (CURSES / "soft-kbd-replace.h").read_text(encoding="utf-8")
DROID = (CURSES / "droid.c").read_text(encoding="utf-8")
DROID_H = (CURSES / "droid.h").read_text(encoding="utf-8")
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_soft_kbd_replace.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "soft-kbd-replace.h"' in DROID
assert "angdroid_soft_kbd_replace_buffer(" in HEADER
assert "if (!output || capacity == 0) return 0;" in HEADER
assert "output[0] = '\\0';" in HEADER
assert "output[used] = '\\0';" in HEADER
assert "if (!keys) return 0;" in HEADER
assert "if (used >= capacity - 1) break;" in HEADER

replace = DROID.split("void soft_kbd_replace(", 1)[1].split(
    "void soft_kbd_linger(", 1)[0]
assert "angdroid_soft_kbd_replace_buffer(" in replace
assert "flashing_keys = true;" in replace
assert 'Term_control_keys("${clear}");' in replace
assert "my_strcat(soft_kbd_buffer" not in replace
assert "extern void soft_kbd_replace(const char *keys);" in DROID_H

hints = MENU.split("static void keys_to_ui(", 1)[1].split(
    "ui_event menu_select(", 1)[0]
assert "strdeldup(buf);" in hints
assert "soft_kbd_replace(buf);" in hints
assert "if (buf[0]) soft_kbd_flash(buf);" not in hints
assert "angdroid_menu_visible_tag_first(" in hints
assert "angdroid_menu_visible_tag_end(" in hints
assert "fa_menu_navigation_is_valid(menu, cursor)" in hints
assert "menu->cmd_keys" in hints and "menu->switch_keys" in hints

select = MENU.split("ui_event menu_select(", 1)[1].split(
    "const menu_iter *menu_find_iter(", 1)[0]
assert "menu_refresh(menu, popup);" in select
assert "keys_to_ui(menu);" in select
assert select.index("menu_refresh(menu, popup);") < select.index("keys_to_ui(menu);")
assert select.index("keys_to_ui(menu);") < select.index("in = inkey_ex();")
assert "in.type == EVT_RESIZE" in select
assert "menu_ensure_cursor_valid(menu);" in select

assert "replaces_previous_hints();" in NATIVE
assert "bounded_and_deduplicated();" in NATIVE
assert "transitions_and_full_capacity();" in NATIVE
assert "test_soft_kbd_replace.c" in RUN
assert "test_faangband_soft_kbd_replace_source.py" in VERIFY
print("PASS: FA modal hints replace stale Android keys on filter, page and resize")
