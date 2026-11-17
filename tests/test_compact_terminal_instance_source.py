#!/usr/bin/env python3
"""Check terminal pointer reuse cannot authorize stale partial character redraws."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
HEADER = (CPP / "angband/compact-player-viewport.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_compact_player_viewport.c").read_text(encoding="utf-8")


def function(src, name):
    start = src.index(name)
    return src[start:src.index("\n}\n", start) + 2]


cache = function(HEADER, "static inline int angdroid_compact_player_cache_matches(")
invalidate = function(HEADER, "static inline void angdroid_compact_player_invalidate(")
assert "uint64_t instance_id;" in HEADER
assert "uint64_t instance_id)" in cache
assert "return instance_id != 0 &&" in cache
assert "state->instance_id == instance_id;" in cache
assert "state->instance_id = 0;" in invalidate
assert "/* Even identical terminal pointer, size, screen version" in NATIVE
assert "/* Locale change */" in NATIVE
assert "state.instance_id = 43;" in NATIVE

for engine in ("angband", "faangband"):
    term_header = (CPP / engine / "ui-term.h").read_text(encoding="utf-8")
    term = (CPP / engine / "ui-term.c").read_text(encoding="utf-8")
    ui = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")

    assert "uint64_t instance_id;" in term_header
    assert "static uint64_t angdroid_term_instance_sequence;" in term
    init = function(term, "errr term_init(term *t, int w, int h, int k)")
    assert "memset(t, 0, sizeof(term));" in init
    assert "if (++angdroid_term_instance_sequence == 0)" in init
    assert "t->instance_id = angdroid_term_instance_sequence;" in init
    assert init.index("memset(t, 0, sizeof(term));") < init.index(
        "t->instance_id = angdroid_term_instance_sequence;")
    nuke = function(term, "errr term_nuke(term *t)")
    assert "t->instance_id = 0;" in nuke
    assert nuke.index("t->instance_id = 0;") < nuke.index("if (t->active_flag)")
    assert nuke.index("t->instance_id = 0;") < nuke.index("return (0);")

    panel = function(ui, "static void update_player_compact_subwindow(")
    assert "angdroid_compact_player_cache_matches(" in panel
    assert "Term->instance_id))" in panel
    assert "cache->instance_id = Term->instance_id;" in panel
    assert panel.index("cache->instance_id = Term->instance_id;") > (
        panel.index("COMPACT_PLAYER_ROW(prt_field("))
    assert "window_flag[win_idx] == PW_PLAYER_2" in panel

# Android currently persists language and warns that the native game
# requires restarting. Do not mutate GameActivity from a settings callback.
prefs = (JAVA / "PreferencesActivity.java").read_text(encoding="utf-8")
ctx = (JAVA / "AppLanguageContext.java").read_text(encoding="utf-8")
assert "app_language_restart_summary" in prefs
assert "Do not recreate GameActivity" in prefs
assert "recreate();" in prefs
assert "Native game calls this at startup" in ctx

assert "test_compact_terminal_instance_source.py" in RUNNER
print("PASS: native term identity protects recycled pointers and settings require restart")
