#!/usr/bin/env python3
"""Invalidate compact display-row caches after logical screen replacement.

Term_redraw() and Term_redraw_all() repaint the current screen buffer, so
neither must bump the generation or force a character data re-render.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_compact_player_viewport.c").read_text(
    encoding="utf-8")
H = (CPP / "angband/compact-player-viewport.h").read_text(encoding="utf-8")


def function(source, signature):
    start = source.index(signature)
    return source[start:source.index("\n}\n", start) + 2]


matcher = function(H, "static inline int angdroid_compact_player_cache_matches(")
assert "state->content_generation == content_generation" in matcher
assert "state->locale_zh_tw == locale_zh_tw" in matcher
assert "angdroid_compact_player_can_redraw_one(state, terminal," in matcher

invalidate = function(H, "static inline void angdroid_compact_player_invalidate(")
assert "state->content_generation = 0;" in invalidate
assert "state->locale_zh_tw = 0;" in invalidate
assert "angdroid_compact_player_cache_matches(" in NATIVE
assert "/* Clear or restore */" in NATIVE
assert "/* Locale change */" in NATIVE

for engine in ("angband", "faangband"):
    term_header = (CPP / engine / "ui-term.h").read_text(encoding="utf-8")
    term_source = (CPP / engine / "ui-term.c").read_text(encoding="utf-8")
    ui_source = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")

    assert "uint32_t content_generation;" in term_header, engine
    for signature in (
        "errr Term_clear(void)",
        "errr Term_load(void)",
        "errr Term_load_all(void)",
        "errr Term_resize(int w, int h)",
    ):
        body = function(term_source, signature)
        assert "++Term->content_generation;" in body, (engine, signature)

    clear = function(term_source, "errr Term_clear(void)")
    assert clear.index("++Term->content_generation;") < clear.index(
        "Term->scr->cu = 0;"), engine
    load = function(term_source, "errr Term_load(void)")
    assert load.index("++Term->content_generation;") < load.index(
        "term_win_copy(Term->scr, tmp, w, h);"), engine
    restore = function(term_source, "errr Term_load_all(void)")
    assert restore.index("++Term->content_generation;") < restore.index(
        "term_win_copy(Term->scr, tgt->saved_mem, w, h);"), engine
    resize = function(term_source, "errr Term_resize(int w, int h)")
    assert resize.index("if ((Term->wid == w) && (Term->hgt == h)) return (1);") < (
        resize.index("++Term->content_generation;"))
    assert resize.index("++Term->content_generation;") < (
        resize.index("Term->wid = w;"))

    repaint = function(term_source, "errr Term_redraw(void)")
    assert "++Term->content_generation" not in repaint, engine
    repaint_all = function(term_source, "extern errr Term_redraw_all(void)")
    assert "++Term->content_generation" not in repaint_all
    assert "Term_redraw();" in repaint_all

    panel = function(ui_source, "static void update_player_compact_subwindow(")
    assert "angdroid_compact_player_cache_matches(" in panel
    assert "Term->content_generation, angdroid_zh_tw_enabled()))" in panel
    assert "cache->content_generation = Term->content_generation;" in panel
    assert "cache->locale_zh_tw = angdroid_zh_tw_enabled();" in panel
    assert panel.index("cache->content_generation = Term->content_generation;") > (
        panel.index("COMPACT_PLAYER_ROW(prt_field("))
    assert panel.index("cache->content_generation = Term->content_generation;") > (
        panel.index("Term_clear();"))

assert "test_compact_player_screen_generation_source.py" in RUNNER
print("PASS: logical screen versions and locale changes invalidate compact rows")
