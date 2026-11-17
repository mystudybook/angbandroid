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
assert 'for engine in angband faangband; do' in script
assert "test_compact_term_screen_restore_source.py" in verify

for engine in ("angband", "faangband"):
    native = (CPP / engine / "ui-term.c").read_text(encoding="utf-8")
    generated = module.generate(engine)
    for signature in module.SIGNATURES:
        body = module.lifecycle.extract_function(native, signature)
        assert body in generated, (engine, signature)

    load = module.lifecycle.extract_function(native, "errr Term_load(void)")
    load_all = module.lifecycle.extract_function(native, "errr Term_load_all(void)")
    assert "if (!Term->mem) return (-1);" in load
    assert "if (!Term->mem) return (-1);" in load_all
    assert load.index("if (!Term->mem) return (-1);") < load.index("Term->saved--;")
    assert load_all.index("if (!Term->mem) return (-1);") < load_all.index(
        "Term->saved--;")
    assert "static void verify_no_saved_load(term *t)" in generated
    assert "static void nested_snapshot(term *t)" in generated
    assert "CHECK(Term->saved == 2);" in generated
    assert "CHECK(Term->saved == 1);" in generated
    assert "CHECK(Term->saved == 0);" in generated
    assert "run_screen(1, 1);" in generated
    assert "run_screen(80, 24);" in generated
    assert "L'甲'" in generated
    assert "L'乙'" in generated
    assert "L'丙'" in generated
    assert "CHECK(allocations - prev_alloc == releases - prev_release);" in generated

print("PASS: actual native clear/save/restore functions and empty-stack guard")
