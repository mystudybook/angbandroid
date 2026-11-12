#!/usr/bin/env python3
"""Extract production FAangband textui_get_check with C99 terminal/input mocks."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
code = (ROOT / "app/src/main/cpp/faangband/ui-input.c").read_text(
    encoding="utf-8")
match = re.search(
    r"^static bool textui_get_check\(const char \*prompt\)\n\{.*?^\}",
    code, flags=re.MULTILINE | re.DOTALL)
if not match:
    raise SystemExit("Missing production textui_get_check()")

header = r"""
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/ui-confirm-prompt-utf8.h"

enum {
    EVT_NONE = 0, EVT_KBRD = 1, EVT_MOUSE = 2, EVT_BUTTON = 3,
    EVT_ESCAPE = 4, EVT_RESIZE = 5, EVT_OTHER = 6,
    EVENT_MESSAGE_FLUSH = 7
};
#define EVENT_EMPTY {EVT_NONE, {0}}
typedef struct ui_event {
    int type;
    struct { int code; } key;
} ui_event;
struct fake_term { int wid, hgt; };
static struct fake_term terminal = {80, 24};
static struct fake_term *Term = &terminal;
struct queued_event { ui_event event; int next_width; };
static struct queued_event queue[8];
static size_t queue_length, queue_index;
static int prompts, clears, hint_clears, hint_flashes, flushes, bad_width;
static char last_text[512], last_hints[128];

static void event_signal(int event)
{
    if (event == EVENT_MESSAGE_FLUSH) ++flushes;
}
static void soft_kbd_flash(const char *keys)
{
    ++hint_flashes;
    if (keys) strncat(last_hints, keys,
        sizeof(last_hints) - strlen(last_hints) - 1);
}
static void soft_kbd_replace(const char *keys)
{
    ++hint_clears;
    last_hints[0] = '\0';
    if (keys) strncat(last_hints, keys, sizeof(last_hints) - 1);
}
static void prt(const char *text, int row, int col)
{
    size_t width;
    (void)row;
    (void)col;
    if (!text || !text[0]) { ++clears; return; }
    ++prompts;
    snprintf(last_text, sizeof(last_text), "%s", text);
    width = angdroid_utf8_display_columns(text);
    if (!Term || width > (size_t)(Term->wid - 1)) ++bad_width;
}
static ui_event inkey_ex(void)
{
    ui_event escape = {EVT_ESCAPE, {0}};
    struct queued_event current;
    if (queue_index >= queue_length) return escape;
    current = queue[queue_index++];
    if (current.event.type == EVT_RESIZE)
        terminal.wid = current.next_width;
    return current.event;
}
static void reset(int width)
{
    terminal.wid = width;
    terminal.hgt = 24;
    queue_length = 0;
    queue_index = 0;
    prompts = clears = hint_clears = hint_flashes = flushes = bad_width = 0;
    last_hints[0] = last_text[0] = '\0';
}
static void push(int type, int code, int width)
{
    struct queued_event *item = &queue[queue_length++];
    item->event.type = type;
    item->event.key.code = code;
    item->next_width = width;
}
"""

tests = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    reset(16);
    push(EVT_KBRD, 'Y', 0);
    CHECK(textui_get_check("護甲精通，確定學習？"));
    CHECK(prompts == 1 && clears == 1 && bad_width == 0);
    CHECK(strstr(last_text, "[y/n] ") != NULL);
    CHECK(flushes == 1);

    reset(20);
    push(EVT_MOUSE, 0, 0);
    CHECK(!textui_get_check("Choose?"));
    CHECK(prompts == 1 && clears == 1);

    reset(25);
    push(EVT_KBRD, 'N', 0);
    CHECK(!textui_get_check("No?"));
    CHECK(clears == 1);

    reset(25);
    push(EVT_ESCAPE, 0, 0);
    CHECK(!textui_get_check("Escape?"));
    CHECK(clears == 1);

    reset(25);
    push(EVT_BUTTON, 'y', 0);
    CHECK(textui_get_check("Button?"));
    CHECK(clears == 1);

    reset(80);
    push(EVT_RESIZE, 0, 16);
    push(EVT_KBRD, 'y', 0);
    CHECK(textui_get_check("中文長說明甲乙丙丁戊己庚辛壬癸，確定學習？"));
    CHECK(prompts == 2 && clears == 1 && bad_width == 0);
    CHECK(hint_flashes == 2 && hint_clears == 1);
    CHECK(strcmp(last_hints, "${yes_no}") == 0);
    CHECK(strstr(last_text, "[y/n] ") != NULL);
    CHECK(queue_index == 2);

    reset(80);
    push(EVT_RESIZE, 0, 2);
    push(EVT_KBRD, 'Y', 0);
    CHECK(!textui_get_check("Too narrow?"));
    CHECK(prompts == 1 && bad_width == 0);
    CHECK(hint_clears == 2 && queue_index == 1);

    reset(25);
    push(EVT_NONE, 0, 0);
    push(EVT_OTHER, 0, 0);
    push(EVT_KBRD, 'Y', 0);
    CHECK(textui_get_check("Ignore unrelated inputs?"));
    CHECK(prompts == 3 && clears == 1 && bad_width == 0);

    reset(2);
    CHECK(!textui_get_check("No terminal width?"));
    CHECK(hint_clears == 1 && prompts == 0 && queue_index == 0);

    printf("PASS: %lu FA real native Yes/No resize and cancel checks\n",
           checks);
    return 0;
}
"""

if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_confirm_check_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + match.group(0) + "\n" + tests, encoding="utf-8")
