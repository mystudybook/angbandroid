#!/usr/bin/env python3
"""FAangband legacy text-out must not paint beyond a rotated terminal.

This source check uses the actual production function and verifies the
offline C99 generator is registered. It does not alter game text strings.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "app/src/main/cpp/faangband/ui-output.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
HOST = (ROOT / "tests/native/generate_fa_text_out_screen_bounds_host.py").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-fa-text-out-screen-bounds.sh").read_text(
    encoding="utf-8")

a = SOURCE.index("void text_out_to_screen(uint8_t a, const char *str)")
b = SOURCE.index(" * Simple text display", a)
function = SOURCE[a:b]

must = (
    "if (!Term || !str) return;",
    "if (wid < 2 || h < 1) return;",
    "if (Term_locate(&x, &y) != 0) return;",
    "if (wrap > 256) wrap = 256;",
    "if (wrap < 2) return;",
    "x = MAX(0, MIN(text_out_indent, wrap - 1));",
    "if (++y >= h) return;",
    "x = MAX(0, MIN(x + text_out_pad, wrap - 1));",
    "if (n >= 0 && n < wrap)",
    "Term_erase(n, y, 255);",
    "Term_gotoxy(x, y);",
    "Term_addch(a, ch);",
    "span = angdroid_is_wide_text(ch, a) ? 2 : 1;",
    "angdroid_screen_text_needs_wrap(x, wrap, ch, a)",
    "if (Term_addch(a, ch) < 0) return;",
    "x += span;",
    "if (x > wrap) x = wrap;",
    "if (cv[i] == ANGDROID_TEXT_CONTINUATION ||",
    "if (span > wrap - 1)",
    "buf[1023] = L'\\0';",
)
for frag in must:
    assert frag in function, frag
assert function.count("if (++y >= h) return;") == 2
assert function.count("x = MAX(0, MIN(x + text_out_pad, wrap - 1));") == 2
assert function.index("if (wrap > 256) wrap = 256;") < function.index(
    "int av[256];")
assert function.index("if (n >= 0 && n < wrap)") < function.index(
    "Term_erase(n, y, 255);")
assert function.index("if (!Term || !str) return;") < function.index(
    "Term_get_size(&wid, &h);")
assert '#include "../curses/unicode_layout.h"' in SOURCE

# FAangband now uses display-cell coordinates in the stream API,
# including direct Term_addstr() calls. The text-out wrapper only
# performs line breaks and must not allocate another continuation.
terminal = (ROOT / "app/src/main/cpp/faangband/ui-term.c").read_text(
    encoding="utf-8")
bridge = (ROOT / "app/src/main/cpp/common/angdroid.c").read_text(
    encoding="utf-8")
layout = (ROOT / "app/src/main/cpp/curses/unicode_layout.h").read_text(
    encoding="utf-8")
assert "Term->scr->cx += span;" in terminal
assert "angdroid_next_visible_text_run(" in bridge
assert "if (cp[i] == ANGDROID_TEXT_CONTINUATION) continue;" in bridge
assert "ANGDROID_TEXT_CONTINUATION ((wchar_t)0xFDD0u)" in layout
assert "Term_queue_char(Term, x + 1, y, a," not in function
assert function.index("if (Term_addch(a, ch) < 0) return;") < function.index(
    "x += span;")
assert "text_out_to_file(" in (ROOT / "app/src/main/cpp/faangband/z-textblock.c").read_text(
    encoding="utf-8")

for frag in ("match.group(0)", "reset(400, 3)", "reset(1, 2)",
             "Term = NULL;", "Term_locate", "Term_what",
             "invalid == 0 && writes == 4",
             "continuation_writes == 2 && xcur == 7",
             "tiles[0][2] == ANGDROID_TEXT_CONTINUATION",
             "angdroid_utf8_next_scalar",
             "angdroid_terminal_columns"):
    assert frag in HOST, frag
assert '-std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT"' in RUNNER
assert "generate_fa_text_out_screen_bounds_host.py" in RUNNER
assert "test-fa-text-out-screen-bounds.sh" in VERIFY
assert "test_faangband_text_out_screen_bounds_source.py" in VERIFY
print("PASS: FA text-out screen boundaries and C99 source extraction")
