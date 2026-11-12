#!/usr/bin/env python3
"""Extract production FAangband modal pager and compile against C99 mocks.

The actual display_area(), pager, and key normalization bodies come from
ui-output.c. Mocks supply only the terminal, input queue and textblock data.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
src = (ROOT / "app/src/main/cpp/faangband/ui-output.c").read_text(
    encoding="utf-8")


def extract(signature):
    hit = re.search(
        rf"^{re.escape(signature)}\n\{{.*?^\}}",
        src, flags=re.MULTILINE | re.DOTALL)
    if not hit:
        raise SystemExit(f"Missing production function: {signature}")
    return hit.group(0)


actual = [
    extract("static void display_area(const wchar_t *text, const uint8_t *attrs,"
            "\n        size_t *line_starts, size_t *line_lengths,"
            "\n        size_t n_lines, region area, size_t line_from)"),
    extract("static struct keypress fa_textblock_key_from_event(ui_event event)"),
    extract("static void fa_textblock_print_bounded("
            "\n    const char *label, int row, int col, int columns)"),
    extract("struct keypress textui_textblock_show(textblock *tb, region orig_area, const char *header)"),
]

head = r"""
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "app/src/main/cpp/angband/menu-text-fit.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define COLOUR_L_BLUE 1
#define ESCAPE 0xE000u
#define ARROW_UP 0x83u
#define ARROW_DOWN 0x80u
enum {
    EVT_NONE = 0, EVT_KBRD = 1, EVT_MOUSE = 2, EVT_BUTTON = 3,
    EVT_ESCAPE = 4, EVT_RESIZE = 5, EVT_OTHER = 6
};
struct keypress { int type; uint32_t code; uint8_t mods; };
static const struct keypress KEYPRESS_NULL = {EVT_NONE, 0, 0};
typedef struct ui_event {
    int type;
    struct keypress key;
    struct { int button; } mouse;
} ui_event;
typedef struct region {
    int col, row, width, page_rows;
} region;
typedef struct textblock {
    wchar_t text[512];
    uint8_t attrs[512];
} textblock;

struct terminal { int wid, hgt; };
static struct terminal device = {80, 24};
static struct terminal *Term = &device;
struct event_record {
    ui_event event;
    int new_width, new_height;
};
static struct event_record queue[128];
static size_t queued, read_at;
static int bad_coordinates, erases, draws, layout_calls, saved, loaded;
static int footer_calls, lines_allocated, lines_freed;
static int lang_zh = 1;

static int angdroid_zh_tw_enabled(void) { return lang_zh; }
static void Term_get_size(int *w, int *h)
{
    *w = Term->wid;
    *h = Term->hgt;
}
static region region_calculate(region area)
{
    if (area.col < 0) area.col += Term->wid;
    if (area.row < 0) area.row += Term->hgt;
    if (area.width <= 0) area.width += Term->wid - area.col;
    if (area.page_rows <= 0) area.page_rows += Term->hgt - area.row;
    return area;
}
static void screen_save(void) { ++saved; }
static void screen_load(void) { ++loaded; }
static void mem_free(void *item)
{
    if (item) ++lines_freed;
    free(item);
}
static const wchar_t *textblock_text(textblock *tb)
{
    return tb ? tb->text : NULL;
}
static const uint8_t *textblock_attrs(textblock *tb)
{
    return tb ? tb->attrs : NULL;
}
static size_t textblock_calculate_lines(textblock *tb, size_t **starts,
        size_t **lengths, size_t width)
{
    size_t n, total, i;
    ++layout_calls;
    if (!tb || !tb->text[0] || width == 0) return 0;
    total = wcslen(tb->text);
    n = (total + width - 1) / width;
    *starts = calloc(n, sizeof(**starts));
    *lengths = calloc(n, sizeof(**lengths));
    if (!*starts || !*lengths) abort();
    lines_allocated += 2;
    for (i = 0; i < n; ++i) {
        (*starts)[i] = i * width;
        (*lengths)[i] = MIN(width, total - i * width);
    }
    return n;
}
static void Term_erase(int col, int row, int width)
{
    ++erases;
    if (!Term || col < 0 || row < 0 || width < 0 ||
        row >= Term->hgt || col + width > Term->wid)
        ++bad_coordinates;
}
static void Term_putch(int col, int row, int color, wchar_t ch)
{
    int cw = angdroid_terminal_columns((uint32_t)ch);
    (void)color;
    ++draws;
    if (!Term || col < 0 || row < 0 || row >= Term->hgt ||
        col + cw > Term->wid - 1)
        ++bad_coordinates;
}
static void c_put_str(int color, const char *s, int row, int col)
{
    (void)color;
    ++footer_calls;
    if (!Term || row < 0 || row >= Term->hgt || col < 0 ||
        col + (int)angdroid_utf8_display_columns(s) > Term->wid - 1)
        ++bad_coordinates;
}
static ui_event inkey_ex(void)
{
    ui_event empty = {EVT_NONE, {EVT_NONE, 0, 0}, {0}};
    struct event_record next;
    if (read_at >= queued) return (ui_event){EVT_ESCAPE,
        {EVT_ESCAPE, ESCAPE, 0}, {0}};
    next = queue[read_at++];
    if (next.event.type == EVT_RESIZE) {
        device.wid = next.new_width;
        device.hgt = next.new_height;
    }
    return next.event;
}
static void reset(int width, int height)
{
    device.wid = width;
    device.hgt = height;
    queued = read_at = 0;
    bad_coordinates = erases = draws = layout_calls = 0;
    saved = loaded = footer_calls = 0;
    lines_allocated = lines_freed = 0;
    lang_zh = 1;
}
static void push(int type, uint32_t key, int mouse, int w, int h)
{
    struct event_record *entry = &queue[queued++];
    memset(entry, 0, sizeof(*entry));
    entry->event.type = type;
    entry->event.key.type = type;
    entry->event.key.code = key;
    entry->event.mouse.button = mouse;
    entry->new_width = w;
    entry->new_height = h;
}
"""
test = r"""
static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    textblock tb = {{0}, {0}};
    region whole = {0, 0, 0, 0};
    struct keypress result;
    int i;

    for (i = 0; i < 280; ++i) {
        tb.text[i] = L'a';
        tb.attrs[i] = 1;
    }
    tb.text[280] = L'\0';

    reset(12, 4);
    push(EVT_KBRD, ARROW_DOWN, 0, 0, 0);
    push(EVT_KBRD, ' ', 0, 0, 0);
    push(EVT_RESIZE, 0, 0, 9, 3);
    push(EVT_KBRD, ARROW_UP, 0, 0, 0);
    push(EVT_KBRD, ESCAPE, 0, 0, 0);
    result = textui_textblock_show(&tb, whole, "超長文字說明");
    CHECK(result.code == ESCAPE && read_at == 5);
    CHECK(layout_calls >= 2 && bad_coordinates == 0);
    CHECK(saved == 1 && loaded == 1);
    CHECK(lines_allocated == lines_freed);

    reset(80, 1);
    push(EVT_KBRD, ' ', 0, 0, 0);
    push(EVT_KBRD, ESCAPE, 0, 0, 0);
    result = textui_textblock_show(&tb, whole, "Header");
    CHECK(result.code == ESCAPE && bad_coordinates == 0);
    CHECK(saved == 1 && loaded == 1);
    CHECK(footer_calls == 0);

    reset(1, 0);
    push(EVT_RESIZE, 0, 0, 20, 5);
    push(EVT_KBRD, 'x', 0, 0, 0);
    result = textui_textblock_show(&tb, whole, NULL);
    CHECK(result.code == 'x' && read_at == 2);
    CHECK(bad_coordinates == 0);
    CHECK(saved == 1 && loaded == 1);

    reset(20, 5);
    tb.text[0] = L'\0';
    push(EVT_KBRD, 'z', 0, 0, 0);
    result = textui_textblock_show(&tb, whole, "No content");
    CHECK(result.code == 'z' && bad_coordinates == 0);
    CHECK(lines_allocated == lines_freed);

    reset(9, 5);
    for (i = 0; i < 100; ++i) {
        tb.text[i] = L'甲';
        tb.attrs[i] = 1;
    }
    tb.text[100] = L'\0';
    for (i = 0; i < 60; ++i)
        push(EVT_KBRD, ARROW_DOWN, 0, 0, 0);
    push(EVT_KBRD, 'q', 0, 0, 0);
    result = textui_textblock_show(&tb, whole, NULL);
    CHECK(result.code == 'q' && read_at == 61);
    CHECK(bad_coordinates == 0 && footer_calls >= 1);
    CHECK(lines_allocated == lines_freed);
    CHECK(saved == 1 && loaded == 1);

    reset(20, 5);
    push(EVT_MOUSE, 0, 2, 0, 0);
    result = textui_textblock_show(&tb, whole, NULL);
    CHECK(result.code == ESCAPE && bad_coordinates == 0);
    CHECK(saved == 1 && loaded == 1);

    printf("PASS: %lu production textblock pager/rotation checks\n", checks);
    return 0;
}
"""
if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_textblock_pager_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    head + "\n" + "\n\n".join(actual) + "\n" + test, encoding="utf-8")
