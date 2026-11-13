#!/usr/bin/env python3
"""Guard FAangband ignore popup UTF-8 prompt and rotated menu viewport.

The popup is display-only: user action IDs, selections and save data must
remain unchanged. This checks the production body, not copied UI strings.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PATH = ROOT / "app/src/main/cpp/faangband/ui-object.c"
SOURCE = PATH.read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

anchor = 'r.width = (int)MIN(menu_dynamic_longest_entry(m) + 3 + 2,'
assert SOURCE.count(anchor) == 1
start = SOURCE.index('/* A rotated phone can temporarily have fewer rows')
end = SOURCE.index('soft_kbd_save_linger(&parent_keys);', start)
popup = SOURCE[start:end]

checks = (
    '#include "../angband/menu-text-fit.h"',
    'Term->wid < 3 || Term->hgt < 4 || m->count <= 0',
    'menu_dynamic_free(m);',
    'r.width = (int)MIN(menu_dynamic_longest_entry(m) + 3 + 2,',
    '(size_t)MAX(1, Term->wid - 2)',
    'r.col = MAX(0, Term->wid - r.width - 1);',
    'r.row = 1;',
    'r.page_rows = MIN(m->count, MAX(1, Term->hgt - 3));',
    'menu_layout(m, &r);',
    'region_erase_bordered(&r);',
    'char prompt_visible[128];',
    'angdroid_menu_copy_columns(prompt_visible, sizeof(prompt_visible),',
    '(size_t)(Term->wid - 1)',
    'prt(prompt_visible, 0, 0);',
)
for pattern in checks:
    assert pattern in (SOURCE if pattern.startswith('#include') else popup), pattern
assert popup.index('if (!Term || Term->wid < 3') < popup.index('screen_save();')
assert popup.index('r.page_rows = MIN') < popup.index('menu_layout(m, &r)')
assert popup.index('angdroid_menu_copy_columns(') < popup.index(
    'prt(prompt_visible, 0, 0);')
assert 'prt(angdroid_game_ui_text("(Enter to select, ESC) Ignore:"' not in popup

# Geometry properties including tiny phone rotations, without creating a
# terminal. Make sure a bordered popup never requests rows past the screen.
for height in range(4, 128):
    for entries in (1, 2, 3, 10, 50, 1000):
        page_rows = min(entries, max(1, height - 3))
        assert 1 <= page_rows <= entries
        assert 1 + page_rows + 1 <= height - 1
for width in range(3, 128):
    for longest in (0, 1, 10, 80, 1000):
        menu_width = min(longest + 5, max(1, width - 2))
        col = max(0, width - menu_width - 1)
        assert 0 <= col <= width - 1
        assert col + menu_width <= width - 1

assert 'selected = menu_dynamic_select(m);' in SOURCE
assert 'screen_load();' in SOURCE
assert 'selected == IGNORE_THIS_ITEM' in SOURCE
assert 'selected == IGNORE_THIS_EGO' in SOURCE
assert 'soft_kbd_restore_linger(&parent_keys, selected < 0);' in SOURCE
assert 'test_faangband_ignore_popup_geometry_source.py' in VERIFY
print("PASS: FA ignore popup phone bounds and UTF-8 prompt source contracts")
