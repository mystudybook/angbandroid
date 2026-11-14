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
help_match = re.search(
    r"^void target_display_help\([^\n]*\)\n\{.*?^\}",
    source, flags=re.MULTILINE | re.DOTALL)
if not help_match:
    raise SystemExit("Missing production target_display_help()")

header = r"""
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"
#define TARGET_OUT_VAL_SIZE 256
#define HELP_HEIGHT 3
#define CMD_IGNORE 1
#define KEYMAP_MODE_ORIG 0
#define KEYMAP_MODE_ROGUE 1
#define OPT(p, field) 0
#define KTRL(k) ((unsigned char)((k) & 0x1f))
#define UN_KTRL(k) ((unsigned char)(((k) < 1 || (k) > 27) ? (k) + 64 : (k) + 96))
#define strnfmt snprintf

struct fake_term { int wid, hgt; };
static struct fake_term actual = {80, 24};
static struct fake_term *Term = &actual;
static int draws, erases, invalid_row, overflow;
static int full_help_clears, help_clear_start, first_row, last_row;
static int zh_flag = 1;
static unsigned char ignore_key = 'i';
static char last_text[512];
static int angdroid_zh_tw_enabled(void) { return zh_flag; }
static int Term_get_size(int *w, int *h)
{
    if (w) *w = actual.wid;
    if (h) *h = actual.hgt;
    return 0;
}
static void clear_from(int row)
{
    ++full_help_clears;
    help_clear_start = row;
}
static unsigned char cmd_lookup_key(int cmd, int mode)
{
    (void)cmd;
    (void)mode;
    return ignore_key;
}
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
    if (first_row < 0 || row < first_row) first_row = row;
    if (row > last_row) last_row = row;
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

    /* In real play HELP_HEIGHT reserves only three rows. The
     * full help must print no more than those rows and may only
     * clear the reserved region, regardless of locale/mode. */
    {
        int zh, mode;
        for (zh = 0; zh <= 1; ++zh) {
            for (mode = 0; mode < 8; ++mode) {
                actual.wid = 80;
                actual.hgt = 24;
                zh_flag = zh;
                ignore_key = (mode & 1) ? 3 : 'i';
                draws = erases = invalid_row = overflow = 0;
                full_help_clears = 0;
                first_row = -1;
                last_row = -1;
                target_display_help((mode & 1) != 0,
                    (mode & 2) != 0, (mode & 4) != 0);
                CHECK(full_help_clears == 1 && help_clear_start == 21);
                CHECK(draws == HELP_HEIGHT && erases == 0);
                CHECK(first_row == 21 && last_row == 23);
                CHECK(invalid_row == 0 && overflow == 0);
                CHECK(angdroid_utf8_display_columns(last_text) <= 79);
                if (mode & 2) {
                    /* Item-ignore is read from the actual keymap; a
                     * control-key shortcut must keep ^c notation. */
                    CHECK(strstr(last_text,
                        (mode & 1) ? "^c" : "i") != NULL);
                }
                if (!(mode & 2) && (mode & 1 || mode & 4)) {
                    CHECK(strstr(last_text, zh ? "t 設目標" :
                        "t: target") != NULL);
                }
            }
        }
    }
    zh_flag = 1;
    actual.wid = 80;
    actual.hgt = 5;
    draws = erases = invalid_row = overflow = 0;
    full_help_clears = 0;
    first_row = -1;
    last_row = -1;
    target_display_help(true, true, false);
    CHECK(draws == 1 && full_help_clears == 0);
    CHECK(first_row == 4 && last_row == 4);
    CHECK(invalid_row == 0 && overflow == 0);

    actual.wid = 32;
    actual.hgt = 25;
    draws = erases = invalid_row = overflow = 0;
    full_help_clears = 0;
    first_row = -1;
    last_row = -1;
    target_display_help(true, false, true);
    CHECK(draws == 1 && full_help_clears == 0);
    CHECK(first_row == 24 && last_row == 24);
    CHECK(invalid_row == 0 && overflow == 0);

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
    + found.group(0) + "\n" + help_match.group(0)
    + "\n" + test, encoding="utf-8")
