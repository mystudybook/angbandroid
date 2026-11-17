#!/usr/bin/env python3
"""Extract production FAangband get_com_ex() and test UTF-8 resize handling.

The host fixture replaces only the UI terminal, event queue and localization
dependencies.  The C function body is taken verbatim from ui-input.c.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "app/src/main/cpp/faangband/ui-input.c").read_text(
    encoding="utf-8")
found = re.search(
    r"^bool get_com_ex\(const char \*prompt, ui_event \*command\)\n\{.*?^\}",
    source, flags=re.MULTILINE | re.DOTALL)
if not found:
    raise SystemExit("Missing production get_com_ex()")

header = r"""
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"

enum {
    EVT_NONE = 0, EVT_KBRD = 1, EVT_MOUSE = 2, EVT_BUTTON = 3,
    EVT_ESCAPE = 4, EVT_RESIZE = 5, EVT_OTHER = 6,
    EVENT_MESSAGE_FLUSH = 7
};
#define ESCAPE 0xE000u
#define EVENT_EMPTY {EVT_NONE, {0,0}, {0}}
typedef struct ui_event {
    int type;
    struct { unsigned int code; int mods; } key;
    struct { int button; } mouse;
} ui_event;
struct fake_term { int wid, hgt; };
static struct fake_term screen = {80, 24};
static struct fake_term *Term = &screen;
struct queued_event { ui_event value; int new_width, new_height; };
static struct queued_event queue[16];
static size_t queue_size, queue_index;
static int clear_count, draw_count, too_wide, flush_count;
static char last_text[512];

static int angdroid_zh_tw_enabled(void) { return 1; }
static const char *angdroid_game_ui_text(const char *s, int zh)
{
    (void)zh;
    return s;
}
static void event_signal(int type)
{
    if (type == EVENT_MESSAGE_FLUSH) ++flush_count;
}
static void prt(const char *s, int row, int col)
{
    (void)row; (void)col;
    if (!s || !s[0]) { ++clear_count; return; }
    ++draw_count;
    snprintf(last_text, sizeof(last_text), "%s", s);
    if (!Term || Term->wid < 1 ||
        angdroid_utf8_display_columns(s) > (size_t)(Term->wid - 1))
        ++too_wide;
}
static ui_event inkey_ex(void)
{
    ui_event escaped = {EVT_ESCAPE, {0,0}, {0}};
    struct queued_event item;
    if (queue_index >= queue_size) return escaped;
    item = queue[queue_index++];
    if (item.value.type == EVT_RESIZE) {
        screen.wid = item.new_width;
        screen.hgt = item.new_height;
    }
    return item.value;
}
static void reset(int width, int height)
{
    screen.wid = width;
    screen.hgt = height;
    queue_size = queue_index = 0;
    clear_count = draw_count = too_wide = flush_count = 0;
    last_text[0] = '\0';
}
static void push(int type, int code, int button, int w, int h)
{
    struct queued_event *item = &queue[queue_size++];
    item->value.type = type;
    item->value.key.code = (unsigned int)code;
    item->value.key.mods = 0;
    item->value.mouse.button = button;
    item->new_width = w;
    item->new_height = h;
}
"""

test = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } \
    ++checks; } while (0)

int main(void)
{
    ui_event answer = EVENT_EMPTY;

    reset(16, 24);
    push(EVT_KBRD, 'l', 0, 0, 0);
    CHECK(get_com_ex("學習專精或檢視能力（l/v/Esc）？", &answer));
    CHECK(answer.type == EVT_KBRD && answer.key.code == 'l');
    CHECK(draw_count == 1 && clear_count == 1 && too_wide == 0);
    CHECK(flush_count == 1);

    reset(80, 24);
    push(EVT_RESIZE, 0, 0, 22, 24);
    push(EVT_RESIZE, 0, 0, 11, 24);
    push(EVT_KBRD, 'v', 0, 0, 0);
    CHECK(get_com_ex("選擇學習專精或檢視能力（l/v/Esc）？", &answer));
    CHECK(answer.type == EVT_KBRD && answer.key.code == 'v');
    CHECK(draw_count == 3 && clear_count == 1 && too_wide == 0);
    CHECK(queue_index == 3);

    reset(70, 24);
    push(EVT_RESIZE, 0, 0, 2, 24);
    push(EVT_KBRD, 'l', 0, 0, 0);
    CHECK(!get_com_ex("已經無法顯示的中文指令", &answer));
    CHECK(answer.type == EVT_KBRD && answer.key.code == ESCAPE);
    CHECK(draw_count == 1 && clear_count >= 1);
    CHECK(queue_index == 1);

    reset(80, 24);
    push(EVT_ESCAPE, 0, 0, 0, 0);
    CHECK(!get_com_ex("Escape?", &answer));
    CHECK(answer.type == EVT_KBRD);
    CHECK(answer.key.code == ESCAPE && answer.key.mods == 0);
    CHECK(draw_count == 1 && clear_count == 1);

    reset(80, 24);
    push(EVT_BUTTON, 'v', 0, 0, 0);
    CHECK(get_com_ex("Android button?", &answer));
    CHECK(answer.type == EVT_KBRD && answer.key.code == 'v');

    reset(80, 24);
    push(EVT_MOUSE, 0, 2, 0, 0);
    CHECK(get_com_ex("Mouse? ", &answer));
    CHECK(answer.type == EVT_MOUSE);

    reset(80, 24);
    push(EVT_NONE, 0, 0, 0, 0);
    push(EVT_OTHER, 0, 0, 0, 0);
    push(EVT_KBRD, 'l', 0, 0, 0);
    CHECK(get_com_ex("Ignore other events? ", &answer));
    CHECK(draw_count == 3 && clear_count == 1 && too_wide == 0);

    reset(2, 24);
    CHECK(!get_com_ex("No readable width?", &answer));
    CHECK(answer.type == EVT_KBRD && answer.key.code == ESCAPE);
    CHECK(draw_count == 0 && clear_count >= 1 && queue_index == 0);

    reset(80, 24);
    CHECK(!get_com_ex("Null result?", NULL));
    CHECK(flush_count == 0 && queue_index == 0);

    printf("PASS: %lu FA command prompt UTF-8 and resize assertions\n",
        checks);
    return 0;
}
"""

if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_command_prompt_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + found.group(0) + "\n" + test, encoding="utf-8")
