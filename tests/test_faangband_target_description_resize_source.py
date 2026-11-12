#!/usr/bin/env python3
"""Guard FAangband description repaint during Android rotation.

Native behavior comes from production-function C99 extraction in
scripts/test-fa-target-description-resize.sh. This test also preserves
the input event contract and checks all six description call sites.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
TARGET = (FA / "ui-target.c").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def section(start, end):
    assert start in TARGET, start
    assert end in TARGET, end
    return TARGET.split(start, 1)[1].split(end, 1)[0]


helper = section(
    "static ui_event fa_target_inkey_described(",
    "/* A short landscape terminal")
for snippet in (
    "ui_event press = EVENT_EMPTY;",
    "press = inkey_ex();",
    "if (press.type == EVT_RESIZE)",
    "fa_target_draw_bounded_line(description, 0);",
    "move_cursor_relative(y, x);",
    "if (press.type == EVT_NONE) continue;",
    "if (!Term || Term->wid < 3 || Term->hgt < 1)",
    'prt("", 0, 0);',
    "press.key.code = ESCAPE;",
    "press.key.mods = 0;",
    "if (press.type == EVT_BUTTON)",
    "press.type = EVT_KBRD;",
    "press.type == EVT_KBRD || press.type == EVT_MOUSE",
):
    assert snippet in helper, snippet
assert helper.index("fa_target_draw_bounded_line(description, 0);") < (
    helper.index("move_cursor_relative(y, x);"))
assert helper.index("if (!Term || Term->wid < 3") < (
    helper.index("fa_target_draw_bounded_line(description, 0);"))

# The same event loop applies to descriptions in all the normal target
# view paths, not the separate full-screen lore/item list. Preserve the
# latter to avoid changing their screen_save/screen_load semantics.
pieces = {
    "single object": section(
        "static ui_event target_recall_loop_object(", "static bool aux_reinit("),
    "monster and carried object": section(
        "static bool aux_monster(", "static bool aux_trap("),
    "trap": section("static bool aux_trap(", "static bool aux_object("),
    "object pile": section("static bool aux_object(", "static bool aux_terrain("),
    "terrain": section("static bool aux_terrain(", "static bool aux_wrapup("),
}
assert pieces["single object"].count("fa_target_inkey_described(") == 1
assert pieces["monster and carried object"].count("fa_target_inkey_described(") == 2
assert pieces["trap"].count("fa_target_inkey_described(") == 1
assert pieces["object pile"].count("fa_target_inkey_described(") == 1
assert pieces["terrain"].count("fa_target_inkey_described(") == 1
assert TARGET.count("fa_target_inkey_described(") == 7
assert TARGET.count("fa_target_draw_bounded_line(out_val, 0);") == 8
assert "prt(out_val, 0, 0);" not in TARGET
assert "display_object_recall_interactive" in pieces["single object"]
assert "lore_show_interactive" in pieces["monster and carried object"]
assert "screen_save();" in pieces["object pile"]
assert "screen_load();" in pieces["object pile"]
assert "target_set_monster(0);" in TARGET
assert "soft_kbd_clear(true);" in TARGET

generator = (ROOT /
    "tests/native/generate_fa_target_description_resize_host.py").read_text(
        encoding="utf-8")
runner = (ROOT /
    "scripts/test-fa-target-description-resize.sh").read_text(encoding="utf-8")
for snippet in (
    "extract(", "fa_target_draw_bounded_line",
    "fa_target_inkey_described", "EVT_RESIZE", "EVT_MOUSE",
    "EVT_BUTTON", "EVT_ESCAPE", "read_at == 2",
    "very_long", "Broken byte",
):
    assert snippet in generator, snippet
for snippet in ("-std=c99 -Wall -Wextra -Werror -pedantic",
                "generate_fa_target_description_resize_host.py"):
    assert snippet in runner, snippet
assert "test-fa-target-description-resize.sh" in VERIFY
assert "test_faangband_target_description_resize_source.py" in VERIFY

print("PASS: FA target description resize repaint and input source contracts")
