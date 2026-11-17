#!/usr/bin/env python3
"""Ensure host C99 lifecycle tests compile real engine functions, not copies."""
import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
GENERATOR = ROOT / "tests/native/generate_compact_term_lifecycle_host.py"

spec = importlib.util.spec_from_file_location("compact_term_host", GENERATOR)
assert spec and spec.loader
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)

runner = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
verifier = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
assert "generate_compact_term_lifecycle_host.py" in runner
assert "for engine in angband faangband; do" in runner
assert "test_compact_real_term_lifecycle_source.py" in verifier

for engine in ("angband", "faangband"):
    source = (CPP / engine / "ui-term.c").read_text(encoding="utf-8")
    build = (CPP / engine / "CMakeLists.txt").read_text(encoding="utf-8")
    assert "ui-term.c" in build
    init = module.extract_function(
        source, "errr term_init(term *t, int w, int h, int k)")
    nuke = module.extract_function(source, "errr term_nuke(term *t)")
    generated = module.generate(engine)

    assert init in generated
    assert nuke in generated
    assert "static uint64_t angdroid_term_instance_sequence;" in generated
    assert init.index("memset(t, 0, sizeof(term));") < init.index(
        "t->instance_id = angdroid_term_instance_sequence;")
    assert nuke.index("t->instance_id = 0;") < nuke.index(
        "if (t->active_flag)")
    assert "assert(t->instance_id == 0);" in generated
    assert "test_reuse_same_address();" in generated
    assert "test_many_term_cycles();" in generated
    assert "CHECK(alloc_count == free_count);" in generated

    header = (CPP / engine / "ui-term.h").read_text(encoding="utf-8")
    assert "uint64_t instance_id;" in header
    assert "uint32_t content_generation;" in header

print("PASS: real Angband/FAangband term init/destroy extracted and compiled by host runner")
