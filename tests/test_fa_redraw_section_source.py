#!/usr/bin/env python3
"""FAangband partial redraw must not split CJK or lose pending work.

The companion C99 generator compiles the actual production function,
not a parallel rewrite, against mock native terminal arrays.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PATH = ROOT / "app/src/main/cpp/faangband/ui-term.c"
SOURCE = PATH.read_text(encoding="utf-8")
HOST = (ROOT / "tests/native/generate_fa_redraw_section_host.py").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-fa-redraw-section.sh").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")

a = SOURCE.index("errr Term_redraw_section(int x1, int y1, int x2, int y2)")
b = SOURCE.index(" * Access routines", a)
redraw = SOURCE[a:b]

for text in (
    "if (!Term || !Term->scr || !Term->old ||",
    "x1 > x2 || y1 > y2 ||",
    "x1 >= Term->wid || x2 < 0 ||",
    "y1 >= Term->hgt || y2 < 0)",
    "if (x2 >= Term->wid) x2 = Term->wid - 1;",
    "if (y2 >= Term->hgt) y2 = Term->hgt - 1;",
    "angdroid_union_dirty_range(y1, y2, &Term->y1, &Term->y2);",
    "int first = x1, last = x2;",
    "angdroid_expand_redraw_columns(Term->wid, &first, &last,",
    "Term->scr->c[i], Term->scr->a[i],",
    "Term->old->c[i], Term->old->a[i]);",
    "angdroid_union_dirty_range(first, last,",
    "&Term->x1[i], &Term->x2[i]);",
    "previous[j] = 0;",
    "Term_fresh();",
):
    assert text in redraw, text
assert "Term->x1[i] = x1;" not in redraw
assert "Term->y1 = y1;" not in redraw
assert "if ((x1 > 0) && (Term->old->a[i][x1] == 255))" not in redraw
assert redraw.index("angdroid_expand_redraw_columns") < (
    redraw.index("angdroid_union_dirty_range(first, last"))
assert redraw.index("previous[j] = 0;") < redraw.index("Term_fresh();")
assert "angdroid_expand_redraw_columns(" in (
    ROOT / "app/src/main/cpp/curses/unicode_layout.h").read_text(
        encoding="utf-8")

for required in (
    "match.group(0)",
    "before_chars[2][7] = L'乙';",
    "now_chars[1][5] = L'甲';",
    "now_chars[0][2] = ANGDROID_TEXT_CONTINUATION;",
    "device.x1[1] == 2 && device.x2[1] == 3",
    "device.x1[1] == 0 && device.x2[1] == 6",
    "before_attrs[0][4] = 255",
    "Term = NULL;",
):
    assert required in HOST, required
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUN
assert "generate_fa_redraw_section_host.py" in RUN
assert "test-fa-redraw-section.sh" in VERIFY
assert "test_fa_redraw_section_source.py" in VERIFY

print("PASS: FA partial redraw CJK columns, bounds and pending requests")
