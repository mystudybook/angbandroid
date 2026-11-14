#!/usr/bin/env python3
"""Display-only FAangband target detail and compact help regression guards.

Native C99 execution is handled by scripts/test-fa-direction-target.sh.
These assertions validate that every target-description path uses the
bounded UTF-8 renderer and that low-height help does not erase the map.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
TARGET = (FA / "ui-target.c").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
HOST = (ROOT / "tests/native/generate_fa_target_footer_host.py").read_text(
    encoding="utf-8")

assert '#include "../angband/menu-text-fit.h"' in TARGET
bounded_start = TARGET.index("static void fa_target_draw_bounded_line(")
compact_start = TARGET.index("static void fa_target_draw_compact_help(")
help_start = TARGET.index("void target_display_help(")
bounded = TARGET[bounded_start:compact_start]
compact = TARGET[compact_start:help_start]
help_body = TARGET[help_start:TARGET.index("\n/**\n * Return whether", help_start)]

for required in (
    "char visible[TARGET_OUT_VAL_SIZE];",
    "if (!Term || Term->hgt <= 0 || row < 0 || row >= Term->hgt)",
    "width = Term->wid - 1;",
    "if (width < 1)",
    'prt("", row, 0);',
    "angdroid_menu_copy_columns(visible, sizeof(visible), description,",
    "prt(visible, row, 0);",
):
    assert required in bounded, required
assert bounded.index("angdroid_menu_copy_columns(") < bounded.index(
    "prt(visible, row, 0);")

assert "fa_target_draw_bounded_line(" in compact
assert '"q 離開，t 設目標，方向／點擊查看，? 說明"' in compact
assert '"q: exit, t: target, dir/click: look, ?: help"' in compact

assert "if (wid <= 1 || hgt <= 0) return;" in help_body
assert "if (hgt <= HELP_HEIGHT + 2 || wid < 64)" in help_body
assert "fa_target_draw_compact_help(hgt - 1);" in help_body
assert help_body.index("fa_target_draw_compact_help(hgt - 1);") < (
    help_body.index("clear_from(help_loc);"))
assert "help_loc = hgt - HELP_HEIGHT;" in help_body
assert "clear_from(help_loc);" in help_body
assert "fa_target_draw_bounded_line(third, help_loc + 2);" in help_body
assert help_body.count("fa_target_draw_bounded_line(") == 3
assert "text_out_to_screen" not in help_body
assert "text_out_wrap =" not in help_body
assert "cmd_lookup_key(CMD_IGNORE," in help_body
assert "UN_KTRL(key)" in help_body
assert "monster || free" in help_body

# The monster/held-item/pile/ground-item/trap/terrain/hallucination
# descriptions all originated from fixed-byte strnfmt() buffers. None
# should go directly to Term_addstr via prt(out_val), which could cut a
# multibyte glyph or spill into the next terminal line.
assert TARGET.count("fa_target_draw_bounded_line(out_val, 0);") == 8
assert "prt(out_val, 0, 0);" not in TARGET

for name in ("target_recall_loop_object", "aux_hallucinate",
             "aux_monster", "aux_trap", "aux_object", "aux_terrain"):
    assert name in TARGET, name
assert "target_set_monster(0);" in TARGET
assert "soft_kbd_clear(true);" in TARGET

# The host tests compile functions extracted from production sources,
# instead of testing a handwritten reimplementation.
for name in ("fa_target_draw_short_help", "fa_target_draw_bounded_line",
             "fa_target_draw_compact_help"):
    assert name in HOST, name
assert "extract(\"fa_target_draw_bounded_line\")" in HOST
assert "extract(\"fa_target_draw_compact_help\")" in HOST
assert "help_match.group(0)" in HOST
assert "#define HELP_HEIGHT 3" in HOST
assert "full_help_clears == 1 && help_clear_start == 21" in HOST
assert "draws == HELP_HEIGHT && erases == 0" in HOST
assert "first_row == 21 && last_row == 23" in HOST
assert "width = 1; width < 80" in HOST
TARGET_H = (FA / "ui-target.h").read_text(encoding="utf-8")
assert "#define HELP_HEIGHT 3" in TARGET_H
assert "#define TARGET_OUT_VAL_SIZE 256" in TARGET_H
assert 'Truncated UTF-8: \\xe4\\xb8' in HOST
assert 'CHECK(strcmp(last_text, "Truncated ??") == 0)' in HOST
assert "invalid_row" in HOST and "overflow" in HOST
assert "test-fa-direction-target.sh" in VERIFY
assert "test_faangband_target_detail_source.py" in VERIFY

print("PASS: FA target compact help, UTF-8 description paths and source guards")
