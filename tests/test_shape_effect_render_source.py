#!/usr/bin/env python3
"""Host C99 runtime regression for the real shape-change effect renderer.

Extracts the actual source from effects-info.c and compiles it with small
stub records for effects, dice, textblocks, timed effects and projections.
This validates displayed formulas/fallback without requiring Android SDK.
"""
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = CPP / "effects-info.c"

HARNESS = r'''
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zh-tw-shape-effects.h"

#define strnfmt snprintf
enum { EF_CURE = 1, EF_DAMAGE, EF_TIMED_INC, EF_PROJECT_LOS, EF_FUTURE };
enum { POISONED, STUN, AFRAID, SHERO, ATT_VAMP, OTHER_TIMED };
enum { TURN_ALL, OTHER_PROJECTION };
typedef struct { int base, dice, sides, m_bonus; } random_value;
struct mock_dice { int base, dice, sides; };
struct effect {
    int index, subtype;
    const struct mock_dice *dice;
    const struct effect *next;
};
static const struct { const char *desc; } timed_effects[] = {
    { "poisoning" }, { "stunning" }, { "fear" }, { "berserk rage" },
    { "temporary hitpoint siphoning" }, { "unexpected state" }
};
static const struct { const char *desc; } projections[] = {
    { "causes monsters to flee" }, { "unexpected projection" }
};
typedef struct { char value[4096]; } textblock;

static textblock *textblock_new(void)
{
    return calloc(1, sizeof(textblock));
}
static void textblock_free(textblock *tb) { free(tb); }
static void textblock_append(textblock *tb, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
static void textblock_append(textblock *tb, const char *fmt, ...)
{
    size_t used = strlen(tb->value);
    va_list args;
    va_start(args, fmt);
    vsnprintf(tb->value + used, sizeof(tb->value) - used, fmt, args);
    va_end(args);
}
static void copy_to_textblock_with_coloring(textblock *tb, const char *s)
{
    textblock_append(tb, "%s", s);
}
static const char *effect_desc(const struct effect *effect)
{
    (void)effect;
    return "canonical original effect description";
}
static int dice_roll(const struct mock_dice *dice, random_value *value)
{
    value->base = dice->base;
    value->dice = dice->dice;
    value->sides = dice->sides;
    return value->base;
}
'''
CHECKS = r'''
static void verify(const struct effect *effect, const char *expected)
{
    textblock *tb = effect_describe_shape_zh(effect);
    if (!tb) {
        fprintf(stderr, "Expected zh-TW text: %s\n", expected);
        abort();
    }
    if (strcmp(tb->value, expected) != 0) {
        fprintf(stderr, "Expected: %s\nActual: %s\n", expected,
            tb->value);
        abort();
    }
    textblock_free(tb);
}
int main(void)
{
    const struct mock_dice bat_damage = { 5, 0, 0 };
    const struct mock_dice warg_damage = { 0, 1, 17 };
    const struct mock_dice vamp_damage = { 23, 0, 0 };
    const struct mock_dice berserk_turns = { 5, 1, 20 };
    const struct mock_dice vamp_turns = { 10, 1, 20 };

    struct effect cure_stun = { EF_CURE, STUN, NULL, NULL };
    struct effect cure_poison = { EF_CURE, POISONED, NULL, &cure_stun };
    verify(&cure_poison, "變身時，解除中毒，並解除暈眩");

    struct effect bat = { EF_DAMAGE, 0, &bat_damage, NULL };
    verify(&bat, "變身時，承受 5 點傷害");

    struct effect warg_berserk = {
        EF_TIMED_INC, SHERO, &berserk_turns, NULL
    };
    struct effect warg_fear = { EF_CURE, AFRAID, NULL, &warg_berserk };
    struct effect warg = { EF_DAMAGE, 0, &warg_damage, &warg_fear };
    verify(&warg, "變身時，承受 1d17 點傷害，並解除恐懼，並延長狂暴狀態 5+1d20 回合");

    struct effect vamp_timed = {
        EF_TIMED_INC, ATT_VAMP, &vamp_turns, NULL
    };
    struct effect vamp = { EF_DAMAGE, 0, &vamp_damage, &vamp_timed };
    verify(&vamp, "變身時，承受 23 點傷害，並延長暫時吸取生命狀態 10+1d20 回合");

    struct effect wolf = { EF_PROJECT_LOS, TURN_ALL, NULL, &warg_fear };
    verify(&wolf, "變身時，使視線內怪物逃跑，並解除恐懼，並延長狂暴狀態 5+1d20 回合");

    /* Level-dependent dice must use current runtime values, not cached
     * values from the header or knowledge menu. */
    {
        struct mock_dice dynamic = { 0, 1, 11 };
        struct effect form = { EF_DAMAGE, 0, &dynamic, NULL };
        verify(&form, "變身時，承受 1d11 點傷害");
        dynamic.sides = 27;
        verify(&form, "變身時，承受 1d27 點傷害");
    }

    /* Unexpected effects or source descriptions must not render a partial
     * Chinese summary; caller can then use effect_describe() in English. */
    {
        struct effect future = { EF_FUTURE, 0, NULL, NULL };
        struct effect partial = { EF_CURE, POISONED, NULL, &future };
        struct effect unknown_state = { EF_TIMED_INC, OTHER_TIMED,
            &berserk_turns, NULL };
        struct effect unknown_project = { EF_PROJECT_LOS, OTHER_PROJECTION,
            NULL, NULL };
        assert(effect_describe_shape_zh(NULL) == NULL);
        assert(effect_describe_shape_zh(&future) == NULL);
        assert(effect_describe_shape_zh(&partial) == NULL);
        assert(effect_describe_shape_zh(&unknown_state) == NULL);
        assert(effect_describe_shape_zh(&unknown_project) == NULL);
    }
    puts("PASS: source-extracted dynamic zh-TW shape effects, dice and fallback");
    return 0;
}
'''


def extract(text, start, end):
    if text.count(start) != 1:
        raise AssertionError(f"Expected unique source marker: {start}")
    tail = text.split(start, 1)[1]
    if end not in tail:
        raise AssertionError(f"Missing function boundary: {end}")
    return start + tail.split(end, 1)[0]


def main():
    source = SOURCE.read_text(encoding="utf-8")
    original_dice_formatter = extract(
        source,
        "static void format_dice_string(",
        "/**\n * Appends a message describing the magical device skill bonus",
    )
    shape_renderer = extract(
        source,
        "textblock *effect_describe_shape_zh(",
        "/**\n * Fills a buffer with a short description",
    )
    cc = shlex.split(os.environ.get("CC", "cc"))
    if not cc:
        raise AssertionError("CC must name a compiler")

    with tempfile.TemporaryDirectory(prefix="shape-effect-native-") as folder:
        root = Path(folder)
        testcase = root / "shape_effect_render.c"
        exe = root / "shape_effect_render"
        testcase.write_text(
            HARNESS + "\n" + original_dice_formatter + "\n"
            + shape_renderer + "\n" + CHECKS, encoding="utf-8")
        compile_cmd = cc + ["-std=c99", "-Wall", "-Wextra", "-Werror",
                            "-pedantic", "-Wformat", "-Werror=format-security",
                            "-I", str(CPP), str(testcase), "-o", str(exe)]
        built = subprocess.run(compile_cmd, text=True, capture_output=True,
                               check=False)
        if built.returncode:
            raise AssertionError("Shape renderer C99 compile failed:\n"
                                 + built.stdout + built.stderr)
        ran = subprocess.run([str(exe)], text=True, capture_output=True,
                             check=False)
        if ran.returncode:
            raise AssertionError("Shape renderer regression failed:\n"
                                 + ran.stdout + ran.stderr)
        print(ran.stdout.strip())
    return 0


if __name__ == "__main__":
    sys.exit(main())
