#!/usr/bin/env python3
"""Guard dynamic popups so g means Buy and cancel cannot select Examine."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
menu = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
header = (ROOT / "app/src/main/cpp/angband/ui-menu.h").read_text(encoding="utf-8")
store = (ROOT / "app/src/main/cpp/angband/ui-store.c").read_text(encoding="utf-8")

if '#include "menu-dynamic-choice.h"' not in menu:
    raise SystemExit("FAIL: dynamic selection guard not included")
if "int menu_dynamic_select_alias(struct menu *m, char alias_key, char target_key);" not in header:
    raise SystemExit("FAIL: dynamic selection alias API not declared")

display = menu.split("static void dynamic_display(", 1)[1].split(
    "\nstatic const menu_iter dynamic_iter", 1
)[0]
if "menu_put_clipped(col, row, width, color, entry->text);" not in display:
    raise SystemExit("FAIL: translated dynamic context text overflows terminal cells")
if "Term_putstr(col, row, width, color, entry->text);" in display:
    raise SystemExit("FAIL: old codepoint-limited dynamic display remains")

selected = menu.split("int menu_dynamic_select_alias(", 1)[1].split(
    "\nvoid menu_dynamic_free(", 1
)[0]
for token in (
    "e.type == EVT_KBRD && alias_key && target_key",
    "strchr(m->switch_keys, alias_key)",
    "target.code = (unsigned char)target_key",
    "alias_row = get_cursor_key(m, m->top, target)",
    "angdroid_menu_dynamic_choice_index(",
    "e.type == EVT_SELECT",
    "cursor < 0 || !is_valid_row(m, cursor)",
    "return menu_dynamic_select_alias(m, 0, 0);",
):
    if token not in selected:
        raise SystemExit(f"FAIL: dynamic selection ambiguous event handling: {token}")
if "if (e.type == EVT_ESCAPE)" in selected and "return -1" not in selected:
    raise SystemExit("FAIL: selection no longer safely cancels")

context = store.split("static bool context_menu_store_item(", 1)[1].split(
    "\n/**\n * Handle store menu input", 1
)[0]
for token in (
    'm->switch_keys = "g";',
    "selected = menu_dynamic_select_alias(m, 'g', 'p');",
    "case ACT_BUY:",
    "case ACT_BUY_ONE:",
    "case ACT_EXAMINE:",
):
    if token not in context:
        raise SystemExit(f"FAIL: store g shortcut loses correct action: {token}")
if "handle_g_context_store_item" in store:
    raise SystemExit("FAIL: unsafe row-handler based key replacement remains")

print("PASS: dynamic menus validate SELECT and g aliases Buy; Chinese text clipped")
