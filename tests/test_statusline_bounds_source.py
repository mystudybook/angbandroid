#!/usr/bin/env python3
"""Source contracts for bounded statusline on narrow Angband/FA terminals.

The host C99 test checks numerical bounds; this verifies actual UI wiring.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
H = (CPP / "angband/statusline-bounds.h").read_text(encoding="utf-8")
SCRIPT = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


def source_function(text: str, signature: str) -> str:
    pos = text.index(signature)
    return text[pos:text.index("\n}\n", pos) + 2]


assert "angdroid_statusline_has_room(" in H
assert "angdroid_statusline_next_col(" in H
assert "term_width > 0 && col >= 0 && col < term_width" in H
assert "used < (size_t)(term_width - col)" in H
assert "advance >= (size_t)(term_width - col)" in H
assert "return col + (int)advance;" in H
for engine, include, status_put in (
    ("angband", '#include "statusline-bounds.h"', "ang_status_put"),
    ("faangband", '#include "../angband/statusline-bounds.h"', "fa_status_put"),
):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    assert include in src, engine
    dispatcher = source_function(src, "static void update_statusline_aux(")
    assert "if (!Term || row < 0 || row >= Term->hgt ||" in dispatcher, engine
    assert "angdroid_statusline_has_room(col, 0, Term->wid)" in dispatcher
    assert "prt(\"\", row, col);" in dispatcher
    assert dispatcher.count("angdroid_statusline_has_room(") == 2, engine
    assert "if (!angdroid_statusline_has_room(col, 0, Term->wid)) break;" in dispatcher
    assert "col = angdroid_statusline_next_col(col," in dispatcher
    assert "status_handlers[i](row, col), Term->wid);" in dispatcher
    assert "col += status_handlers[i]" not in dispatcher

    timed = source_function(src, "static size_t prt_tmd(")
    assert "for (i = 0; i < TMD_MAX; i++)" in timed
    assert "if (!Term || !angdroid_statusline_has_room(col, len," in timed
    assert "Term->wid)) break;" in timed
    assert "if (player->timed[i])" in timed
    assert "while (player->timed[i] > grade->max)" in timed
    assert "if (!grade->name) continue;" in timed
    assert "angdroid_timed_status_name(grade->name," in timed
    assert "if (i == TMD_FOOD)" in timed
    assert "meter = format(\"%d %%\", player->timed[i] / 100)" in timed
    assert timed.count("angdroid_statusline_has_room(col, len,") == 2, engine
    assert timed.count(status_put + "(grade->color,") == 2, engine

assert "test_statusline_bounds.c" in NATIVE
assert "test_statusline_bounds_source.py" in SCRIPT
print("PASS: bounded statusline dispatch and multi-effect rendering in both engines")
