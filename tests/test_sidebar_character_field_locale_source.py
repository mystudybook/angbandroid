#!/usr/bin/env python3
"""Character sidebar and compact topbar CJK column safety contracts.

Static integration checks: native C99 glyph tests are registered separately.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


def body(src: str, signature: str) -> str:
    start = src.index(signature)
    return src[start:src.index("\n}\n", start) + 2]


for engine, includes in (
    ("angband", ('#include "zh-tw-ui.h"',
                 '#include "zh-tw-class-titles.h"',
                 '#include "zh-tw-character-sheet.h"')),
    ("faangband", ('#include "../angband/zh-tw-ui.h"',
                   '#include "../angband/zh-tw-class-titles.h"',
                   '#include "../angband/zh-tw-character-sheet.h"')),
):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    for inc in includes:
        assert inc in src, engine

    field = body(src, "static void prt_field(")
    assert "if (angdroid_zh_tw_enabled())" in field
    assert "if (!Term || row < 0 || row >= Term->hgt ||" in field
    assert "col < 0 || col >= Term->wid) return;" in field
    assert "width = (size_t)(Term->wid - col);" in field
    assert "if (width > 13) width = 13;" in field
    assert "Term_erase(col, row, (int)width);" in field
    assert "angdroid_menu_copy_columns(visible, sizeof(visible), info, width)" in field
    assert "if (visible[0]) c_put_str(COLOUR_L_BLUE, visible, row, col);" in field
    assert 'c_put_str(COLOUR_WHITE, "             ", row, col);' in field
    assert "c_put_str(COLOUR_L_BLUE, info, row, col);" in field

    title = body(src, "static void prt_title(")
    assert "angdroid_zh_tw_class_title(buf," in title
    race = body(src, "static void prt_race(")
    player_class = body(src, "static void prt_class(")
    assert "angdroid_game_ui_text(player->race->name," in race
    assert "angdroid_game_ui_text(player->class->name," in player_class

    compact = body(src, "static int prt_compact_zh_caption(")
    assert "char visible[512]" in compact
    assert "if (!Term || !label || row < 0 || row >= Term->hgt ||" in compact
    assert "col < 0 || col >= Term->wid) return 0;" in compact
    assert "available = (size_t)(Term->wid - col);" in compact
    assert "angdroid_menu_copy_columns(visible, sizeof(visible)," in compact
    assert "return (int)used + (used < available ? 1 : 0);" in compact

    combined = body(src, "static int prt_race_class_short(")
    assert "if (player_is_shapechanged(player)) return 0;" in combined
    assert "angdroid_game_ui_text(player->race->name, true)" in combined
    assert "angdroid_zh_tw_class_title(" in combined
    assert "prt_compact_zh_caption(buf, COLOUR_L_GREEN, row, col)" in combined
    assert "return strlen(buf)+1;" in combined  # English fallback

    depth = body(src, "static int prt_depth_short(")
    assert "angdroid_character_sheet_text(buf, true)" in depth, engine

    for fn in ("prt_speed_short", "prt_depth_short", "prt_title_short"):
        compact_caller = body(src, "static int " + fn + "(")
        assert "if (angdroid_zh_tw_enabled())" in compact_caller, (engine, fn)
        assert "prt_compact_zh_caption(" in compact_caller, (engine, fn)

    compact_window = body(src, "static void update_player_compact_subwindow(")
    assert "angdroid_game_ui_text(player->race->name," in compact_window
    assert "angdroid_game_ui_text(player->class->name," in compact_window

assert "test_sidebar_character_field_utf8.c" in NATIVE
assert "test_sidebar_character_field_locale_source.py" in VERIFY
print("PASS: bilingual 13-cell sidebar and bounded topbar names in both engines")
