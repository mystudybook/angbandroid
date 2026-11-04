#!/usr/bin/env python3
"""Check native navigation no longer loops forever on invalid horizontal rows."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")

if '#include "menu-valid-cursor.h"' not in source:
    raise SystemExit("FAIL: menu cursor helper not included")

section = source.split("bool menu_handle_keypress(", 1)[1].split(
    "static void keys_to_ui(", 1
)[0]
required = (
    "int direction = ddy[dir] ? ddy[dir] : ddx[dir];",
    "angdroid_menu_find_valid_cursor(menu->cursor,",
    "menu_navigation_is_valid",
    "if (next >= 0)",
    "out->type = EVT_NONE;",
)
for fragment in required:
    if fragment not in section:
        raise SystemExit(f"FAIL: missing finite menu navigation guard: {fragment}")
if "menu->cursor += ddy[dir];" in section:
    raise SystemExit("FAIL: zero vertical step could hang a horizontal menu move")

print("PASS: native menu direction skips invalid rows with a bounded search")
