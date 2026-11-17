#!/usr/bin/env python3
"""Canonical shape and FA locality names must be display-only localized.

Source-contract regression; the C99 lookup behavior has a separate host test.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
LIB = ROOT / "variant_lib"
SHAPES = (CPP / "angband/zh-tw-shape-display.h").read_text(encoding="utf-8")
FA_SHAPES = (CPP / "angband/zh-tw-faangband-shapes.h").read_text(encoding="utf-8")
LOCALITIES = (CPP / "angband/zh-tw-faangband-localities.h").read_text(encoding="utf-8")
FA_LOCALITIES = (CPP / "faangband/list-localities.h").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


def body(src: str, signature: str) -> str:
    start = src.index(signature)
    return src[start:src.index("\n}\n", start) + 2]


def keys(src: str):
    pairs = re.findall(r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}', src)
    assert len(pairs) == len(set(en for en, _ in pairs))
    return set(en for en, _ in pairs)


common = keys(SHAPES)
fa = keys(FA_SHAPES)
assert len(common) == 9 and len(fa) == 14 and not (common & fa)
for engine in ("angband", "faangband"):
    raw = (LIB / engine / "gamedata/shape.txt").read_text(encoding="utf-8")
    canonical = {line[5:] for line in raw.splitlines()
                 if line.startswith("name:")}
    assert canonical == (common if engine == "angband" else common | fa), (
        engine, sorted(canonical ^ (common if engine == "angband" else common | fa)))
assert "if (!english || !zh_tw) return english;" in SHAPES
assert "angdroid_faangband_shape_name(english, zh_tw)" in SHAPES

loc_names = re.findall(r'LOC\(\s*([A-Z_]+),\s*"([^"]+)"\s*\)', FA_LOCALITIES)
assert len(loc_names) == 38
canonical_localities = {name for symbol, name in loc_names if symbol != "MAX"}
assert len(canonical_localities) == 37
assert keys(LOCALITIES) == canonical_localities
assert "if (!english || !zh_tw) return english;" in LOCALITIES
assert "return english;" in LOCALITIES

for engine, inc in (
    ("angband", '"zh-tw-shape-display.h"'),
    ("faangband", '"../angband/zh-tw-shape-display.h"'),
):
    src = (CPP / engine / "ui-display.c").read_text(encoding="utf-8")
    assert inc in src
    fmt_title = body(src, "static int fmt_title(")
    assert "my_strcpy(buf, player->shape->name, max)" in fmt_title
    assert "my_strcpy(buf, player->class->title[" in fmt_title
    title = body(src, "static const char *angdroid_visible_player_title(")
    assert "if (!angdroid_zh_tw_enabled()) return canonical;" in title
    assert "!player->wizard && !player->total_winner" in title
    assert "player->lev <= PY_MAX_LEVEL && player_is_shapechanged(player)" in title
    assert "angdroid_shape_display_name(player->shape->name, true)" in title
    assert "angdroid_zh_tw_class_title(canonical, true)" in title
    assert "prt_field(angdroid_visible_player_title(buf), row, col);" in src
    compact = body(src, "static int prt_title_short(")
    assert "angdroid_visible_player_title(buf)" in compact

ang = (CPP / "angband/ui-display.c").read_text(encoding="utf-8")
ang_depth = body(ang, "static void prt_depth(")
assert "angdroid_character_sheet_text(depths, true)" in ang_depth
assert "if (width > 13) width = 13;" in ang_depth
assert "Term_erase(col, row, (int)width);" in ang_depth
assert "angdroid_menu_copy_columns(visible, sizeof(visible)," in ang_depth
assert 'put_str(format("%-13s", depths), row, col);' in ang_depth

fa_src = (CPP / "faangband/ui-display.c").read_text(encoding="utf-8")
assert '"../angband/zh-tw-faangband-localities.h"' in fa_src
fa_fmt_depth = body(fa_src, "static int fmt_depth(")
assert "angdroid_faangband_locality_text(" in fa_fmt_depth
assert "locality_name(lev->locality), true" in fa_fmt_depth
assert '"%s %d"' in fa_fmt_depth and '"%s 城鎮"' in fa_fmt_depth
assert "angdroid_utf8_copy_complete(buf, (size_t)max, display)" in fa_fmt_depth
assert "angdroid_utf8_display_columns(buf)" in fa_fmt_depth
assert "my_strcpy(buf, level_name(lev), max);" in fa_fmt_depth
fa_depth = body(fa_src, "static size_t prt_depth(")
assert "char depths[160];" in fa_depth
assert "return fa_status_put(COLOUR_WHITE, depths, row, col);" in fa_depth
assert "return strlen(depths) + 1;" in fa_depth
compact_depth = body(fa_src, "static int prt_depth_short(")
assert "char buf[160];" in compact_depth
assert "prt_compact_zh_caption(" in compact_depth
assert "angdroid_character_sheet_text(buf, true)" in compact_depth

assert "test_shape_locality_display_locale.c" in NATIVE
assert "test_shape_locality_display_source.py" in VERIFY
print("PASS: 23 shape names and 37 FA localities with unchanged game IDs")
