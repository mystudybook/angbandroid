#!/usr/bin/env python3
"""Effect-choice menu: display-only bilingual labels and UTF-8 width."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SRC=(ROOT/"app/src/main/cpp/faangband/ui-effect.c").read_text(encoding="utf-8")
RUN=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def contains(*tokens):
    for s in tokens:
        assert s in SRC, s

contains(
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    '"隨機選擇以下其中一個" : "one of the following at random"',
    '"選擇效果：" : "Which effect? "',
    '"效果選單的項目數量不一致，請回報此問題。"',
    '"Mismatched count and effect list passed to effect_menu_new().  Please report this bug."',
    'angdroid_utf8_display_columns(ms[ms_count]) + 3',
    "if (!Term || Term->wid < 8 || Term->hgt < 4) return NULL;",
    "if (allow_random) ++count;",
    "ms[ms_count] = NULL;",
    "menu_setpriv(m, ms_count, ms);",
    "selection = m->cursor;",
    "if (selection == 0) {",
    "selection = -2;",
    "--selection;",
    "selection = -1;",
    "effect_menu_destroy(m);",
)
assert "strlen(ms[ms_count])" not in SRC
assert "width = MAX(width, (int)MIN(len + 3" not in SRC
assert "test_faangband_effect_menu_locale_source.py" in RUN
print("PASS: effect menu bilingual, CJK column widths and selection IDs preserved")
