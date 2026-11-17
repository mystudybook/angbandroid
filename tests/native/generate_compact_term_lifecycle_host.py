#!/usr/bin/env python3
"""Compile real term_init()/term_nuke() bodies against a tiny host C99 term.

This generator extracts the implementations from each engine's ui-term.c;
no duplicate lifecycle implementation is maintained in the C99 test. Mock
only the allocator, backing windows and the parts of term not used by those
functions. The generated test intentionally does not claim to build the APK.
"""
import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def extract_function(source: str, signature: str) -> str:
    matches = list(re.finditer(r"(?m)^" + re.escape(signature) + r"\s*$", source))
    if len(matches) != 1:
        raise ValueError(f"Expected one real definition of {signature}: {len(matches)}")
    start = matches[0].start()
    end = source.find("\n}\n", start)
    if end < 0:
        raise ValueError(f"Missing closing brace: {signature}")
    result = source[start:end + 2]
    if "\n{" not in result:
        raise ValueError(f"Unexpected opening brace: {signature}")
    return result


def generate(engine: str) -> str:
    source_file = ROOT / "app" / "src" / "main" / "cpp" / engine / "ui-term.c"
    source = source_file.read_text(encoding="utf-8")
    sequence = r"(?m)^static uint64_t angdroid_term_instance_sequence;$"
    found = re.findall(sequence, source)
    if len(found) != 1:
        raise ValueError("Missing unique terminal serial declaration")
    init = extract_function(source, "errr term_init(term *t, int w, int h, int k)")
    nuke = extract_function(source, "errr term_nuke(term *t)")
    for required in (
        "memset(t, 0, sizeof(term));",
        "t->instance_id = angdroid_term_instance_sequence;",
        "term_win_init(t->scr, w, h);",
    ):
        if required not in init:
            raise ValueError(f"Term initializer no longer meets harness: {required}")
    if "t->instance_id = 0;" not in nuke:
        raise ValueError("Term destruction no longer invalidates cache identity")
    if nuke.index("t->instance_id = 0;") > nuke.index("if (t->active_flag)"):
        raise ValueError("Term identity must be invalid before native nuke hooks")

    prelude = r'''/* Generated from the real ui-term.c initializer and destructor. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "app/src/main/cpp/angband/compact-player-viewport.h"

#define SIDEBAR_LEFT 1
typedef int errr;
typedef struct ui_event { int type; } ui_event;
typedef struct term_win { int width, height; int *cells; } term_win;
typedef struct term term;
struct term {
    bool active_flag, mapped_flag, total_erase;
    uint64_t instance_id;
    uint32_t content_generation;
    int wid, hgt, y1, y2, saved, sidebar_mode;
    uint16_t key_head, key_tail, key_size;
    ui_event *key_queue;
    int *x1, *x2;
    term_win *old, *scr, *mem, *tmp;
    void (*nuke_hook)(term *);
};

static unsigned long checks, alloc_count, free_count, nuke_hooks;
#define CHECK(x) do { assert(x); ++checks; } while (0)
static void *mem_zalloc(size_t n) {
    void *p = calloc(1, n ? n : 1);
    assert(p);
    ++alloc_count;
    return p;
}
static void mem_free(void *p) {
    if (p) { ++free_count; free(p); }
}
static void term_win_init(term_win *win, int width, int height) {
    assert(win && width > 0 && height > 0);
    win->width = width;
    win->height = height;
    win->cells = mem_zalloc((size_t)width * (size_t)height * sizeof(int));
}
static void term_win_nuke(term_win *win) {
    assert(win);
    mem_free(win->cells);
    win->cells = NULL;
}
static void on_nuke(term *t) {
    /* A teardown hook cannot access a terminal with a valid cache ID. */
    assert(t->instance_id == 0);
    ++nuke_hooks;
}
'''
    tests = r'''
static void test_reuse_same_address(void)
{
    term t;
    uint64_t original;
    struct angdroid_compact_player_redraw_state cache;
    CHECK(term_init(&t, 80, 24, 16) == 0);
    CHECK(t.instance_id != 0);
    CHECK(t.total_erase);
    CHECK(t.wid == 80 && t.hgt == 24);
    CHECK(t.x2[0] == 79 && t.x2[23] == 79);
    original = t.instance_id;
    cache.terminal = &t;
    cache.width = 80;
    cache.height = 24;
    cache.ready = 1;
    cache.content_generation = 0;
    cache.locale_zh_tw = 1;
    cache.instance_id = original;
    CHECK(angdroid_compact_player_cache_matches(
        &cache, &t, 80, 24, 1, 0, 1, original));

    t.active_flag = true;
    t.mapped_flag = true;
    t.nuke_hook = on_nuke;
    CHECK(term_nuke(&t) == 0);
    CHECK(nuke_hooks == 1);
    CHECK(t.instance_id == 0);
    CHECK(!angdroid_compact_player_cache_matches(
        &cache, &t, 80, 24, 1, 0, 1, t.instance_id));

    /* The same address, size, locale and content generation are all reused. */
    CHECK(term_init(&t, 80, 24, 16) == 0);
    CHECK(t.instance_id != original);
    CHECK(t.content_generation == 0);
    CHECK(!angdroid_compact_player_cache_matches(
        &cache, &t, 80, 24, 1, 0, 1, t.instance_id));
    cache.instance_id = t.instance_id;
    CHECK(angdroid_compact_player_cache_matches(
        &cache, &t, 80, 24, 1, 0, 1, t.instance_id));

    /* The real nuke() also frees saved and temporary windows. */
    t.mem = mem_zalloc(sizeof(*t.mem));
    t.tmp = mem_zalloc(sizeof(*t.tmp));
    term_win_init(t.mem, t.wid, t.hgt);
    term_win_init(t.tmp, t.wid, t.hgt);
    CHECK(term_nuke(&t) == 0);
    CHECK(t.instance_id == 0);
}

static void test_many_term_cycles(void)
{
    term main_term, other_term;
    uint64_t last_id = 0;
    unsigned i;
    for (i = 0; i < 128; ++i) {
        int width = 20 + (int)(i % 43);
        int height = 1 + (int)(i % 31);
        CHECK(term_init(&main_term, width, height, 8) == 0);
        CHECK(main_term.instance_id > last_id);
        last_id = main_term.instance_id;
        CHECK(main_term.wid == width && main_term.hgt == height);
        CHECK(main_term.x2[height - 1] == width - 1);
        CHECK(term_init(&other_term, 80, 24, 8) == 0);
        CHECK(other_term.instance_id > last_id);
        last_id = other_term.instance_id;
        CHECK(term_nuke(&other_term) == 0);
        CHECK(term_nuke(&main_term) == 0);
    }
    CHECK(alloc_count == free_count);
}

int main(void)
{
    test_reuse_same_address();
    test_many_term_cycles();
    CHECK(alloc_count == free_count);
    printf("PASS: real %s term_init / term_nuke C99 (%lu assertions)\n",
           ENGINE_NAME, checks);
    return 0;
}
'''
    return (
        prelude + '\n' + found[0] + '\n\n' + init + '\n\n' + nuke
        + '\n\n#define ENGINE_NAME "' + engine + '"\n' + tests
    )


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("engine", choices=("angband", "faangband"))
    ap.add_argument("output", type=Path)
    args = ap.parse_args()
    args.output.write_text(generate(args.engine), encoding="utf-8")


if __name__ == "__main__":
    main()
