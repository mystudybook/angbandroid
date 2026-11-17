#!/usr/bin/env python3
"""Host-compile the real terminal clear/save/load/load_all/redraw functions.

Pulls unchanged function bodies from each engine's actual ui-term.c. Only
window allocation, terminal hardware refresh and unrelated game dependencies
are mocked. No source checkout or Android NDK is necessary beyond host C99.
"""
import argparse
import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LIFECYCLE_GENERATOR = ROOT / "tests/native/generate_compact_term_lifecycle_host.py"
spec = importlib.util.spec_from_file_location("term_lifecycle_extract", LIFECYCLE_GENERATOR)
if not spec or not spec.loader:
    raise RuntimeError("Unable to load native lifecycle extractor")
lifecycle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(lifecycle)

SIGNATURES = (
    "static errr term_win_copy(term_win *s, term_win *f, int w, int h)",
    "errr term_init(term *t, int w, int h, int k)",
    "errr term_nuke(term *t)",
    "errr Term_resize(int w, int h)",
    "errr Term_clear(void)",
    "errr Term_save(void)",
    "errr Term_load(void)",
    "errr Term_load_all(void)",
    "errr Term_redraw(void)",
)

PRELUDE = r'''/* Generated real ui-term.c screen / term functions, minimal host mocks. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <limits.h>
#include "app/src/main/cpp/angband/compact-player-viewport.h"

#define SIDEBAR_LEFT 1
#define COLOUR_WHITE 1
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define EVT_RESIZE 1
#define EVENT_EMPTY {0}
typedef int errr;
typedef struct ui_event { int type; } ui_event;
typedef struct term_win term_win;
typedef struct term term;
struct term_win {
    int width, height;
    bool cu, cv;
    int cx, cy, cnx, cny;
    int **a, **ta;
    wchar_t **c, **tc;
    term_win *next;
};
struct term {
    bool active_flag, mapped_flag, total_erase, fixed_shape;
    uint64_t instance_id;
    uint32_t content_generation;
    int wid, hgt, y1, y2, sidebar_mode;
    uint8_t saved;
    uint16_t key_head, key_tail, key_size;
    ui_event *key_queue;
    int *x1, *x2;
    term_win *old, *scr, *mem, *tmp;
    void (*nuke_hook)(term *);
};
static term *Term;
static unsigned long checks, allocations, releases, fresh_count, resize_events;
#define CHECK(x) do { assert(x); ++checks; } while (0)
static void *mem_alloc(size_t n) {
    void *p = malloc(n ? n : 1);
    assert(p);
    ++allocations;
    return p;
}
static void *mem_zalloc(size_t n) {
    void *p = calloc(1, n ? n : 1);
    assert(p);
    ++allocations;
    return p;
}
static void mem_free(void *p) {
    if (p) { ++releases; free(p); }
}
static void term_win_init(term_win *win, int w, int h) {
    int y;
    assert(win && w > 0 && h > 0);
    win->width = w;
    win->height = h;
    win->a = mem_zalloc((size_t)h * sizeof(*win->a));
    win->c = mem_zalloc((size_t)h * sizeof(*win->c));
    win->ta = mem_zalloc((size_t)h * sizeof(*win->ta));
    win->tc = mem_zalloc((size_t)h * sizeof(*win->tc));
    for (y = 0; y < h; ++y) {
        win->a[y] = mem_zalloc((size_t)w * sizeof(**win->a));
        win->c[y] = mem_zalloc((size_t)w * sizeof(**win->c));
        win->ta[y] = mem_zalloc((size_t)w * sizeof(**win->ta));
        win->tc[y] = mem_zalloc((size_t)w * sizeof(**win->tc));
    }
}
static void term_win_nuke(term_win *win) {
    int y;
    /* Each snapshot retains its own dimensions through a resize. */
    assert(win && Term);
    for (y = 0; y < win->height; ++y) {
        mem_free(win->a[y]); mem_free(win->c[y]);
        mem_free(win->ta[y]); mem_free(win->tc[y]);
    }
    mem_free(win->a); mem_free(win->c);
    mem_free(win->ta); mem_free(win->tc);
    win->a = NULL; win->c = NULL; win->ta = NULL; win->tc = NULL;
}
static errr Term_fresh(void) {
    ++fresh_count;
    return 0;
}
static errr Term_event_push(const ui_event *event) {
    CHECK(event && event->type == EVT_RESIZE);
    ++resize_events;
    return 0;
}
static void on_nuke(term *t) {
    assert(t->instance_id == 0);
}
'''

TESTS = r'''
static void set_screen(wchar_t ch, int attr) {
    int x, y;
    for (y = 0; y < Term->hgt; ++y)
        for (x = 0; x < Term->wid; ++x) {
            Term->scr->c[y][x] = ch;
            Term->scr->a[y][x] = attr;
            Term->scr->tc[y][x] = ch;
            Term->scr->ta[y][x] = attr + 1;
        }
}
static void expect_screen(wchar_t ch, int attr) {
    int x, y;
    for (y = 0; y < Term->hgt; ++y)
        for (x = 0; x < Term->wid; ++x) {
            CHECK(Term->scr->c[y][x] == ch);
            CHECK(Term->scr->a[y][x] == attr);
            CHECK(Term->scr->tc[y][x] == ch);
            CHECK(Term->scr->ta[y][x] == attr + 1);
        }
}
static void expect_blank_screen(void) {
    int x, y;
    for (y = 0; y < Term->hgt; ++y)
        for (x = 0; x < Term->wid; ++x) {
            CHECK(Term->scr->c[y][x] == L' ');
            CHECK(Term->scr->a[y][x] == COLOUR_WHITE);
            CHECK(Term->scr->tc[y][x] == 0);
            CHECK(Term->scr->ta[y][x] == 0);
        }
}

static void verify_no_saved_load(term *t) {
    int dirty_before = Term->y1;
    uint32_t version_before = Term->content_generation;
    uint8_t saved_before = Term->saved;
    CHECK(!Term->mem);
    CHECK(Term_load() == -1);
    CHECK(Term->content_generation == version_before);
    CHECK(Term->saved == saved_before);
    CHECK(Term->y1 == dirty_before);
    CHECK(Term_load_all() == -1);
    CHECK(Term->content_generation == version_before);
    CHECK(Term->saved == saved_before);
    CHECK(Term->y1 == dirty_before);
    CHECK(!Term->mem);
    (void)t;
}

static void single_snapshot(term *t) {
    uint32_t version;
    uint64_t instance = t->instance_id;
    unsigned long fresh_before = fresh_count;
    Term = t;
    set_screen(L'甲', 3);
    version = Term->content_generation;
    CHECK(Term_save() == 0);
    CHECK(Term->saved == 1);
    CHECK(Term->content_generation == version); /* Saving is non-destructive */
    CHECK(Term_clear() == 0);
    CHECK(Term->content_generation == version + 1);
    CHECK(Term->total_erase);
    CHECK(Term->y1 == 0 && Term->y2 == Term->hgt - 1);
    expect_blank_screen();
    CHECK(Term_load() == 0);
    CHECK(Term->content_generation == version + 2);
    CHECK(Term->saved == 0);
    CHECK(Term->mem == NULL);
    expect_screen(L'甲', 3);
    CHECK(Term->instance_id == instance);
    CHECK(Term_redraw() == 0);
    CHECK(fresh_count == fresh_before + 1);
    CHECK(Term->content_generation == version + 2);
    verify_no_saved_load(t);
}

static void nested_snapshot(term *t) {
    uint32_t version;
    unsigned long fresh_before;
    Term = t;
    set_screen(L'甲', 1);
    CHECK(Term_save() == 0); /* Oldest */
    set_screen(L'乙', 2);
    CHECK(Term_save() == 0); /* Newest */
    set_screen(L'丙', 3);
    version = Term->content_generation;
    fresh_before = fresh_count;
    CHECK(Term->saved == 2);
    CHECK(Term_load_all() == 0);
    CHECK(Term->content_generation == version + 2);
    CHECK(fresh_count == fresh_before + 2);
    CHECK(Term->saved == 1);
    CHECK(Term->mem != NULL);
    expect_screen(L'乙', 2);
    CHECK(Term_load() == 0); /* Restore original older snapshot */
    CHECK(Term->saved == 0);
    expect_screen(L'甲', 1);
    verify_no_saved_load(t);
}


static void resize_nested_snapshot(term *t) {
    int x, y, old_w = t->wid, old_h = t->hgt;
    uint32_t before = Term->content_generation;
    unsigned long events = resize_events;
    Term = t;
    set_screen(L'甲', 1);
    CHECK(Term_save() == 0);
    set_screen(L'乙', 2);
    CHECK(Term_save() == 0);
    set_screen(L'丙', 3);
    CHECK(Term->saved == 2);
    CHECK(Term_resize(old_w, old_h) == 1);
    CHECK(Term_resize(0, old_h) == -1);
    t->fixed_shape = true;
    CHECK(Term_resize(old_w + 2, old_h + 1) == -1);
    t->fixed_shape = false;
    CHECK(Term->content_generation == before);
    CHECK(resize_events == events);
    CHECK(Term_resize(old_w + 2, old_h + 1) == 0);
    CHECK(Term->content_generation == before + 1);
    CHECK(resize_events == events + 1);
    CHECK(Term->saved == 2);
    CHECK(Term->mem && Term->mem->next && !Term->mem->next->next);
    for (y = 0; y < Term->hgt; ++y) {
        for (x = 0; x < Term->wid; ++x) {
            bool inside = x < old_w && y < old_h;
            CHECK(Term->scr->c[y][x] == (inside ? L'丙' : 0));
            CHECK(Term->mem->c[y][x] == (inside ? L'乙' : 0));
            CHECK(Term->mem->next->c[y][x] == (inside ? L'甲' : 0));
            CHECK(Term->scr->a[y][x] == (inside ? 3 : 0));
            CHECK(Term->mem->a[y][x] == (inside ? 2 : 0));
            CHECK(Term->mem->next->a[y][x] == (inside ? 1 : 0));
        }
    }
    CHECK(Term_resize(old_w, old_h) == 0);
    CHECK(Term->content_generation == before + 2);
    CHECK(resize_events == events + 2);
    CHECK(Term->saved == 2);
    expect_screen(L'丙', 3);
    CHECK(Term_load_all() == 0);
    CHECK(Term->saved == 1);
    expect_screen(L'乙', 2);
    CHECK(Term_load() == 0);
    CHECK(Term->saved == 0 && Term->mem == NULL);
    expect_screen(L'甲', 1);
    verify_no_saved_load(t);
}

static void snapshot_depth_limit(void) {
    term t;
    unsigned i;
    term_win *top;
    size_t allocated = allocations, freed = releases;
    Term = &t;
    CHECK(term_init(&t, 2, 1, 8) == 0);
    for (i = 0; i < UINT8_MAX; ++i) {
        CHECK(Term_save() == 0);
        CHECK(Term->saved == i + 1);
    }
    top = Term->mem;
    CHECK(Term_save() == -1);
    CHECK(Term->saved == UINT8_MAX && Term->mem == top);
    CHECK(Term_resize(3, 2) == 0);
    CHECK(Term->saved == UINT8_MAX);
    CHECK(Term_load() == 0);
    CHECK(Term->saved == UINT8_MAX - 1 && Term->mem != NULL);
    /* Destructor must free all remaining saved frames. */
    CHECK(term_nuke(&t) == 0);
    CHECK(t.saved == 0 && t.mem == NULL);
    CHECK(allocations - allocated == releases - freed);
}

static void run_screen(int width, int height) {
    term t;
    size_t prev_alloc = allocations, prev_release = releases;
    uint64_t previous_id = 0;
    unsigned pass;
    for (pass = 0; pass < 3; ++pass) {
        Term = &t;
        CHECK(term_init(&t, width, height, 8) == 0);
        CHECK(t.instance_id != 0);
        if (previous_id) CHECK(t.instance_id != previous_id);
        previous_id = t.instance_id;
        Term = &t;
        CHECK(t.content_generation == 0);
        verify_no_saved_load(&t);
        single_snapshot(&t);
        nested_snapshot(&t);
        resize_nested_snapshot(&t);
        t.nuke_hook = on_nuke;
        t.active_flag = true;
        CHECK(term_nuke(&t) == 0);
        CHECK(t.instance_id == 0);
        CHECK(allocations - prev_alloc == releases - prev_release);
    }
}

int main(void) {
    run_screen(1, 1);
    run_screen(5, 3);
    run_screen(12, 2);
    run_screen(80, 24);
    snapshot_depth_limit();
    printf("PASS: real %s Term_clear/save/load/load_all (%lu assertions)\n",
           ENGINE_NAME, checks);
    return 0;
}
'''


def generate(engine: str) -> str:
    src = (ROOT / "app/src/main/cpp" / engine / "ui-term.c").read_text(
        encoding="utf-8")
    extracts = [lifecycle.extract_function(src, signature) for signature in SIGNATURES]
    for sig, body in zip(SIGNATURES, extracts):
        if sig == "errr Term_save(void)" and "if (Term->saved == UINT8_MAX) return (-1);" not in body:
            raise ValueError("Snapshot stack must enforce its uint8_t depth limit")
        if sig == "errr term_nuke(term *t)" and "while (t->mem)" not in body:
            raise ValueError("Term destruction must release every saved frame")
        if sig in ("errr Term_load(void)", "errr Term_load_all(void)"):
            if "if (!Term->mem) return (-1);" not in body:
                raise ValueError(f"{sig} must guard absent saved screens")
            if body.index("if (!Term->mem) return (-1);") > body.index(
                    "Term->saved--;"):
                raise ValueError(f"{sig} changes saved counter before checking")
    return (
        PRELUDE + "\nstatic uint64_t angdroid_term_instance_sequence;\n\n"
        + "\n\n".join(extracts) + "\n#define ENGINE_NAME \"" + engine + "\"\n"
        + TESTS
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("engine", choices=("angband", "faangband"))
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.write_text(generate(args.engine), encoding="utf-8")


if __name__ == "__main__":
    main()
