#!/usr/bin/env python3
"""Angband status indicators: bilingual labels, preserved logic, CJK widths.

Static source checks only; this does not execute NDK or gameplay tests.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/angband/ui-display.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def fn(name: str) -> str:
    marker = "static size_t " + name + "("
    start = SRC.index(marker)
    return SRC[start:SRC.index("\n}\n", start) + 2]


# A single width-aware helper is shared across the new status paths.
assert '#include "menu-text-fit.h"' in SRC
helper = fn("ang_status_put")
assert "angdroid_menu_copy_columns(visible, sizeof(visible)," in helper
assert "Term->wid - col" in helper
assert "return cells + 1;" in helper
exact = fn("ang_status_put_exact")
assert "ang_status_put(attr, message, row, col)" in exact
assert "return advance ? advance - 1 : 0;" in exact

# Feeling numbers, unexplored markers and their colour categories stay intact.
feeling = fn("prt_level_feeling")
assert '"層感:" : "LF:"' in feeling
assert "angdroid_utf8_display_columns(label)" in feeling
assert "OPT(player, birth_feelings)" in feeling
assert "cave->feeling_squares < z_info->feeling_need" in feeling
assert "mon_feeling_color[mon_feeling]" in feeling
assert "obj_feeling_color_print" in feeling
assert "11-obj_feeling" in feeling and "10-mon_feeling" in feeling

light = fn("prt_light")
assert '"光源 %d " : "Light %d "' in light
assert "square_light(cave, player->grid)" in light
assert "COLOUR_YELLOW" in light and "COLOUR_PURPLE" in light
assert "ang_status_put_exact(" in light
assert "ABS(light) > 9" not in light

moves = fn("prt_moves")
assert '"移動 +%d "' in moves and '"Moves +%d "' in moves
assert '"移動 -%d "' in moves and '"Moves -%d "' in moves
assert "player->state.num_moves" in moves
assert "ABS(i)" in moves
assert "COLOUR_L_TEAL" in moves
assert moves.count("ang_status_put_exact(") == 2
assert "return 0;" in moves

dtrap = fn("prt_dtrap")
assert '"偵陷 " : "DTrap "' in dtrap
assert "square_isdtrap(cave, player->grid)" in dtrap
assert "square_dtrap_edge(cave, player->grid)" in dtrap
assert "COLOUR_YELLOW : COLOUR_L_GREEN" in dtrap
assert "ang_status_put_exact(" in dtrap

unignore = fn("prt_unignore")
assert '"暫停忽略" : "Unignoring"' in unignore
assert "player->unignoring" in unignore
assert "ang_status_put(COLOUR_WHITE," in unignore

# Existing functionality and English presentation are unchanged.
assert '"回城" : "Recall"' in fn("prt_recall")
assert '"深入" : "Descent"' in fn("prt_descent")
assert '"學習 (%d)" : "Study (%d)"' in fn("prt_study")
assert "prt_level_feeling, prt_light, prt_moves, prt_unignore" in SRC
assert "prt_descent, prt_state, prt_study, prt_tmd, prt_dtrap, prt_terrain" in SRC
assert "col = angdroid_statusline_next_col(col," in SRC
assert "status_handlers[i](row, col), Term->wid);" in SRC
assert "test_angband_remaining_statusline_locale_source.py" in RUN

print("PASS: Angband feeling/light/moves/dtrap/unignore UI translation contracts")
