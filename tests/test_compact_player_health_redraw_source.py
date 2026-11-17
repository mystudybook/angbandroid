#!/usr/bin/env python3
"""Protect monster health bars and compact player windows on small terminals.

Both game engines must keep original field order and the same UI gameplay
values while avoiding offscreen text and stale rows after player changes.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
CPP = ROOT / "app/src/main/cpp"


def function(source, signature):
    start = source.index(signature)
    end = source.index("\n}\n", start) + 2
    return source[start:end]


for engine in ("angband", "faangband"):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")

    health = function(src, "static int prt_health_aux(")
    assert "if (!Term || row < 0 || row >= Term->hgt ||" in health
    assert "col < 0 || col >= Term->wid) return 0;" in health
    assert "angdroid_statusline_fits_field(col, 12, Term->wid)" in health
    assert "Term_erase(col, row, Term->wid - col);" in health
    assert health.index("angdroid_statusline_fits_field(") < health.index(
        "attr = monster_health_attr();")
    assert "Term_erase(col, row, 12);" in health
    assert "Term_putstr(col, row, 12, attr, \"[----------]\");" in health
    assert "Term_putstr(col + 1, row, len, attr, \"**********\");" in health
    assert "(mon->maxhp <= 0)" in health
    assert health.index("(mon->maxhp <= 0)") < health.index(
        "100L * mon->hp / mon->maxhp;")

    short = function(src, "static int prt_health_short(")
    assert "Term->wid - col < 12" in short
    assert "len = prt_health_aux(row, col);" in short
    assert short.index("Term->wid - col < 12") < short.index(
        "len = prt_health_aux(row, col);")

    compact = function(src, "static void update_player_compact_subwindow(")
    assert "if (!inv_term || !player || !player->race || !player->class) return;" in compact
    assert "Term->hgt <= 0 || Term->wid <= 0" in compact
    assert "#define COMPACT_PLAYER_ROW(call) do {" in compact
    assert "#define COMPACT_PLAYER_VALUE_ROW(call) do {" in compact
    assert "if (row < Term->hgt) { call; }" in compact
    assert "if (row < Term->hgt && Term->wid >= 12) { call; }" in compact
    assert "#undef COMPACT_PLAYER_ROW" in compact
    assert "#undef COMPACT_PLAYER_VALUE_ROW" in compact

    activation = compact.index("Term_activate(inv_term);")
    cleared = compact.index("Term_clear();")
    first_row = compact.index("COMPACT_PLAYER_ROW(prt_field(")
    flashed = compact.index("Term_fresh();", first_row)
    restored = compact.index("Term_activate(old);", flashed)
    assert activation < cleared < first_row < flashed < restored

    expected = (
        "COMPACT_PLAYER_ROW(prt_field(",  # race and class
        "COMPACT_PLAYER_ROW(prt_title(row, col));",
        "COMPACT_PLAYER_VALUE_ROW(prt_level(row, col));",
        "COMPACT_PLAYER_VALUE_ROW(prt_exp(row, col));",
        "COMPACT_PLAYER_VALUE_ROW(prt_gold(row, col));",
        "COMPACT_PLAYER_ROW(prt_equippy(row, col));",
        "COMPACT_PLAYER_VALUE_ROW(prt_stat(i, row, col));",
        "++row; /* Original blank separator row. */",
        "COMPACT_PLAYER_VALUE_ROW(prt_ac(row, col));",
        "COMPACT_PLAYER_VALUE_ROW(prt_hp(row, col));",
        "COMPACT_PLAYER_VALUE_ROW(prt_sp(row, col));",
        "if (row < Term->hgt) prt_health(row, col);",
    )
    assert compact.count("COMPACT_PLAYER_ROW(prt_field(") == 2
    positions = [compact.index(s) for s in expected]
    assert positions == sorted(positions), engine
    assert "for (i = 0; i < STAT_MAX; i++)" in compact
    assert compact.count("COMPACT_PLAYER_VALUE_ROW(") == 8

    equipment = function(src, "static void prt_equippy(")
    assert "col < 0 || col >= Term->wid) return;" in equipment
    assert "i < player->body.count && i < Term->wid - col" in equipment
    assert "Term_putch(col + i, row, a, c);" in equipment

assert "test_compact_player_health_redraw_source.py" in RUNNER
print("PASS: compact player refresh clears stale values and bounds both health renderers")
