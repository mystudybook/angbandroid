#!/usr/bin/env python3
"""Verify the native overlay depth regression compiles unmodified wrappers."""
import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GEN = ROOT / "tests/native/generate_screen_overlay_depth_host.py"
SCRIPT = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

spec = importlib.util.spec_from_file_location("screen_overlay_depth_test", GEN)
assert spec and spec.loader
generator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generator)

assert "generate_screen_overlay_depth_host.py" in SCRIPT
assert "test_screen_overlay_depth_source.py" in VERIFY

for engine in ("angband", "faangband"):
    original = (ROOT / "app/src/main/cpp" / engine / "ui-output.c").read_text(
        encoding="utf-8")
    generated = generator.generate(engine)
    for signature in generator.SIGNATURES:
        body = generator.module.extract_function(original, signature)
        assert body in generated, (engine, signature)

    reset = generator.module.extract_function(original,
        "void screen_reset_overlay_state(void)")
    ui_init = (ROOT / "app/src/main/cpp" / engine / "ui-init.c").read_text(
        encoding="utf-8")
    output_h = (ROOT / "app/src/main/cpp" / engine / "ui-output.h").read_text(
        encoding="utf-8")
    term_h = (ROOT / "app/src/main/cpp" / engine / "ui-term.h").read_text(
        encoding="utf-8")
    assert "screen_save_depth = 1;" in reset
    assert "screen_save_unbacked_depth = 0;" in reset
    assert "screen_save_excess_depth = 0;" in reset
    assert "void screen_reset_overlay_state(void);" in output_h
    assert "extern errr Term_discard_saved_screens(term *t);" in term_h
    init_entry = ui_init.index("void textui_init(void)")
    discard = ui_init.index("Term_discard_saved_screens(term_screen)", init_entry)
    reset_call = ui_init.index("screen_reset_overlay_state();", discard)
    pref = ui_init.index('process_pref_file("pref.prf"', init_entry)
    play_again = ui_init.index("if (!play_again)", init_entry)
    assert init_entry < discard < reset_call < pref < play_again
    assert "screen_save_depth++;" not in ui_init
    save = generator.module.extract_function(original, "void screen_save(void)")
    load = generator.module.extract_function(original, "void screen_load(void)")
    load_all = generator.module.extract_function(original,
        "void screen_load_all(void)")
    skip = generator.module.extract_function(original,
        "static bool screen_load_is_unbacked(void)")

    assert "static uint64_t screen_save_unbacked_depth;" in original
    assert "static uint64_t screen_save_excess_depth;" in original
    assert "if (screen_save_unbacked_depth || Term_save() != 0)" in save
    assert "if (screen_save_depth < INT16_MAX)" in save
    assert "++screen_save_excess_depth;" in save
    assert "if (screen_save_excess_depth)" in skip
    assert "if (screen_save_depth <= 0) return true;" in skip
    assert "if (screen_save_unbacked_depth)" in skip
    assert "if (screen_load_is_unbacked()) return;" in load
    assert "if (screen_load_is_unbacked()) return;" in load_all
    assert "if (Term_load() == 0) --screen_save_depth;" in load
    assert "if (Term_load_all() == 0) --screen_save_depth;" in load_all
    assert "Term_save();\n\tscreen_save_depth++;" not in original
    assert "Term_load();\n\tscreen_save_depth--;" not in original

    for name in (
        "test_empty_pops", "test_nested_and_replay", "test_native_capacity",
        "test_failed_native_save", "test_failed_native_restore",
        "test_public_depth_saturation", "test_new_character_session",
    ):
        assert "static void " + name + "(void)" in generated, (engine, name)
    assert "CHECK(screen_save_depth == INT16_MAX);" in generated
    assert "CHECK(screen_save_excess_depth == 5);" in generated
    assert "CHECK(screen_save_unbacked_depth == 7);" in generated
    assert 'CHECK(replayed == before + 3);' in generated
    assert "CHECK(screen_save_depth == 1);" in generated
    assert "CHECK(screen_save_excess_depth == 0 && screen_save_unbacked_depth == 0);" in generated

print("PASS: Angband/FA screen depths and new-game reset lifecycle contracts")
