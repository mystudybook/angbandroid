#!/usr/bin/env python3
"""Source contracts for compact two-row topbar safety in both engines.

Every handler must be guarded before its starting column is passed to the
terminal; FA localized locality names must keep their numeric depth suffix.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
expected_top = (
    "prt_level_short(row, col)",
    "prt_exp_short(row, col)",
    "prt_stat_short(STAT_STR, row, col)",
    "prt_stat_short(STAT_INT, row, col)",
    "prt_stat_short(STAT_WIS, row, col)",
    "prt_stat_short(STAT_DEX, row, col)",
    "prt_stat_short(STAT_CON, row, col)",
    "prt_ac_short(row, col)",
    "prt_gold_short(row, col)",
    "prt_race_class_short(row, col)",
)
expected_bottom = (
    "prt_hp_short(row, col)",
    "prt_sp_short(row, col)",
    "prt_health_short(row, col)",
    "prt_speed_short(row, col)",
    "prt_depth_short(row, col)",
    "prt_title_short(row, col)",
)

def body(src: str, signature: str) -> str:
    begin = src.index(signature)
    return src[begin:src.index("\n}\n", begin) + 2]

for engine in ("angband", "faangband"):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    topbar = body(src, "static void update_topbar(")
    assert 'if (!Term || row < 0 || row >= Term->hgt || Term->wid <= 0) return;' in topbar, engine
    assert '#define APPEND_TOPBAR(call) do { \\' in topbar, engine
    assert "angdroid_statusline_has_room(col, 0, Term->wid)" in topbar
    assert "angdroid_statusline_next_col(col, (size_t)(call)," in topbar
    assert "#undef APPEND_TOPBAR" in topbar
    assert topbar.count('prt("", row, col);') == 2
    assert "++row;\n\tif (row >= Term->hgt) return;\n\tcol = 0;" in topbar
    assert not re.search(r"col\s*\+=\s*prt_", topbar)
    calls = re.findall(r"APPEND_TOPBAR\((prt_[^;\n]+)\);", topbar)
    assert calls == list(expected_top + expected_bottom), (engine, calls)

fa = (CPP / "faangband/ui-display.c").read_text(encoding="utf-8")
for signature, buffer_marker in (
    ("static size_t prt_depth(", "char depths[160];"),
    ("static int prt_depth_short(", "char buf[160];"),
):
    section = body(fa, signature)
    assert buffer_marker in section, signature
    assert "fmt_depth(" in section, signature

fa_formatter = body(fa, "static int fmt_depth(")
assert "char display[160];" in fa_formatter
assert 'strnfmt(display, sizeof(display), "%s %d", locality,' in fa_formatter
assert '"%s 城鎮"' in fa_formatter
assert "angdroid_utf8_copy_complete(buf, (size_t)max, display)" in fa_formatter
assert "my_strcpy(buf, level_name(lev), max);" in fa_formatter

native = (ROOT / "tests/native/test_shape_locality_display_locale.c").read_text(encoding="utf-8")
assert '#include "../../app/src/main/cpp/faangband/list-localities.h"' in native
assert "CHECK(count == 38)" in native
assert 'strcmp(copied + strlen(copied) - 4, " 999")' in native

assert "test_compact_topbar_bounds_source.py" in RUNNER
print("PASS: 16 bounded compact topbar labels, two rows and full FA depth suffixes")
