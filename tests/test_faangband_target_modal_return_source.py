#!/usr/bin/env python3
"""FAangband monster/object target modal close must not consume a second key.

The two interactive detail viewers already await their closing keystroke in
textui_textblock_show(). Target inspection should redraw the description
after they return, rather than making the user type again.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
target = (FA / "ui-target.c").read_text(encoding="utf-8")
monster = (FA / "ui-mon-lore.c").read_text(encoding="utf-8")
item = (FA / "ui-object.c").read_text(encoding="utf-8")
output = (FA / "ui-output.c").read_text(encoding="utf-8")
verify = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def between(text, start, stop):
    assert start in text and stop in text, (start, stop)
    return text.split(start, 1)[1].split(stop, 1)[0]


def modal_branch(function, call):
    before = between(target, function, "static bool aux_reinit("
        if "target_recall_loop_object" in function else "static bool aux_trap(")
    assert "if (recall) {" in before
    branch = between(before, "if (recall) {", "} else {")
    assert call in branch
    assert "recall = false;" in branch
    assert "continue;" in branch
    assert "inkey_m();" not in branch
    assert branch.index(call) < branch.index("recall = false;") < (
        branch.index("continue;"))
    return before


single = modal_branch("static ui_event target_recall_loop_object(",
                      "display_object_recall_interactive(cave->objects[obj->oidx]);")
normal = modal_branch("static bool aux_monster(",
                      "lore_show_interactive(mon->race, lore);")
assert "press = fa_target_inkey_described(out_val, y, x);" in single
assert "auxst->press = fa_target_inkey_described(out_val," in normal
assert "recall = !recall;" in single
assert "recall = !recall;" in normal

# Both modal renderers already block in the textblock UI.
object_modal = between(item,
    "void display_object_recall_interactive(struct object *obj)",
    "void textui_obj_examine(void)")
monster_modal = between(monster,
    "void lore_show_interactive(const struct monster_race *race,",
    "void lore_show_subwindow(")
assert "textui_textblock_show(tb, SCREEN_REGION, header_buf);" in object_modal
assert "textui_textblock_show(tb, SCREEN_REGION, NULL);" in monster_modal
assert "textblock_free(tb);" in object_modal
assert "textblock_free(tb);" in monster_modal
textblock = between(output, "struct keypress textui_textblock_show(",
                    "void text_out_to_screen(")
assert "input = inkey_ex();" in textblock
assert "ch = fa_textblock_key_from_event(input);" in textblock
assert "if (input.type == EVT_RESIZE || input.type == EVT_NONE) continue;" in textblock
assert "screen_save();" in textblock and "screen_load();" in textblock
assert "test_faangband_target_modal_return_source.py" in verify
print("PASS: FA target modal detail returns without a second blocking input")
