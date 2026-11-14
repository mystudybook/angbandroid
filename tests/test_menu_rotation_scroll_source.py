#!/usr/bin/env python3
"""Native menu scroll and disabled-action safety across rotated viewports.

Pure source contract; the production viewport helper is executed by the
separate native C99 test, not by this Python script.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
ANG = (CPP / "angband/ui-menu.c").read_text(encoding="utf-8")
FA = (CPP / "faangband/ui-menu.c").read_text(encoding="utf-8")
HEADER = (CPP / "angband/menu-scroll-viewport.h").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_menu_scroll_viewport.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

assert '#include "menu-scroll-viewport.h"' in ANG
assert "static inline int angdroid_menu_scroll_clamp_top(" in HEADER
assert "if (count <= 0 || page_rows <= 0)" in HEADER
assert "top > max_top" in HEADER
assert "cursor - top >= page_rows" in HEADER

for begin, end in (
    ("static void display_scrolling(", "static char scroll_get_tag("),
    ("static void object_skin_display(", "static char object_skin_get_tag("),
):
    region = ANG.split(begin, 1)[1].split(end, 1)[0]
    assert "angdroid_menu_scroll_clamp_top(" in region
    assert "if (*top < n && i < n - *top)" in region
    assert "if (i < n) {" not in region
    assert "if (cursor >= *top && cursor < n &&" in region
    assert "cursor - *top < rows_per_page" in region
    assert "menu->cursor_x_offset < loc->width" in region
    assert "Term_gotoxy(" in region

select = ANG.split("ui_event menu_select(", 1)[1].split(
    "/* ================== MENU ACCESSORS", 1)[0]
assert select.count("menu_ensure_cursor_valid(menu);") >= 2
assert "if (menu->active.width <= 0 || menu->active.page_rows <= 0)" in select
assert "menu->row_funcs->resize(menu);" in select

# FAangband also must not execute gray action selected from a filtered menu.
assert '#include "../angband/menu-action-guard.h"' in FA
fa_action = FA.split("static bool menu_action_handle(", 1)[1].split(
    "/**\n * Virtual function table", 1)[0]
assert "angdroid_menu_action_can_run(acts[oid].flags," in fa_action
assert "MN_ACT_GRAYED, acts[oid].action != NULL" in fa_action
assert "acts->flags & MN_ACT_GRAYED" not in fa_action

for snippet in (
    "exhaustive_small_viewports();",
    "boundary_cases();",
    "angdroid_menu_scroll_clamp_top(",
    "INT_MAX",
    "INT_MIN",
):
    assert snippet in TEST, snippet
assert "test_menu_scroll_viewport.c" in RUN
assert "test_menu_rotation_scroll_source.py" in VERIFY

print("PASS: rotated scroll/item viewports and FA selected gray action")
