#!/usr/bin/env python3
"""Compile the real FAangband repeat-direction handler with host input mocks."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
code = (ROOT / "app/src/main/cpp/faangband/ui-input.c").read_text(
    encoding="utf-8")
match = re.search(
    r"^static bool textui_get_rep_dir\(int \*dp, bool allow_5\)\n\{.*?^\}",
    code, flags=re.MULTILINE | re.DOTALL)
if not match:
    raise SystemExit("Missing production textui_get_rep_dir()")

header = r"""
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"

enum {
    EVT_NONE = 0, EVT_KBRD = 1, EVT_MOUSE = 2, EVT_ESCAPE = 3,
    EVT_RESIZE = 4, EVENT_MESSAGE_FLUSH = 5
};
#define ESCAPE 0xE000u
#define SCAN_INSTANT 65535u
#define SCAN_OFF 0u
typedef struct keypress { unsigned int code; } keypress;
typedef struct ui_event {
    int type;
    keypress key;
    struct { int button, x, y; } mouse;
} ui_event;
#define KEY_GRID_X(e) ((e).mouse.x)
#define KEY_GRID_Y(e) ((e).mouse.y)
struct loc { int x, y; };
static struct loc loc(int x, int y)
{
    struct loc result = {x, y};
    return result;
}
struct fake_term { int wid, hgt; };
static struct fake_term terminal = {80, 24};
static struct fake_term *Term = &terminal;
struct fake_player {
    struct loc grid;
    struct { int lazymove_delay; } opts;
};
static struct fake_player game_player = {{10, 10}, {0}};
static struct fake_player *player = &game_player;
static unsigned int inkey_scan;
static int dir_transitions[10][10];
struct queued_event { ui_event event; int next_width; };
static struct queued_event queue[16];
static size_t queued, consumed;
static int draws, erases, bells, overflow, message_flushes;
static char drawn[256];

static const char *angdroid_game_ui_text(const char *s, int zh)
{
    (void)s;
    return zh ? "請選擇方向或點擊位置（Esc 取消）" :
        "Direction or click (Esc to cancel)?";
}
static int angdroid_zh_tw_enabled(void) { return 1; }
static void prt(const char *s, int row, int col)
{
    (void)row; (void)col;
    if (!s || !s[0]) { ++erases; return; }
    ++draws;
    snprintf(drawn, sizeof(drawn), "%s", s);
    if (angdroid_utf8_display_columns(s) >
        (size_t)(terminal.wid - 1)) ++overflow;
}
static void bell(void) { ++bells; }
static void event_signal(int e)
{
    if (e == EVENT_MESSAGE_FLUSH) ++message_flushes;
}
static int pathfind_direction_to(struct loc from, struct loc to)
{
    return to.x > from.x ? 6 : to.x < from.x ? 4 :
        to.y > from.y ? 2 : 8;
}
static int target_dir_allow(keypress key, bool allow_5)
{
    int d = key.code >= '0' && key.code <= '9' ?
        (int)(key.code - '0') : 0;
    if (d == 5 && !allow_5) return 0;
    return d;
}
static ui_event inkey_ex(void)
{
    ui_event none = {EVT_NONE, {0}, {0, 0, 0}};
    struct queued_event item;
    if (consumed >= queued) return none;
    item = queue[consumed++];
    if (item.event.type == EVT_RESIZE)
        terminal.wid = item.next_width;
    return item.event;
}
static void reset(int width, int lazy_delay)
{
    int a, b;
    terminal.wid = width;
    terminal.hgt = 24;
    player->opts.lazymove_delay = lazy_delay;
    queued = consumed = 0;
    draws = erases = bells = overflow = message_flushes = 0;
    inkey_scan = SCAN_OFF;
    drawn[0] = '\0';
    for (a = 0; a < 10; ++a)
        for (b = 0; b < 10; ++b)
            dir_transitions[a][b] = b;
}
static void push(int type, unsigned int code,
    int button, int x, int y, int next_width)
{
    struct queued_event *slot = &queue[queued++];
    slot->event.type = type;
    slot->event.key.code = code;
    slot->event.mouse.button = button;
    slot->event.mouse.x = x;
    slot->event.mouse.y = y;
    slot->next_width = next_width;
}
"""

tests = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    int direction = -1;

    /* Instant direction skips prompting and leaves the scanner normal. */
    reset(80, 0);
    push(EVT_KBRD, '6', 0, 0, 0, 0);
    CHECK(textui_get_rep_dir(&direction, false));
    CHECK(direction == 6 && draws == 0 && bells == 0);
    CHECK(inkey_scan == SCAN_OFF && consumed == 1);

    reset(16, 0);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_KBRD, '8', 0, 0, 0, 0);
    CHECK(textui_get_rep_dir(&direction, false));
    CHECK(direction == 8 && draws == 1 && erases == 1);
    CHECK(overflow == 0 && bells == 0);
    CHECK(angdroid_utf8_display_columns(drawn) <= 15);

    /* Initial and prompted resizes are not unrecognized directions. */
    reset(80, 0);
    push(EVT_RESIZE, 0, 0, 0, 0, 24);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_KBRD, '4', 0, 0, 0, 0);
    CHECK(textui_get_rep_dir(&direction, false));
    CHECK(direction == 4 && draws == 1 && bells == 0);
    CHECK(consumed == 3 && overflow == 0);

    reset(80, 0);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_RESIZE, 0, 0, 0, 0, 12);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_KBRD, '2', 0, 0, 0, 0);
    CHECK(textui_get_rep_dir(&direction, false));
    CHECK(direction == 2 && draws == 2 && bells == 0);
    CHECK(overflow == 0 && consumed == 4);

    reset(80, 0);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_ESCAPE, 0, 0, 0, 0, 0);
    CHECK(!textui_get_rep_dir(&direction, false));
    CHECK(direction == 0 && erases == 1 && bells == 0);

    reset(30, 0);
    push(EVT_MOUSE, 0, 1, 12, 10, 0);
    CHECK(textui_get_rep_dir(&direction, false));
    CHECK(direction == 6);
    reset(30, 0);
    push(EVT_MOUSE, 0, 2, 0, 0, 0);
    CHECK(!textui_get_rep_dir(&direction, false));
    CHECK(direction == 0);

    /* A resize while waiting for lazy second key must cancel pending
     * movement, restore SCAN_OFF, and neither ring nor consume a move. */
    reset(80, 25);
    push(EVT_KBRD, '6', 0, 0, 0, 0);
    push(EVT_RESIZE, 0, 0, 0, 0, 16);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_KBRD, '4', 0, 0, 0, 0);
    CHECK(textui_get_rep_dir(&direction, false));
    CHECK(direction == 4 && bells == 0 && overflow == 0);
    CHECK(inkey_scan == SCAN_OFF && consumed == 4);

    /* A tiny display must not ask for an unreadable direction or eat
     * a queued key after the resize has invalidated the prompt. */
    reset(80, 0);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_RESIZE, 0, 0, 0, 0, 2);
    push(EVT_NONE, 0, 0, 0, 0, 0);
    push(EVT_KBRD, '6', 0, 0, 0, 0);
    CHECK(!textui_get_rep_dir(&direction, false));
    CHECK(direction == 0 && erases == 1 && bells == 0);
    CHECK(consumed == 3 && overflow == 0);

    printf("PASS: %lu FA production direction/resize checks\n", checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_direction_input_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + match.group(0) + "\n" + tests, encoding="utf-8")
