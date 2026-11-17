#!/usr/bin/env python3
"""Ensure host verification compiles genuine screen save/restore functions."""
import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
GENER = ROOT / "tests/native/generate_compact_term_screen_restore_host.py"
script = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
verify = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

spec = importlib.util.spec_from_file_location("compact_screen_restore", GENER)
assert spec and spec.loader
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)

assert "generate_compact_term_screen_restore_host.py" in script
assert "test_term_soft_cursor_bounds.c" in script
assert "test_term_resize_cjk.c" in script
assert 'for engine in angband faangband; do' in script
assert "test_compact_term_screen_restore_source.py" in verify

for engine in ("angband", "faangband"):
    native = (CPP / engine / "ui-term.c").read_text(encoding="utf-8")
    generated = module.generate(engine)
    for signature in module.SIGNATURES:
        body = module.lifecycle.extract_function(native, signature)
        assert body in generated, (engine, signature)

    fresh = module.lifecycle.extract_function(native, "errr Term_fresh(void)")
    assert '#include "../curses/term-soft-cursor-bounds.h"' in native
    assert fresh.count("angdroid_term_cursor_last_cell(") == 2
    assert fresh.count("angdroid_wide_partner_column(") == 1
    assert "if (Term->x1[ty] > left) Term->x1[ty] = left;" in fresh
    assert "if (Term->x2[ty] < right) Term->x2[ty] = right;" in fresh
    assert "angdroid_term_big_cursor_fits(scr->cx, scr->cy, w, h," in fresh
    assert "old->cy + old->cny - 1" not in fresh
    assert "old->cx + old->cnx - 1" not in fresh
    save = module.lifecycle.extract_function(native, "errr Term_save(void)")
    nuke = module.lifecycle.extract_function(native, "errr term_nuke(term *t)")
    resize = module.lifecycle.extract_function(native, "errr Term_resize(int w, int h)")
    assert '#include "../curses/term-resize-cjk.h"' in native
    assert resize.count("term_win_copy_resized(") == 4
    resized_copy = module.lifecycle.extract_function(native,
        "static void term_win_copy_resized(term_win *dest, term_win *src,\\n        int copy_w, int copy_h, int old_w)")
    assert "term_win_copy(dest, src, copy_w, copy_h);" in resized_copy
    assert resized_copy.count("angdroid_term_repair_resized_cjk_row(") == 2
    clamp = module.lifecycle.extract_function(
        native, "static void term_win_clamp_cursor(term_win *win, int w, int h)")
    assert "win->cu = true;" in clamp
    assert "win->cx = w - 1;" in clamp
    assert "win->cy = h - 1;" in clamp
    assert resize.count("term_win_clamp_cursor(") == 5

    assert "if (Term->saved == UINT8_MAX) return (-1);" in save
    assert save.index("if (Term->saved == UINT8_MAX)") < save.index("mem_zalloc(")
    assert "while (t->mem)" in nuke
    assert "term_win_nuke(saved);" in nuke
    assert "mem_free(saved);" in nuke
    assert "hold_mem = hold_mem->next;" in resize
    assert "term_win_copy(*hold_mem_dest, hold_mem, wid, hgt);" in resize
    load = module.lifecycle.extract_function(native, "errr Term_load(void)")
    load_all = module.lifecycle.extract_function(native, "errr Term_load_all(void)")
    assert "if (!Term->mem) return (-1);" in load
    assert "if (!Term->mem) return (-1);" in load_all
    assert load.index("if (!Term->mem) return (-1);") < load.index("Term->saved--;")
    assert load_all.index("if (!Term->mem) return (-1);") < load_all.index(
        "Term->saved--;")
    assert "static void verify_no_saved_load(term *t)" in generated
    assert "static void nested_snapshot(term *t)" in generated
    assert "static void resize_nested_snapshot(term *t)" in generated
    assert "static void snapshot_depth_limit(void)" in generated
    assert "static void cursor_shrink_guard(term *t)" in generated
    assert "static void resized_cjk_snapshot_edges(void)" in generated
    assert "expect_clipped_edge(t.mem->next, 3, t.hgt);" in generated
    assert "CHECK(t->scr->cu && t->scr->cx == w - 1 && t->scr->cy == h - 1);" in generated
    assert "CHECK(t->tmp->cu && t->tmp->cx == 0 && t->tmp->cy == 0);" in generated
    assert "CHECK(Term_resize(old_w + 2, old_h + 1) == 0);" in generated
    assert "CHECK(Term->saved == UINT8_MAX && Term->mem == top);" in generated
    assert "CHECK(Term->saved == 2);" in generated
    assert "CHECK(Term->saved == 1);" in generated
    assert "CHECK(Term->saved == 0);" in generated
    assert "run_screen(1, 1);" in generated
    assert "run_screen(80, 24);" in generated
    assert "L'甲'" in generated
    assert "L'乙'" in generated
    assert "L'丙'" in generated
    assert "CHECK(allocations - prev_alloc == releases - prev_release);" in generated

print("PASS: native cursor redraw and resized CJK cells across all windows")
