#!/usr/bin/env python3
"""FAangband recall selection and history: zh-TW screen-only checks."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RECALL = (ROOT / "app/src/main/cpp/faangband/ui-recall.c").read_text(
    encoding="utf-8")
HISTORY = (ROOT / "app/src/main/cpp/faangband/ui-history.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def has(text, *items):
    for item in items:
        assert item in text, item

has(RECALL,
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    '"要取代哪個傳送點？（ESC 取消）"',
    '"要前往哪個傳送點？"',
    '"確定要取代傳送點 %s？"',
    '"取消傳送？"',
    '"尚未設定地點"',
    '"不取代傳送點"',
    '"城鎮" : "Town"',
    '"Which recall point will you replace (or ESC):"',
    '"Which recall point do you want to go to?"',
    '"Really replace recall point %s"',
    '"Cancel recall? "',
    '"No location set     "',
    '"Don\'t replace     "',
    'format("%s %d   ", locality_name(locality), level)',
    "choice[i] = i;",
    "selection = choice[oid];",
    "selection < num_entries - 2",
    "evt = menu_select(m, cursor, true);",
    "evt.type = EVT_ESCAPE;",
    "return (evt.type != EVT_ESCAPE) ? selection : -1;")

render = RECALL.split("static void recall_display(", 1)[1].split(
    "static bool recall_action(", 1)[0]
has(render,
    "char place[96];",
    "char visible[96];",
    "angdroid_menu_copy_columns(visible, sizeof(visible), place,",
    "width > 0 ? (size_t)width : 0",
    "c_put_str(attr, visible, row, col);")

has(HISTORY,
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    '#include "ui-history-viewport.h"',
    '"[角色歷史紀錄]"',
    '"      回合   地點               事件"',
    '"（遺失）"',
    '"[方向鍵捲動，p/PgUp 上頁，n/PgDn 下頁，ESC 離開]"',
    '"[Player history]"',
    '"      Turn   Place               Note"')
display = HISTORY.split("void history_display(void)", 1)[1].split(
    "void dump_history(", 1)[0]
dump = HISTORY.split("void dump_history(", 1)[1]
has(display,
    "page_size = angdroid_fa_history_page_rows(hgt);",
    "first_item = angdroid_fa_history_clamp_top(first_item, max_item);",
    "first_item = angdroid_fa_history_step(first_item,",
    "row < page_size && i < max_item",
    "fa_history_screen_row(&history_list_local[i], row + 2, wid);",
    "angdroid_fa_history_place_columns(width)",
    "if (place_width > 0)",
    "width < 40",
    "width >= 40",
    '"遺失：" : "LOST: "',
    '"（遺失）" : " (LOST)"',
    "fa_history_put_clipped(COLOUR_WHITE,",
    "if (hgt > 1)",
    "if (Term) screen_load();",
    "case KC_PGDOWN:", "case KC_PGUP:", "case ESCAPE:")
assert "page_size = hgt - 5" not in display
assert "row <= page_size" not in display
assert "int scroll_to = first_item -" not in display
assert "strnfmt(buf, sizeof(buf), \"%10ld%20s" not in display
assert "angdroid_menu_copy_columns(dest + bytes," in HISTORY
assert "angdroid_utf8_display_columns(place)" in HISTORY
assert "test-fa-history-viewport.sh" in VERIFY
VIEWPORT = (ROOT / "app/src/main/cpp/faangband/ui-history-viewport.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_fa_history_viewport.c").read_text(
    encoding="utf-8")
for token in ("static inline int angdroid_fa_history_page_rows(",
              "static inline size_t angdroid_fa_history_clamp_top(",
              "static inline size_t angdroid_fa_history_step(",
              "static inline int angdroid_fa_history_place_columns("):
    assert token in VIEWPORT and token in NATIVE, token
has(dump, 'file_putf(file, "[Player history]\\n");',
    'file_putf(file, "      Turn   Place               Note\\n");',
    'my_strcat(buf, " (LOST)", sizeof(buf));')
assert "angdroid_zh_tw_enabled()" not in dump
assert "test_faangband_recall_history_locale_source.py" in VERIFY

print("PASS: FAangband recall / history zh-TW display, English export preserved")
