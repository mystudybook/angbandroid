#!/usr/bin/env python3
"""Compact numeric screen bounds and FA character-dump English isolation."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
BOUND = (CPP / "angband/statusline-bounds.h").read_text(encoding="utf-8")


def body(text, signature):
    start = text.index(signature)
    return text[start:text.index("\n}\n", start) + 2]


assert "static inline int angdroid_statusline_fits_field(" in BOUND
assert "columns <= (size_t)(term_width - col)" in BOUND
for engine in ("angband", "faangband"):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    fit = body(src, "static bool prt_short_field_fits(")
    assert "Term && row >= 0 && row < Term->hgt" in fit, engine
    assert "angdroid_statusline_fits_field(col, columns, Term->wid)" in fit

    for name, digits in (
        ("prt_level_short", "tmp"),
        ("prt_stat_short", "tmp"),
        ("prt_exp_short", "out_val"),
        ("prt_ac_short", "tmp"),
        ("prt_gold_short", "tmp"),
    ):
        fun = body(src, "static int " + name + "(")
        assert "prt_short_field_fits(row, col," in fun, (engine, name)
        assert "strlen(" + digits + ")" in fun, (engine, name)
        assert fun.index("prt_short_field_fits(row, col,") < (
            fun.index("put_str(") if "put_str(" in fun else len(fun)), (engine, name)

    clipped = body(src, "static int prt_vital_short_clipped(")
    assert "angdroid_menu_copy_columns(visible, sizeof(visible)," in clipped
    assert "if (!Term || row < 0 || row >= Term->hgt ||" in clipped
    assert "if (used == 3 && length <= room - used)" in clipped
    assert "if (length + 1 <= room - used)" in clipped
    assert 'c_put_str(COLOUR_WHITE, "/", row, col + (int)used);' in clipped
    assert "return (int)used + (used < room ? 1 : 0);" in clipped

    for name, label, current, maximum in (
        ("prt_hp_short", "HP:", "cur_hp", "max_hp"),
        ("prt_sp_short", "SP:", "cur_sp", "max_sp"),
    ):
        fun = body(src, "static int " + name + "(")
        assert "col < 0 || col >= Term->wid) return 0;" in fun
        assert "4 + strlen(" + current + ") + strlen(" + maximum + ")" in fun
        assert 'prt_vital_short_clipped("' + label + '"' in fun
        assert 'put_str("' + label + '", row, col);' in fun
        assert "c_put_str(COLOUR_L_GREEN, " + maximum + ", row, col);" in fun
    mana = body(src, "static int prt_sp_short(")
    assert "player->class->magic.total_spells" in mana
    assert "player->class->magic.spell_first" in mana

    health = body(src, "static int prt_health_short(")
    assert "Term->wid - col < 12" in health
    assert "len = prt_health_aux(row, col);" in health
    assert health.index("Term->wid - col < 12") < health.index(
        "len = prt_health_aux(row, col);")

fa = (CPP / "faangband/ui-player.c").read_text(encoding="utf-8")
title = body(fa, "static struct panel *get_panel_topleft(")
mid = body(fa, "static struct panel *get_panel_midleft(")
depth = body(fa, "static const char *show_depth(")
speed = body(fa, "static const char *show_speed(")
for label in ("Name", "Race", "Class", "Title", "Gold"):
    assert 'angdroid_game_ui_text("' + label + '"' in title
for label in ("Level", "Cur Exp", "Max Exp", "Adv Exp", "Burden",
              "Overweight", "Max Depth"):
    assert 'angdroid_game_ui_text("' + label + '"' in mid
assert "angdroid_zh_tw_enabled()" not in title
assert "angdroid_zh_tw_enabled()" not in mid
assert 'angdroid_character_sheet_text("Town", fa_player_display_zh_tw())' in depth
assert 'angdroid_character_sheet_text("Normal", fa_player_display_zh_tw())' in speed
assert "return angdroid_zh_tw_enabled() && !fa_player_dump_english;" in fa
dump = fa.split("void write_character_dump(ang_file *fff)", 1)[1].split(
    "bool dump_save(", 1)[0]
assert dump.index("fa_player_dump_english = true;") < dump.index(
    "display_player(2);") < dump.index("display_player(1);") < dump.index(
    "fa_player_dump_english = false;")

assert "test_statusline_bounds.c" in NATIVE
assert "test_compact_vitals_and_fa_dump_source.py" in RUNNER
print("PASS: both compact numeric fields bounded; FA screen/dump locale isolated")
