#!/usr/bin/env python3
"""FAangband spell menu must fit narrow/short terminals and show description."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPELL = (ROOT / "app/src/main/cpp/faangband/ui-spell.c").read_text(
    encoding="utf-8")
HELPER = (ROOT / "app/src/main/cpp/faangband/ui-spell-viewport.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_fa_spell_viewport.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")

def has(source, *fragments):
    for fragment in fragments:
        assert fragment in source, fragment

browse = SPELL.split("static void spell_menu_browser(", 1)[1].split(
    "static const menu_iter spell_menu_iter", 1)[0]
new_menu = SPELL.split("static struct menu *spell_menu_new(", 1)[1].split(
    "static void spell_menu_destroy(", 1)[0]
select = SPELL.split("static int spell_menu_select(", 1)[1].split(
    "static void spell_menu_browse(", 1)[0]
browse_menu = SPELL.split("static void spell_menu_browse(", 1)[1].split(
    "void textui_book_browse(", 1)[0]

has(SPELL,
    '#include "ui-spell-viewport.h"',
    'angdroid_zh_tw_spell_name(spell->name, angdroid_zh_tw_enabled())',
    'angdroid_zh_tw_spell_description(',
    'fa_spell_menu_header(width)')
has(new_menu,
    'width = angdroid_fa_spell_menu_columns(Term->wid);',
    'page_rows = angdroid_fa_spell_menu_rows(Term->hgt, d->n_spells);',
    'if (page_rows == 0 || !menu_layout(m, &loc)) {',
    'mem_free(d->spells);',
    'mem_free(d);',
    'mem_free(m);',
    'loc.col = -width;',
    'loc.row = 1;',
    'loc.width = width;')
assert "Term->wid - 15" not in new_menu
assert "d->n_spells + 1" not in new_menu
has(browse,
    'oid < 0 || oid >= d->n_spells',
    'browse_row = loc->row + loc->page_rows;',
    'if (browse_row < 0 || browse_row >= Term->hgt) return;',
    'if (!spell) return;',
    'Term_gotoxy(loc->col, browse_row);',
    'text_out("%s", angdroid_zh_tw_spell_description(',
    'text_out_pad = 0;',
    'text_out_indent = 0;',
    'effect_avg_damage(e, shared_dice)',
    'PY_SPELL_WORKED',
    'PY_SPELL_FORGOTTEN')
assert 'text_out("\\n%s",' not in browse
has(select, "if (!Term) return -1;", "if (Term) screen_load();",
    "return d->selected_spell;")
has(browse_menu, "if (!Term) return;", "if (Term) screen_load();")
has(HELPER, "angdroid_fa_spell_menu_columns(",
    "angdroid_fa_spell_menu_rows(",
    "spell_count >= maximum - 1 ? maximum : spell_count + 1;")
has(NATIVE,
    "angdroid_fa_spell_menu_rows(4, INT_MAX) == 2",
    "for (height = 0; height <= 180; ++height)")
assert "test-fa-spell-viewport.sh" in VERIFY
assert "test_faangband_spell_menu_viewport_source.py" in VERIFY
print("PASS: FAangband spell menu width, browse description and C99 wiring")
