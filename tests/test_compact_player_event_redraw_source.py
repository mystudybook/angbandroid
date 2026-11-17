#!/usr/bin/env python3
"""Conservative event-specific compact player updates across both engines.

No EVENT_END buffering: redraw_stuff() deliberately skips EVENT_END with the
main map hidden, so delayed-only panel updates could leave stale information.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
HEADER = (CPP / "angband/compact-player-viewport.h").read_text(encoding="utf-8")
SCRIPT = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
C99 = (ROOT / "tests/native/test_compact_player_viewport.c").read_text(
    encoding="utf-8")

def source_function(src, name):
    start = src.index(name)
    return src[start:src.index("\n}\n", start) + 2]

assert "struct angdroid_compact_player_redraw_state" in HEADER
assert "state->terminal == terminal" in HEADER
assert "state->width == term_width && state->height == term_height" in HEADER
assert "state && state->ready && sole_compact_flag" in HEADER
assert "angdroid_compact_row_hp(" in HEADER
assert "angdroid_compact_row_sp(" in HEADER
assert "angdroid_compact_row_monster(" in HEADER
assert "ANGDROID_COMPACT_ROW_EXP" in HEADER
assert "angdroid_compact_player_can_redraw_one(" in C99
assert "CHECK(angdroid_compact_row_hp(5) == 14)" in C99
assert "CHECK(angdroid_compact_row_sp(5) == 15)" in C99
assert "CHECK(angdroid_compact_row_monster(5) == 16)" in C99

mapping = {
    "EVENT_PLAYERTITLE": "ANGDROID_COMPACT_ROW_TITLE",
    "EVENT_EXPERIENCE": "ANGDROID_COMPACT_ROW_EXP",
    "EVENT_GOLD": "ANGDROID_COMPACT_ROW_GOLD",
    "EVENT_HP": "angdroid_compact_row_hp(STAT_MAX)",
    "EVENT_MANA": "angdroid_compact_row_sp(STAT_MAX)",
    "EVENT_MONSTERHEALTH": "angdroid_compact_row_monster(STAT_MAX)",
}
for engine in ("angband", "faangband"):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    redraw = source_function(src, "static void update_player_compact_subwindow(")
    flags = source_function(src, "static void subwindow_set_flags(")
    register = source_function(src, "static void subwindow_flag_changed(")
    player_calcs = (CPP / engine / "player-calcs.c").read_text(encoding="utf-8")
    broadcast = source_function(player_calcs, "void redraw_stuff(")

    assert "compact_player_redraw_state[ANGBAND_TERM_MAX]" in src
    assert "angdroid_compact_player_invalidate(" in flags
    assert "&compact_player_redraw_state[win_idx]" in flags
    assert "if (angband_term[i] == inv_term)" in redraw
    assert "window_flag[win_idx] == PW_PLAYER_2" in redraw
    assert "angdroid_compact_player_cache_matches(" in redraw
    assert "cache->terminal = inv_term;" in redraw
    assert "cache->width = Term->wid;" in redraw
    assert "cache->height = Term->hgt;" in redraw
    assert "cache->content_generation = Term->content_generation;" in redraw
    assert "cache->locale_zh_tw = angdroid_zh_tw_enabled();" in redraw
    assert "Term->content_generation, angdroid_zh_tw_enabled()))" in redraw
    assert "cache->ready = (window_flag[win_idx] == PW_PLAYER_2);" in redraw
    assert redraw.index("if (win_idx >= 0 &&") < redraw.index(
        "visible_rows = angdroid_compact_player_visible_rows(")
    assert redraw.index("Term_erase(0, partial_row, Term->wid);") < (
        redraw.index("Term_fresh();"))
    assert redraw.index("Term_fresh();") < redraw.index(
        "Term_activate(old);", redraw.index("Term_fresh();"))
    assert "if (partial_row < Term->hgt)" in redraw
    assert "if (Term->wid >= 12) prt_hp(partial_row, col);" in redraw
    assert "if (Term->wid >= 12) prt_sp(partial_row, col);" in redraw
    assert "prt_health(partial_row, col);" in redraw
    assert "prt_title(partial_row, col);" in redraw
    assert "prt_exp(partial_row, col);" in redraw
    assert "prt_gold(partial_row, col);" in redraw
    assert "Term_clear();" in redraw
    assert "Term_erase(0, i, Term->wid);" in redraw
    assert "COMPACT_PLAYER_ROW(prt_field(" in redraw
    assert "set_register_or_deregister(player_events" in register
    assert "update_player_compact_subwindow," in register

    for signal, row in mapping.items():
        assert "case " + signal + ":" in redraw
        assert "partial_row = " + row + ";" in redraw
    for unchanged in ("EVENT_PLAYERSPEED", "EVENT_DUNGEONLEVEL"):
        assert "case " + unchanged + ":" in redraw
    assert "Neither status is present in this character subwindow." in redraw
    # Unsupported multi-field events remain full redraw, not stale single row.
    assert "Equipment, level, stats or race changes may affect" in redraw
    assert "case EVENT_PLAYERLEVEL:" not in redraw
    assert "case EVENT_RACE_CLASS:" not in redraw
    assert "case EVENT_STATS:" not in redraw

    # The two engines both omit EVENT_END for hidden-map redraws. Any
    # change that buffers until EVENT_END would break those updates.
    assert broadcast.index("if (!map_is_visible()) return;") < (
        broadcast.index("event_signal(EVENT_END);"))
    assert "event_signal(hnd->event);" in broadcast

assert "test_compact_player_event_redraw_source.py" in SCRIPT
print("PASS: immediate per-event rows with initial/full/mixed-flag fallbacks")
