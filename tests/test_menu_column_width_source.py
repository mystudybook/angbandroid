#!/usr/bin/env python3
"""Check both Angband multi-column menu paths use the nonzero width helper.

The pure C arithmetic is also tested by test_menu_column_width.c.
This static check is not an Android UI acceptance test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(encoding="utf-8")
start = source.index("static int columns_get_cursor(")
end = source.index("static char column_get_tag(", start)
section = source[start:end]

expression = "angdroid_menu_fit_column_width(23, loc->width, 0, cols)"
if section.count(expression) != 2:
    raise SystemExit("FAIL: menu hit-testing and drawing must use the same active width")
if "colw = (w - col) / cols;" in section:
    raise SystemExit("FAIL: unsafe native column width division remains")
if section.count("if (rows_per_page <= 0 || n <= 0)") != 2:
    raise SystemExit("FAIL: zero-height menu layout is not guarded")
if "return angdroid_menu_columns_hit(" not in section:
    raise SystemExit("FAIL: multi-column touch mapping bypasses checked hit-test")
if "((col - loc->col) / colw)" in section:
    raise SystemExit("FAIL: unsafe native cursor mapping remains")
print("PASS: native menu column width and touch mapping are guarded")
