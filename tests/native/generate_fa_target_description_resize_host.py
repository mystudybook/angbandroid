#!/usr/bin/env python3
"""Compile production FAangband target detail/redraw helpers with C99 mocks.

The tested function bodies are extracted directly from ui-target.c; only
terminal dimensions, event delivery, cursor placement and prt() are mocked.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "app/src/main/cpp/faangband/ui-target.c").read_text(
    encoding="utf-8")


def extract(signature):
    found = re.search(
        rf"^{re.escape(signature)}\n\{{.*?^\}}",
        source, flags=re.MULTILINE | re.DOTALL)
    if not found:
        raise SystemExit(f"Missing production {signature}")
    return found.group(0)


draw = extract("static void fa_target_draw_bounded_line(const char *description, int row)")
read = extract("static ui_event fa_target_inkey_described(" +
               "\n    const char *description, int y, int x)")

header = r"""
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"

#define TARGET_OUT_VAL_SIZE 256
#define ESCAPE 0xE000u
enum { EVT_NONE = 0, EVT_KBRD = 1, EVT_MOUSE = 2,
       EVT_RESIZE = 3, EVT_ESCAPE = 4, EVT_BUTTON = 5, EVT_OTHER = 6 };
#define EVENT_EMPTY {0}
typedef struct ui_event {
    int type;
    struct { unsigned int code; unsigned int mods; } key;
    struct { int button, x, y; } mouse;
} ui_event;

struct fake_term { int wid, hgt; };
static struct fake_term screen = {80, 24};
static struct fake_term *Term = &screen;
struct queued { ui_event event; int next_width, next_height; };
static struct queued queue[16];
static size_t queued, read_at;
static int paints, clears, cursor_calls, last_cursor_y, last_cursor_x;
static int overflow, invalid_row;
static char last_line[512];

static void prt(const char *s, int row, int col)
{
    (void)col;
    if (!Term || row < 0 || row >= Term->hgt) ++invalid_row;
    if (!s || !s[0]) { ++clears; return; }
    ++paints;
    snprintf(last_line, sizeof(last_line), "%s", s);
    if (!Term || Term->wid <= 1 ||
        angdroid_utf8_display_columns(s) > (size_t)(Term->wid - 1))
        ++overflow;
}
static void move_cursor_relative(int y, int x)
{
    ++cursor_calls;
    last_cursor_y = y;
    last_cursor_x = x;
}
static ui_event inkey_ex(void)
{
    ui_event fallback = {EVT_ESCAPE, {0, 0}, {0, 0, 0}};
    struct queued item;
    if (read_at >= queued) return fallback;
    item = queue[read_at++];
    if (item.event.type == EVT_RESIZE && Term) {
        Term->wid = item.next_width;
        Term->hgt = item.next_height;
    }
    return item.event;
}
static void reset(int width, int height)
{
    screen.wid = width;
    screen.hgt = height;
    Term = &screen;
    queued = read_at = 0;
    paints = clears = cursor_calls = last_cursor_y = last_cursor_x = 0;
    overflow = invalid_row = 0;
    last_line[0] = '\0';
}
static void push(int type, unsigned int code, int button,
                 int width, int height)
{
    struct queued *item = &queue[queued++];
    memset(item, 0, sizeof(*item));
    item->event.type = type;
    item->event.key.code = code;
    item->event.mouse.button = button;
    item->next_width = width;
    item->next_height = height;
}
"""
tests = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    ui_event event;
    static const char *long_label =
        "你看到一個非常古老的魔法物品與罕見怪物，具有很長很長的描述。";

    reset(80, 24);
    push(EVT_RESIZE, 0, 0, 24, 24);
    push(EVT_RESIZE, 0, 0, 12, 24);
    push(EVT_BUTTON, 'r', 0, 0, 0);
    event = fa_target_inkey_described(long_label, 45, 21);
    CHECK(event.type == EVT_KBRD && event.key.code == 'r');
    CHECK(read_at == 3 && paints == 2 && clears == 0);
    CHECK(cursor_calls == 2);
    CHECK(last_cursor_y == 45 && last_cursor_x == 21);
    CHECK(overflow == 0 && invalid_row == 0);
    CHECK(angdroid_utf8_display_columns(last_line) <= 11);

    reset(80, 24);
    push(EVT_ESCAPE, 0, 0, 0, 0);
    event = fa_target_inkey_described("Test", 1, 1);
    CHECK(event.type == EVT_KBRD && event.key.code == ESCAPE);
    CHECK(event.key.mods == 0 && read_at == 1);
    CHECK(paints == 0 && cursor_calls == 0);

    reset(80, 24);
    push(EVT_MOUSE, 0, 1, 0, 0);
    event = fa_target_inkey_described("Mouse", 1, 1);
    CHECK(event.type == EVT_MOUSE && event.mouse.button == 1);

    reset(80, 24);
    push(EVT_KBRD, 't', 0, 0, 0);
    event = fa_target_inkey_described("Keyboard", 1, 1);
    CHECK(event.type == EVT_KBRD && event.key.code == 't');

    reset(80, 24);
    push(EVT_NONE, 0, 0, 0, 0);
    push(EVT_OTHER, 0, 0, 0, 0);
    push(EVT_KBRD, 'q', 0, 0, 0);
    event = fa_target_inkey_described("Ignore noninput", 1, 1);
    CHECK(event.type == EVT_KBRD && event.key.code == 'q');
    CHECK(read_at == 3 && paints == 0);

    reset(80, 24);
    push(EVT_RESIZE, 0, 0, 12, 24);
    push(EVT_RESIZE, 0, 0, 2, 24);
    push(EVT_KBRD, 't', 0, 0, 0);
    event = fa_target_inkey_described(long_label, 1, 1);
    CHECK(event.type == EVT_KBRD && event.key.code == ESCAPE);
    CHECK(event.key.mods == 0);
    CHECK(read_at == 2 && paints == 1 && clears == 1);
    CHECK(cursor_calls == 1 && overflow == 0);

    reset(80, 24);
    push(EVT_RESIZE, 0, 0, 60, 0);
    push(EVT_KBRD, 't', 0, 0, 0);
    event = fa_target_inkey_described(long_label, 1, 1);
    CHECK(event.type == EVT_KBRD && event.key.code == ESCAPE);
    CHECK(read_at == 1 && cursor_calls == 0);

    reset(80, 24);
    fa_target_draw_bounded_line("Broken byte \xe4\xb8", 0);
    CHECK(strcmp(last_line, "Broken byte ??") == 0);
    CHECK(overflow == 0 && invalid_row == 0);

    reset(80, 24);
    {
        char very_long[2048];
        size_t i;
        for (i = 0; i < 600; ++i)
            memcpy(very_long + i * 3, "甲", 3);
        very_long[1800] = '\0';
        fa_target_draw_bounded_line(very_long, 0);
        CHECK(overflow == 0 && invalid_row == 0);
        CHECK(angdroid_utf8_display_columns(last_line) <= 79);
    }

    printf("PASS: %lu production target detail input/resize checks\n",
           checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_target_description_resize_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + draw + "\n" + read + "\n" + tests, encoding="utf-8")
