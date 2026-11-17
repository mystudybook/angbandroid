#!/usr/bin/env python3
"""Compile real screen_save/load/load_all from both engines with host mocks."""
import argparse
import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXTRACTOR = ROOT / "tests/native/generate_compact_term_lifecycle_host.py"
spec = importlib.util.spec_from_file_location("screen_depth_extract", EXTRACTOR)
if not spec or not spec.loader:
    raise RuntimeError("Missing term function extractor")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)

SIGNATURES = (
    "void screen_reset_overlay_state(void)",
    "static bool screen_load_is_unbacked(void)",
    "void screen_save(void)",
    "void screen_load(void)",
    "void screen_load_all(void)",
    "bool textui_map_is_visible(void)",
)

PRELUDE = r'''/* Real ui-output.c screen wrappers; native term calls are mocked. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#define PR_MAP 0x40u
#define EVENT_MESSAGE_FLUSH 19

struct upkeep { unsigned redraw; };
struct actor { struct upkeep *upkeep; };
static struct upkeep upkeep;
static struct actor actor = { &upkeep };
static struct actor *player = &actor;

static unsigned physical_depth, replayed, saves_called, loads_called;
static unsigned loads_all_called, redrawn, flushes;
static bool fail_save_once, fail_load_once, fail_load_all_once;
static unsigned long checks;
#define CHECK(c) do { assert(c); ++checks; } while (0)

static void redraw_stuff(struct actor *p) {
    CHECK(p && p->upkeep);
    ++redrawn;
}
static void event_signal(int event) {
    CHECK(event == EVENT_MESSAGE_FLUSH);
    ++flushes;
}
static int Term_save(void) {
    ++saves_called;
    if (fail_save_once) { fail_save_once = false; return -1; }
    if (physical_depth == UINT8_MAX) return -1;
    ++physical_depth;
    return 0;
}
static int Term_load(void) {
    ++loads_called;
    if (fail_load_once) { fail_load_once = false; return -1; }
    if (!physical_depth) return -1;
    --physical_depth;
    return 0;
}
static int Term_load_all(void) {
    ++loads_all_called;
    if (fail_load_all_once) { fail_load_all_once = false; return -1; }
    if (!physical_depth) return -1;
    replayed += physical_depth;
    --physical_depth;
    return 0;
}
'''

TESTS = r'''
static void fresh(void) {
    CHECK(screen_save_depth == 0);
    CHECK(screen_save_unbacked_depth == 0);
    CHECK(screen_save_excess_depth == 0);
    CHECK(physical_depth == 0);
    CHECK(textui_map_is_visible());
}
static void test_empty_pops(void) {
    unsigned before = loads_called, before_all = loads_all_called;
    fresh();
    screen_load();
    screen_load_all();
    CHECK(loads_called == before && loads_all_called == before_all);
    fresh();
}
static void test_nested_and_replay(void) {
    unsigned before = replayed;
    screen_save();
    screen_save();
    screen_save();
    CHECK(screen_save_depth == 3 && physical_depth == 3);
    CHECK(!textui_map_is_visible());
    screen_load_all();
    CHECK(screen_save_depth == 2 && physical_depth == 2);
    CHECK(replayed == before + 3);
    screen_load();
    screen_load();
    fresh();
}
static void test_native_capacity(void) {
    unsigned i, before_loads = loads_called;
    for (i = 0; i < UINT8_MAX; ++i) screen_save();
    CHECK(screen_save_depth == UINT8_MAX && physical_depth == UINT8_MAX);
    CHECK(screen_save_unbacked_depth == 0);
    for (i = 0; i < 7; ++i) screen_save();
    CHECK(screen_save_depth == UINT8_MAX + 7);
    CHECK(screen_save_unbacked_depth == 7);
    CHECK(physical_depth == UINT8_MAX);
    screen_load_all(); /* Unsaved overlay: must not pop the prior snapshot. */
    CHECK(screen_save_unbacked_depth == 6);
    CHECK(screen_save_depth == UINT8_MAX + 6);
    CHECK(physical_depth == UINT8_MAX);
    CHECK(loads_called == before_loads);
    for (i = 0; i < 6; ++i) screen_load();
    CHECK(screen_save_unbacked_depth == 0);
    CHECK(screen_save_depth == UINT8_MAX && physical_depth == UINT8_MAX);
    screen_load_all();
    CHECK(screen_save_depth == UINT8_MAX - 1);
    CHECK(physical_depth == UINT8_MAX - 1);
    for (i = 0; i < UINT8_MAX - 1; ++i) screen_load();
    fresh();
}
static void test_failed_native_save(void) {
    unsigned before = saves_called;
    screen_save();
    fail_save_once = true;
    screen_save();
    screen_save(); /* Must not attempt a native save while nested failure lives. */
    CHECK(saves_called == before + 2);
    CHECK(physical_depth == 1 && screen_save_depth == 3);
    CHECK(screen_save_unbacked_depth == 2);
    screen_load();
    screen_load_all();
    CHECK(screen_save_depth == 1 && screen_save_unbacked_depth == 0);
    CHECK(physical_depth == 1);
    screen_load();
    fresh();
}
static void test_failed_native_restore(void) {
    screen_save();
    fail_load_once = true;
    screen_load();
    CHECK(screen_save_depth == 1 && physical_depth == 1);
    fail_load_all_once = true;
    screen_load_all();
    CHECK(screen_save_depth == 1 && physical_depth == 1);
    screen_load_all();
    fresh();
}
static void test_public_depth_saturation(void) {
    unsigned i;
    const unsigned total = (unsigned)INT16_MAX + 5u;
    for (i = 0; i < total; ++i) screen_save();
    CHECK(screen_save_depth == INT16_MAX);
    CHECK(screen_save_excess_depth == 5);
    CHECK(screen_save_unbacked_depth == total - UINT8_MAX);
    CHECK(physical_depth == UINT8_MAX);
    CHECK(!textui_map_is_visible());
    for (i = 0; i < 5; ++i) screen_load();
    CHECK(screen_save_depth == INT16_MAX && screen_save_excess_depth == 0);
    for (i = 0; i < total - 5 - UINT8_MAX; ++i) screen_load_all();
    CHECK(screen_save_depth == UINT8_MAX);
    CHECK(screen_save_unbacked_depth == 0 && physical_depth == UINT8_MAX);
    for (i = 0; i < UINT8_MAX; ++i) screen_load();
    fresh();
}

static void test_new_character_session(void) {
    unsigned i;
    unsigned before_restore = loads_called;
    fresh();

    /* Reproduce a stranded native stack and rejected overlays from the
     * previous character. textui_init() discards the physical saves first. */
    for (i = 0; i < UINT8_MAX + 5u; ++i) screen_save();
    CHECK(physical_depth == UINT8_MAX);
    CHECK(screen_save_unbacked_depth == 5);
    CHECK(screen_save_depth == UINT8_MAX + 5);

    physical_depth = 0; /* Term_discard_saved_screens(term_screen) */
    screen_reset_overlay_state();
    CHECK(physical_depth == 0);
    CHECK(screen_save_depth == 1); /* Intentional startup icky depth */
    CHECK(screen_save_unbacked_depth == 0);
    CHECK(screen_save_excess_depth == 0);
    CHECK(!textui_map_is_visible());

    screen_save(); /* New game's splash screen */
    CHECK(screen_save_depth == 2 && physical_depth == 1);
    screen_load();
    CHECK(physical_depth == 0 && screen_save_depth == 1);
    CHECK(loads_called == before_restore + 1);
    --screen_save_depth; /* ui_enter_world() opens the map */
    fresh();

    /* The previous character may also leave depth near INT16_MAX. */
    screen_save_depth = INT16_MAX;
    screen_save_excess_depth = 11;
    screen_save_unbacked_depth = 600;
    physical_depth = 0; /* Discard old native snapshots on replay */
    screen_reset_overlay_state();
    CHECK(screen_save_depth == 1);
    CHECK(screen_save_excess_depth == 0 && screen_save_unbacked_depth == 0);
    --screen_save_depth; /* Only initial icky-depth decrement */
    fresh();

    /* Repeated restart at the same terminal address is idempotent. */
    screen_reset_overlay_state();
    CHECK(screen_save_depth == 1);
    screen_reset_overlay_state();
    CHECK(screen_save_depth == 1);
    --screen_save_depth;
    fresh();
}

int main(void) {
    test_empty_pops();
    test_nested_and_replay();
    test_native_capacity();
    test_failed_native_save();
    test_failed_native_restore();
    test_public_depth_saturation();
    test_new_character_session();
    printf("PASS: %s real ui-output save/load wrappers (%lu checks)\n",
           ENGINE_NAME, checks);
    return 0;
}
'''

def generate(engine: str) -> str:
    native = (ROOT / "app/src/main/cpp" / engine / "ui-output.c").read_text(
        encoding="utf-8")
    if "int16_t screen_save_depth;" not in native:
        raise ValueError("Public legacy save depth changed")
    if "static uint64_t screen_save_unbacked_depth;" not in native:
        raise ValueError("Missing unbacked snapshot depth")
    if "static uint64_t screen_save_excess_depth;" not in native:
        raise ValueError("Missing overflowing logical screen depth")
    extracted = [module.extract_function(native, sig) for sig in SIGNATURES]
    return (
        PRELUDE
        + "\nint16_t screen_save_depth;\n"
        + "static uint64_t screen_save_unbacked_depth;\n"
        + "static uint64_t screen_save_excess_depth;\n\n"
        + "\n\n".join(extracted)
        + '\n#define ENGINE_NAME "' + engine + '"\n'
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
