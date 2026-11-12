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


def extract(name):
    fn = re.search(
        rf"^static void {name}\([^\n]*\)\n\{{.*?^\}}",
        source, flags=re.MULTILINE | re.DOTALL)
    if not fn:
        raise SystemExit(f"Missing production {name}()")
    return fn.group(0)


bounded = extract("fa_target_draw_bounded_line")
compact = extract("fa_target_draw_compact_help")

header = r"""
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"
#define TARGET_OUT_VAL_SIZE 256

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
    /* Real monster/item/terrain lines must never render partial UTF-8
     * or extend into another cell, including after a byte-limited format
     * ends midway through a Chinese character. */
    {
        static const char *descriptions[] = {
            "罕見的魔法武器，具有十分長的描述文字。",
            "An exceptionally long object description in English.",
            "精靈😀和其他寬字元",
            "Truncated UTF-8: \xe4\xb8", "", NULL
        };
        size_t d;
        for (d = 0; d < sizeof(descriptions) / sizeof(descriptions[0]); ++d) {
            for (width = 1; width < 80; ++width) {
                actual.wid = width;
                actual.hgt = 12;
                draws = erases = invalid_row = overflow = 0;
                last_text[0] = '\0';
                fa_target_draw_bounded_line(descriptions[d], 0);
                CHECK(invalid_row == 0 && overflow == 0);
                CHECK(draws + erases == 1);
                CHECK(angdroid_utf8_display_columns(last_text) <=
                    (size_t)(width - 1));
                if (width == 1) CHECK(erases == 1);
            }
        }
        actual.wid = 80;
        actual.hgt = 12;
        fa_target_draw_bounded_line("Truncated \xe4\xb8", 0);
        CHECK(strcmp(last_text, "Truncated ??") == 0);

        /* Compact help occupies just the footer. It needs no multir
         * text engine and must remain valid on landscape height=1. */
        for (width = 1; width < 80; ++width) {
            actual.wid = width;
            actual.hgt = 1;
            draws = erases = invalid_row = overflow = 0;
            last_text[0] = '\0';
            fa_target_draw_compact_help(0);
            CHECK(invalid_row == 0 && overflow == 0);
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
    header + "\n" + bounded + "\n" + compact + "\n"
    + found.group(0) + "\n" + test, encoding="utf-8")
