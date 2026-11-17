#!/usr/bin/env python3
"""Prevent stale per-terminal compact player caches across character lifetimes."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
HEADER = (CPP / "angband/compact-player-viewport.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_compact_player_viewport.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")


def function(src, signature):
    start = src.index(signature)
    return src[start:src.index("\n}\n", start) + 2]


assert "#include <stddef.h>" in HEADER
invalidate = function(HEADER, "static inline void angdroid_compact_player_invalidate(")
for field in ("state->terminal = NULL;", "state->width = 0;",
              "state->height = 0;", "state->ready = 0;"):
    assert field in invalidate, field
assert "if (!state) return;" in invalidate
assert "angdroid_compact_player_invalidate(&state);" in NATIVE
assert "angdroid_compact_player_invalidate(NULL);" in NATIVE
assert "CHECK(state.terminal == NULL);" in NATIVE
assert "CHECK(state.ready == 0);" in NATIVE

for engine in ("angband", "faangband"):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    invalidate_all = function(
        src, "static void angdroid_compact_player_invalidate_all(")
    assert "for (i = 0; i < ANGBAND_TERM_MAX; ++i)" in invalidate_all
    assert "angdroid_compact_player_invalidate(&compact_player_redraw_state[i]);" in invalidate_all

    for handler in ("ui_enter_world", "ui_leave_world", "ui_leave_game"):
        body = function(src, "static void " + handler + "(")
        assert "angdroid_compact_player_invalidate_all();" in body, (
            engine, handler)

    enter = function(src, "static void ui_enter_world(")
    assert enter.index("angdroid_compact_player_invalidate_all();") < (
        enter.index("redraw_stuff(player);"))
    leave = function(src, "static void ui_leave_world(")
    assert leave.index("angdroid_compact_player_invalidate_all();") < (
        leave.index("event_remove_handler_set(player_events"))
    exit_game = function(src, "static void ui_leave_game(")
    assert exit_game.index("angdroid_compact_player_invalidate_all();") < (
        exit_game.index("event_remove_handler(EVENT_MESSAGE"))

    flags = function(src, "static void subwindow_set_flags(")
    assert "angdroid_compact_player_invalidate(" in flags
    assert flags.index("angdroid_compact_player_invalidate(") < (
        flags.index("window_flag[win_idx] = new_flags;"))
    assert flags.index("angdroid_compact_player_invalidate(") < (
        flags.index("Term_clear();"))
    assert "Term_fresh();" in flags

    compact = function(src, "static void update_player_compact_subwindow(")
    assert "if (angband_term[i] == inv_term)" in compact
    assert "angdroid_compact_player_can_redraw_one(" in compact
    assert "window_flag[win_idx] == PW_PLAYER_2" in compact
    assert "cache->ready = (window_flag[win_idx] == PW_PLAYER_2);" in compact
    assert compact.index("cache->ready = ") > compact.index(
        "COMPACT_PLAYER_ROW(prt_field(")

    init = function(src, "void init_display(")
    assert "event_add_handler(EVENT_ENTER_WORLD, ui_enter_world, NULL);" in init
    assert "event_add_handler(EVENT_LEAVE_WORLD, ui_leave_world, NULL);" in init
    assert "event_add_handler(EVENT_LEAVE_GAME, ui_leave_game, NULL);" in init

assert "test_compact_player_lifecycle_source.py" in RUNNER
assert "test_compact_player_viewport.c" in (
    ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
print("PASS: both engines invalidate single-row caches on all world boundaries")
