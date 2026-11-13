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
)
for frag in must:
    assert frag in function, frag
assert function.count("if (++y >= h) return;") == 2
assert function.count("x = MAX(0, MIN(x + text_out_pad, wrap - 1));") == 2
assert function.index("if (wrap > 256) wrap = 256;") < function.index(
    "int av[256];")
assert function.index("if (n >= 0 && n < wid)") < function.index(
    "Term_erase(n, y, 255);")
assert function.index("if (!Term || !str) return;") < function.index(
    "Term_get_size(&wid, &h);")
assert "text_out_to_file(" in (ROOT / "app/src/main/cpp/faangband/z-textblock.c").read_text(
    encoding="utf-8")

for frag in ("match.group(0)", "reset(400, 3)", "reset(1, 2)",
             "Term = NULL;", "Term_locate", "Term_what",
             "invalid == 0 && writes == 4"):
    assert frag in HOST, frag
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "generate_fa_text_out_screen_bounds_host.py" in RUNNER
assert "test-fa-text-out-screen-bounds.sh" in VERIFY
assert "test_faangband_text_out_screen_bounds_source.py" in VERIFY
print("PASS: FA text-out screen boundaries and C99 source extraction")
