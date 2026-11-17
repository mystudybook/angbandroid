#!/usr/bin/env python3
"""FAangband statusline captions: display-only zh-TW without byte/cell overlap."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SRC=(ROOT/"app/src/main/cpp/faangband/ui-display.c").read_text(encoding="utf-8")
RUN=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
for s in (
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    'static size_t fa_status_put(',
    'angdroid_menu_copy_columns(visible, sizeof(visible),',
    'return cells + 1;',
    '"回城" : "Recall"',
    '"深入" : "Descent"',
    '"學習 (%d)" : "Study (%d)"',
    '"專精 (%d)" : "Spec. (%d)"',
    '"暫停忽略" : "Unignoring"',
    '"請稍候……" : "Please wait..."',
    'player->word_recall',
    'player->deep_descent',
    'player_book_has_unlearned_spells(player)',
):
    assert s in SRC, s
for fn in ("prt_recall", "prt_descent", "prt_study", "prt_spec", "prt_unignore"):
    body=SRC.split("static size_t "+fn+"(",1)[1].split("\n}\n",1)[0]
    assert "fa_status_put(" in body,fn
    assert "strlen(text) + 1" not in body,fn
assert "static status_f *status_handlers[]" in SRC
assert "col += status_handlers[i](row, col);" in SRC
assert "test_faangband_statusline_locale_source.py" in RUN
print("PASS: FA state labels bilingual with measured terminal-cell advance")
