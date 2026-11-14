#!/usr/bin/env python3
"""FAangband curse selection and Hall of Fame screen localization contracts."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CURSE = (ROOT / "app/src/main/cpp/faangband/ui-curse.c").read_text(
    encoding="utf-8")
SCORE = (ROOT / "app/src/main/cpp/faangband/ui-score.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def has(src, *items):
    for item in items:
        assert item in src, item

has(CURSE,
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    '"  %s（詛咒強度 %d）"',
    '"  %s (curse strength %d)"',
    '"要移除哪種詛咒（法術強度 %s）？"',
    '" Remove which curse (spell strength %s)?"',
    "angdroid_utf8_display_columns(label)",
    "header_columns = angdroid_utf8_display_columns(header);",
    "angdroid_menu_copy_columns(visible, sizeof(visible), buf,",
    "width > 0 ? (size_t)width : 0",
    "angdroid_menu_copy_columns(visible_header, sizeof(visible_header),",
    "area.width = (int)requested_width;",
    "requested_width > (size_t)max_width",
    "if (visible[0])",
    "selection = choice[oid].index;",
    "obj->known->curses[i].power > 0",
    "obj->known->curses[i].power < 100",
    "player_knows_curse(player, i)",
    "m->selections = all_letters_nohjkl;",
    "m->flags = (MN_PVT_TAGS);",
    "m->browse_hook = curse_menu_browser;",
    "return selection;")

has(SCORE,
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    'angdroid_menu_copy_columns(visible, sizeof(visible), text,',
    '"%3d.%9s  %s（%s %s），等級 %d"',
    '"%3d.%9s  %s the %s %s, level %d"',
    '"（最高 %d）"',
    '" (Max %d)"',
    '"在城鎮死於 %s"',
    '"在地下城第 %d 層死於 %s"',
    '"Killed by %s in the town"',
    '"Killed by %s on dungeon level %d"',
    '"（玩家 %s，日期 %s，金幣 %s，回合 %s）"',
    '"(User %s, Date %s, Gold %s, Turn %s)."',
    '"%s 名人榜（第 %d 名起）"',
    '"%s Hall of Fame (from position %d)"',
    '"[ESC 離開，上方向鍵上頁，其他按鍵下頁]"',
    '"[ESC 離開，其他按鍵翻至下一頁]"',
    "int footer_row = MIN(23, Term->hgt - 1);",
    "fa_score_put_clipped(attr, out_val, n * 4 + 2, 0);",
    "fa_score_put_clipped(attr, out_val, n * 4 + 3, 15);",
    "fa_score_put_clipped(attr, out_val, n * 4 + 4, 15);")

# Score storage, generation and ranking are unchanged.
has(SCORE,
    "highscore_read(scores, N_ELEMENTS(scores));",
    'build_score(&the_score, player, "nobody (yet!)", NULL);',
    "highscore_where(&the_score, scores, N_ELEMENTS(scores));",
    "highscore_add(&the_score, scores, N_ELEMENTS(scores));",
    "if (!scores[count].what[0])",
    "if (ch.code == ESCAPE)",
    "ch.code == ARROW_UP && allow_scrolling",
    "if (allow_scrolling)",
    "Term_fresh();")
assert "test_faangband_curse_score_locale_source.py" in VERIFY
print("PASS: FA curse/score zh-TW, UTF-8 columns and game invariants")
