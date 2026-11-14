#!/usr/bin/env python3
"""Strict host C99 tests for the production target floor-list input helpers.

Extracts the actual shortcut-to-index logic and resize-aware input loop
from ui-target.c; mocks only terminal geometry and input delivery.
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


index = extract("static int fa_target_floor_list_index("
    "\n    ui_event press, int floor_count, int width, int height)")
read = extract("static ui_event fa_target_floor_list_inkey("
    "\n    struct object **floor_list, int floor_count, const char *description)")
head = r"""
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum { EVT_NONE = 0, EVT_KBRD = 1, EVT_MOUSE = 2, EVT_BUTTON = 3,
       EVT_ESCAPE = 4, EVT_RESIZE = 5, EVT_OTHER = 6 };
#define EVENT_EMPTY {0}
#define ESCAPE 0xE000u
#define OLIST_DEATH 0x80
#define OLIST_WEIGHT 0x08
#define OLIST_GOLD 0x04
#define KEY_GRID_X(e) ((e).mouse.x)
#define KEY_GRID_Y(e) ((e).mouse.y)
typedef struct ui_event {
    int type;
    struct { uint32_t code; int mods; } key;
    struct { int button, x, y; } mouse;
} ui_event;
struct object { int unused; };
struct terminal { int wid, hgt; };
static struct terminal screen = {80, 24};
static struct terminal *Term = &screen;
static const char all_letters_nohjkl[] =
    "abcdefgimnopqrstuvwxyzABCDEFGIMNOPQRSTUVWXYZ";

struct queue_item {
    ui_event input;
    int new_width, new_height;
};
static struct queue_item events[32];
static size_t count, next;
static int rendered, described, last_floor_count;

static void show_floor(struct object **floor, int n, int mode,
                       void *unused)
{
    (void)floor; (void)mode; (void)unused;
    ++rendered;
    last_floor_count = n;
}
static void fa_target_draw_bounded_line(const char *description, int row)
{
    (void)description; (void)row;
    ++described;
}
static ui_event inkey_ex(void)
{
    ui_event escape = {EVT_ESCAPE, {0, 0}, {0, 0, 0}};
    struct queue_item item;
    if (next == count) return escape;
    item = events[next++];
    if (item.input.type == EVT_RESIZE) {
        screen.wid = item.new_width;
        screen.hgt = item.new_height;
    }
    return item.input;
}
static void reset(int width, int height)
{
    screen.wid = width; screen.hgt = height;
    count = next = 0;
    rendered = described = last_floor_count = 0;
}
static void push(int type, uint32_t code, int button,
                 int x, int y, int width, int height)
{
    struct queue_item *entry = &events[count++];
    memset(entry, 0, sizeof(*entry));
    entry->input.type = type;
    entry->input.key.code = code;
    entry->input.mouse.button = button;
    entry->input.mouse.x = x;
    entry->input.mouse.y = y;
    entry->new_width = width;
    entry->new_height = height;
}
"""
test = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    ui_event press = EVENT_EMPTY;
    struct object one = {0};
    struct object *floor[] = {&one, &one, &one};

    press.type = EVT_KBRD;
    press.key.code = 'm';
    CHECK(fa_target_floor_list_index(press, 20, 80, 24) == 8);
    press.key.code = 'A';
    CHECK(fa_target_floor_list_index(press, 30, 80, 24) == 22);
    press.key.code = 'h';
    CHECK(fa_target_floor_list_index(press, 20, 80, 24) == -1);
    press.key.code = 0;
    CHECK(fa_target_floor_list_index(press, 20, 80, 24) == -1);
    press.key.code = 0x161u;
    CHECK(fa_target_floor_list_index(press, 20, 80, 24) == -1);
    press.key.code = 'm';
    CHECK(fa_target_floor_list_index(press, 8, 80, 24) == -1);

    press.type = EVT_MOUSE;
    press.mouse.button = 1;
    press.mouse.x = 6;
    press.mouse.y = 3;
    CHECK(fa_target_floor_list_index(press, 10, 80, 24) == 2);
    press.mouse.y = 0;
    CHECK(fa_target_floor_list_index(press, 10, 80, 24) == -1);
    press.mouse.y = 24;
    CHECK(fa_target_floor_list_index(press, 30, 80, 24) == -1);
    press.mouse.y = 3;
    press.mouse.x = 79;
    CHECK(fa_target_floor_list_index(press, 10, 80, 24) == -1);
    press.mouse.x = 0;
    press.mouse.button = 2;
    CHECK(fa_target_floor_list_index(press, 10, 80, 24) == -1);
    press.mouse.button = 1;
    CHECK(fa_target_floor_list_index(press, 0, 80, 24) == -1);
    CHECK(fa_target_floor_list_index(press, 10, 1, 24) == -1);

    reset(80, 24);
    push(EVT_RESIZE, 0, 0, 0, 0, 40, 12);
    push(EVT_RESIZE, 0, 0, 0, 0, 18, 8);
    push(EVT_MOUSE, 0, 1, 3, 2, 0, 0);
    press = fa_target_floor_list_inkey(floor, 3, "物品堆");
    CHECK(press.type == EVT_MOUSE && next == 3);
    CHECK(rendered == 2 && described == 2 && last_floor_count == 3);
    CHECK(fa_target_floor_list_index(press, 3, screen.wid, screen.hgt) == 1);

    reset(80, 24);
    push(EVT_RESIZE, 0, 0, 0, 0, 2, 4);
    push(EVT_KBRD, 'a', 0, 0, 0, 0, 0);
    press = fa_target_floor_list_inkey(floor, 3, "Too small");
    CHECK(press.type == EVT_KBRD && press.key.code == ESCAPE);
    CHECK(next == 1 && rendered == 0 && described == 0);

    reset(80, 24);
    push(EVT_BUTTON, 'b', 0, 0, 0, 0, 0);
    press = fa_target_floor_list_inkey(floor, 3, "Button");
    CHECK(press.type == EVT_KBRD && press.key.code == 'b');

    reset(80, 24);
    push(EVT_ESCAPE, 0, 0, 0, 0, 0, 0);
    press = fa_target_floor_list_inkey(floor, 3, "Escape");
    CHECK(press.type == EVT_KBRD && press.key.code == ESCAPE);

    printf("PASS: %lu production floor list input and label checks\n",
           checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_floor_list_input_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    head + "\n" + index + "\n" + read + "\n" + test, encoding="utf-8")
