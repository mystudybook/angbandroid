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
if section.count("if (rows_per_page <= 0 || n <= 0)") != 1:
    raise SystemExit("FAIL: zero-height/empty column hit-test is not guarded")
if "if (rows_per_page <= 0 || loc->width <= 0)" not in section:
    raise SystemExit("FAIL: rotated column renderer can use an invalid viewport")
if "for (r = 0; r < rows_per_page; ++r)" not in section or (
    "Term_erase(col, row + r, loc->width);" not in section
):
    raise SystemExit("FAIL: old columns remain after a filter is cleared")
if "if (n <= 0)" not in section:
    raise SystemExit("FAIL: renderer does not clear an empty filtered view")
if "c < loc->width / colw" not in section:
    raise SystemExit("FAIL: renderer draws columns beyond terminal width")
if "r < n - base" not in section:
    raise SystemExit("FAIL: final column can address an invalid row")
if "dx >= 0 && dx < loc->width" not in section:
    raise SystemExit("FAIL: cursor may be sent off-screen after rotation")
if "return angdroid_menu_columns_hit(" not in section:
    raise SystemExit("FAIL: multi-column touch mapping bypasses checked hit-test")
if "((col - loc->col) / colw)" in section:
    raise SystemExit("FAIL: unsafe native cursor mapping remains")
print("PASS: native menu column width and touch mapping are guarded")
