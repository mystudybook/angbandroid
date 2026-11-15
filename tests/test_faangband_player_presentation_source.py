#!/usr/bin/env python3
"""FAangband stat headings/units translated on-screen, dump stays unchanged."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/faangband/ui-player.c").read_text(
    encoding="utf-8")
HDR = (ROOT / "app/src/main/cpp/faangband/ui-player-presentation.h").read_text(
    encoding="utf-8")
C99 = (ROOT / "tests/native/test_fa_player_presentation.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def has(source, *parts):
    for part in parts:
        assert part in source, part

has(SRC,
    '#include "ui-player-presentation.h"',
    '#include "../angband/menu-text-fit.h"',
    "static bool fa_player_dump_english = false;",
    "return angdroid_zh_tw_enabled() && !fa_player_dump_english;",
    "angdroid_menu_copy_columns(visible, sizeof(visible),",
    "fa_player_stat_heading(0, row-1, col+5, 7);",
    "fa_player_stat_heading(1, row-1, col+12, 4);",
    "fa_player_stat_heading(2, row-1, col+16, 4);",
    "fa_player_stat_heading(3, row-1, col+20, 4);",
    "fa_player_stat_heading(4, row-1, col+24, 7);")
for name in ("BLOWS", "SHOTS", "BURDEN", "OVERWEIGHT", "INFRA"):
    has(SRC, "ANGDROID_FA_SHEET_" + name)
for col in ("本值", "種族", "職業", "裝備", "最佳",
            "%d.%d/回合", "%.1f 磅", "%d.%d 磅", "%d 呎"):
    has(HDR, col)

stats = SRC.split("void display_player_stat_info(void)", 1)[1].split(
    "static void display_player_sust_info(", 1)[0]
dump = SRC.split("void write_character_dump(ang_file *fff)", 1)[1].split(
    "bool dump_save(", 1)[0]
assert 'c_put_str(COLOUR_WHITE, "  Self"' not in stats
assert dump.index("fa_player_dump_english = true;") < dump.index(
    "display_player(2);") < dump.index("display_player(1);") < dump.index(
    "fa_player_dump_english = false;")
has(dump,
    'file_putf(fff, "  [%s Character Dump]\\n\\n", buildid);',
    'file_putf(fff, "\\n  [Specialty Abilities]\\n\\n");',
    'file_putf(fff, "  [Character Equipment]\\n\\n");',
    'file_putf(fff, "\\n\\n  [Character Inventory]\\n\\n");')
assert "test-fa-player-presentation.sh" in RUNNER
assert "test_faangband_player_presentation_source.py" in RUNNER
assert "angdroid_fa_stat_heading(" in C99
assert "angdroid_fa_sheet_unit_format(" in C99
print("PASS: FA character stat captions/units localized; dump path preserved")
