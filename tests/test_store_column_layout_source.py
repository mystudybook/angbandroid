#!/usr/bin/env python3
"""Guard Angband shop layout on phone-width and rotated terminals.

The standalone C99 test validates placement arithmetic; this check verifies
that the actual store view uses it. Does not run the Android app.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-store.c").read_text(encoding="utf-8")

if '#include "store-column-layout.h"' not in source:
    raise SystemExit("FAIL: store column layout not included")
if '#include "menu-text-fit.h"' not in source:
    raise SystemExit("FAIL: store text lacks Unicode terminal-cell clipping")

painter = source.split("static void store_put_clipped(", 1)[1].split("\n}", 1)[0]
if "angdroid_menu_copy_columns(" not in painter:
    raise SystemExit("FAIL: store clipping does not preserve full UTF-8 glyphs")

recalc = source.split("static void store_display_recalc(", 1)[1].split(
    "\n/**\n * Redisplay a single store entry", 1
)[0]
for part in (
    "angdroid_store_columns(",
    "columns.price_col >= 0 ? columns.price_col : 0",
    "columns.weight_col >= 0 ? columns.weight_col : 0",
    "angdroid_store_safe_row(",
    "menu_layout(m, &loc)",
):
    if part not in recalc:
        raise SystemExit(f"FAIL: shop geometry not bounded: {part}")
for old in ("= wid - 14;", "= wid - 26;", "hgt -= 3;"):
    if old in recalc:
        raise SystemExit(f"FAIL: underflow-prone coordinate remains: {old}")

entry = source.split("static void store_display_entry(", 1)[1].split(
    "\n/**\n * Display store (after clearing screen)", 1
)[0]
for part in (
    "angdroid_store_name_budget(columns, col, width)",
    "store_put_clipped(obj->kind->base->attr, o_name, row, col, name_width)",
    "if (columns.weight_col >= 0 && columns.weight_col < col + width)",
    "if (columns.price_col >= 0 && columns.price_col < col + width)",
    "store_put_clipped(colour, out_val, row, columns.price_col",
):
    if part not in entry:
        raise SystemExit(f"FAIL: store item text/column overlap: {part}")
if "c_put_str(obj->kind->base->attr, o_name, row, col);" in entry:
    raise SystemExit("FAIL: raw unbounded item name rendering remains")

frame = source.split("static void store_display_frame(", 1)[1].split(
    "\n/**\n * Display help.", 1
)[0]
if frame.count("store_put_clipped(") < 5:
    raise SystemExit("FAIL: shop headers/owner not clipped")
if "summary_cols + owner_cols + 4" not in frame:
    raise SystemExit("FAIL: owner and right-aligned shop text may overlap")
if "ctx->scr_places_x[LOC_OWNER] - strlen(buf)" in frame:
    raise SystemExit("FAIL: unsigned owner position underflow remains")

redraw = source.split("static void store_redraw(", 1)[1].split(
    "\nstatic bool store_get_check(", 1
)[0]
if redraw.count("store_put_clipped(") < 2:
    raise SystemExit("FAIL: gold and help text bypass cell clipping")

resize = source.split("static void store_menu_resize(", 1)[1].split(
    "static region store_menu_region", 1
)[0]
if "store_display_recalc(ctx);" not in resize or "store_redraw(ctx);" not in resize:
    raise SystemExit("FAIL: layout columns not refreshed on resize")
if "store_menu_resize\n};" not in source:
    raise SystemExit("FAIL: resize hook not registered with shop menu")

print("PASS: store headers, name, gold, prices and rotation use bounded cells")
