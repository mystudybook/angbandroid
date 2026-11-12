#!/usr/bin/env python3
"""Extract production FAangband get_char() into a host-buildable C99 fixture.

Only event queue, terminal and Android soft-keyboard delivery are mocked.
No changes to game-state or Android runtime are required.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
code = (ROOT / "app/src/main/cpp/faangband/ui-input.c").read_text(
    encoding="utf-8")
match = re.search(
    r"^char get_char\(const char \*prompt, const char \*options, "
    r"size_t len, char fallback\)\n\{.*?^\}",
    code, flags=re.MULTILINE | re.DOTALL)
if not match:
    raise SystemExit("Missing production get_char()")

header = r"""
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/ui-confirm-prompt-utf8.h"

typedef uint32_t keycode_t;
enum {
    EVT_NONE = 0, EVT_KBRD = 1, EVT_BUTTON = 2, EVT_MOUSE = 3,
    EVT_ESCAPE = 4, EVT_RESIZE = 5, EVT_OTHER = 6,
    EVENT_MESSAGE_FLUSH = 7
};
#define ESCAPE 0xE000u
#define EVENT_EMPTY {EVT_NONE, {0}, {0}}
typedef struct ui_event {
    int type;
    struct { keycode_t code; } key;
    struct { int button; } mouse;
} ui_event;
struct fake_term { int wid, hgt; };
static struct fake_term screen = {80, 24};
static struct fake_term *Term = &screen;
struct queued_event { ui_event event; int next_width; };
static struct queued_event queue[16];
static size_t total, index_at;
static int prompts, clears, hint_clears, hint_flashes, bad_width;
static char last_text[512], last_hints[512];

static void event_signal(int unused) { (void)unused; }
static void soft_kbd_flash(const char *s)
{
    ++hint_flashes;
    if (s) strncat(last_hints, s,
        sizeof(last_hints) - strlen(last_hints) - 1);
}
static void soft_kbd_replace(const char *s)
{
    ++hint_clears;
    last_hints[0] = '\0';
    if (s) strncat(last_hints, s, sizeof(last_hints) - 1);
}
static void prt(const char *s, int row, int col)
{
    (void)row; (void)col;
    if (!s || !s[0]) { ++clears; return; }
    ++prompts;
    snprintf(last_text, sizeof(last_text), "%s", s);
    if (!Term || Term->wid <= 1 ||
        angdroid_utf8_display_columns(s) > (size_t)(Term->wid - 1))
        ++bad_width;
}
static ui_event inkey_ex(void)
{
    ui_event esc = {EVT_ESCAPE, {0}, {0}};
    struct queued_event item;
    if (index_at >= total) return esc;
    item = queue[index_at++];
    if (item.event.type == EVT_RESIZE) screen.wid = item.next_width;
    return item.event;
}
static void reset(int width)
{
    screen.wid = width;
    screen.hgt = 24;
    total = index_at = 0;
    prompts = clears = hint_clears = hint_flashes = bad_width = 0;
    last_text[0] = last_hints[0] = '\0';
}
static void push(int event, keycode_t code, int button, int width)
{
    struct queued_event *item = &queue[total++];
    item->event.type = event;
    item->event.key.code = code;
    item->event.mouse.button = button;
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
    reset(18);
    push(EVT_KBRD, 'A', 0, 0);
    CHECK(get_char("漢字能力請選擇甲乙丙丁戊己庚辛壬癸",
        "abc", 3, 'c') == 'a');
    CHECK(prompts == 1 && clears == 1 && bad_width == 0);
    CHECK(strstr(last_text, "[abc] ") != NULL);
    CHECK(last_hints[0] == '\0');

    reset(30);
    push(EVT_KBRD, 'b', 0, 0);
    CHECK(get_char("Limited", "abc", 1, 'z') == 'z');
    CHECK(clears == 1);

    reset(30);
    push(EVT_KBRD, 0, 0, 0);
    CHECK(get_char("NUL code", "abc", 3, 'z') == 'z');
    CHECK(prompts == 1 && clears == 1);

    reset(30);
    push(EVT_KBRD, 0x161u, 0, 0);
    CHECK(get_char("Truncated extended Unicode", "a", 1, 'z') == 'z');

    reset(30);
    push(EVT_KBRD, ESCAPE, 0, 0);
    CHECK(get_char("Special Escape", "abc", 3, 'z') == 'z');

    reset(30);
    push(EVT_BUTTON, 'c', 0, 0);
    CHECK(get_char("Android button", "abc", 3, 'z') == 'c');

    reset(30);
    push(EVT_MOUSE, 0, 1, 0);
    CHECK(get_char("Primary tap as Return", "\n", 1, 'z') == '\n');
    reset(30);
    push(EVT_MOUSE, 0, 2, 0);
    CHECK(get_char("Secondary tap cancels", "abc", 3, 'z') == 'z');

    reset(80);
    push(EVT_RESIZE, 0, 0, 10);
    push(EVT_KBRD, 'b', 0, 0);
    CHECK(get_char("中文長能力選單重新排版", "abc", 3, 'z') == 'b');
    CHECK(prompts == 2 && clears == 1 && bad_width == 0);
    CHECK(hint_flashes == 1 && hint_clears == 2);
    CHECK(strstr(last_text, "[abc] ") != NULL);
    CHECK(index_at == 2);

    reset(80);
    push(EVT_RESIZE, 0, 0, 1);
    push(EVT_KBRD, 'a', 0, 0);
    CHECK(get_char("Too narrow", "abc", 3, 'z') == 'z');
    CHECK(prompts == 1 && bad_width == 0 && index_at == 1);

    reset(7);
    push(EVT_KBRD, 'b', 0, 0);
    CHECK(get_char("A very long question", "abcdef", 6, 'z') == 'b');
    CHECK(strcmp(last_text, "[a+]") == 0);
    CHECK(bad_width == 0);

    reset(25);
    push(EVT_NONE, 0, 0, 0);
    push(EVT_OTHER, 0, 0, 0);
    push(EVT_KBRD, 'a', 0, 0);
    CHECK(get_char("Ignore other events", "abc", 3, 'z') == 'a');
    CHECK(prompts == 3 && clears == 1);
    CHECK(hint_clears == 3 && bad_width == 0);

    reset(2);
    CHECK(get_char("No room", "abc", 3, 'z') == 'z');
    CHECK(index_at == 0 && prompts == 0 && hint_clears == 1);

    reset(30);
    CHECK(get_char("Empty choices", "", 0, 'z') == 'z');
    CHECK(get_char("Null options", NULL, 1, 'z') == 'z');
    CHECK(prompts == 0 && index_at == 0);

    printf("PASS: %lu FA production get_char C99 event and resize checks\n",
        checks);
    return 0;
}
"""

if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_choice_input_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + match.group(0) + "\n" + tests, encoding="utf-8")
