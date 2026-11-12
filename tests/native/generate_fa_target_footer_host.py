#!/usr/bin/env python3
"""Host C99 regression for the actual FAangband targeting footer renderer."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "app/src/main/cpp/faangband/ui-target.c").read_text(
    encoding="utf-8")
found = re.search(r"^static void fa_target_draw_short_help\(int row\)\n\{.*?^\}",
                  source, flags=re.MULTILINE | re.DOTALL)
if not found:
    raise SystemExit("Missing production fa_target_draw_short_help()")

header = r"""
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"

struct fake_term { int wid, hgt; };
static struct fake_term actual = {80, 24};
static struct fake_term *Term = &actual;
static int draws, erases, invalid_row, overflow;
static char last_text[128];
static int angdroid_zh_tw_enabled(void) { return 1; }
static const char *angdroid_game_ui_text(const char *text, int zh)
{
    (void)text; (void)zh;
    return "按下問號查看目標選取操作說明？";
}
static void prt(const char *text, int row, int col)
{
    (void)col;
    if (row < 0 || !Term || row >= Term->hgt) ++invalid_row;
    if (!text || !text[0]) { ++erases; return; }
    ++draws;
    snprintf(last_text, sizeof(last_text), "%s", text);
    if (angdroid_utf8_display_columns(text) >
        (size_t)(Term->wid - 1)) ++overflow;
}
"""

test = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    int width;
    for (width = 1; width < 80; ++width) {
        actual.wid = width;
        actual.hgt = 24;
        draws = erases = invalid_row = overflow = 0;
        fa_target_draw_short_help(23);
        CHECK(invalid_row == 0 && overflow == 0);
        if (width == 1) {
            CHECK(draws == 0 && erases == 1);
        } else {
            CHECK(draws + erases == 1);
            CHECK(angdroid_utf8_display_columns(last_text) <=
                (size_t)(width - 1));
        }
    }
    actual.wid = 16;
    actual.hgt = 24;
    draws = erases = invalid_row = overflow = 0;
    fa_target_draw_short_help(-1);
    fa_target_draw_short_help(24);
    CHECK(draws == 0 && erases == 0 && invalid_row == 0);
    actual.hgt = 8;
    fa_target_draw_short_help(7);
    CHECK(draws == 1 && invalid_row == 0 && overflow == 0);
    actual.hgt = 0;
    fa_target_draw_short_help(0);
    CHECK(draws == 1 && invalid_row == 0);
    Term = NULL;
    fa_target_draw_short_help(0);
    CHECK(draws == 1 && invalid_row == 0);
    printf("PASS: %lu FA target footer UTF-8 and resized row checks\n", checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_target_footer_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + found.group(0) + "\n" + test, encoding="utf-8")
