#!/usr/bin/env python3
"""Character subwindow redraw clears only occupied rows for tall windows."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
HEADER = (CPP / "angband/compact-player-viewport.h").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")

assert "ANGDROID_COMPACT_PLAYER_FIXED_ROWS 12" in HEADER
assert "if (term_height <= 0 || stat_count < 0) return 0;" in HEADER
assert "stat_count >= term_height - ANGDROID_COMPACT_PLAYER_FIXED_ROWS" in HEADER
assert "return stat_count + ANGDROID_COMPACT_PLAYER_FIXED_ROWS;" in HEADER

for engine, include in (
    ("angband", '#include "compact-player-viewport.h"'),
    ("faangband", '#include "../angband/compact-player-viewport.h"'),
):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    begin = src.index("static void update_player_compact_subwindow(")
    body = src[begin:src.index("\n}\n", begin) + 2]
    assert include in src, engine
    assert "visible_rows = angdroid_compact_player_visible_rows(" in body
    assert "STAT_MAX, Term->hgt);" in body
    assert "if (visible_rows == Term->hgt)" in body
    assert "Term_clear();" in body
    assert "for (i = 0; i < visible_rows; ++i)" in body
    assert "Term_erase(0, i, Term->wid);" in body
    assert body.index("Term_activate(inv_term);") < body.index(
        "visible_rows = angdroid_compact_player_visible_rows(") < body.index(
        "COMPACT_PLAYER_ROW(prt_field(")
    assert "Term_fresh();" in body
    assert body.index("Term_fresh();") < body.rindex("Term_activate(old);")
    assert "if (row < Term->hgt && Term->wid >= 12) { call; }" in body

    flags_start = src.index("static void subwindow_set_flags(")
    flags = src[flags_start:src.index("\n}\n", flags_start) + 2]
    assert "window_flag[win_idx] = new_flags;" in flags
    assert "Term_clear();" in flags
    assert "Term_fresh();" in flags
    assert flags.index("Term_clear();") < flags.index("Term_fresh();")

assert "test_compact_player_viewport.c" in NATIVE
assert "test_compact_player_viewport_source.py" in RUNNER
print("PASS: compact player clears only owned rows and flags still clear all")
