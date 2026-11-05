#!/usr/bin/env python3
"""Compile the real target_display_help() with host printf-security checking.

Extracts the actual C function from ui-target.c and compiles it with minimal
terminal stubs. This does not build the NDK library or verify the Android UI.
"""
from pathlib import Path
import os
import re
import shlex
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = CPP / "ui-target.c"

HARNESS = r'''
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "zh-tw-target-help.h"

#define HELP_HEIGHT 3
#define COLOUR_L_GREEN 1
#define CMD_IGNORE 1
#define KEYMAP_MODE_ROGUE 1
#define KEYMAP_MODE_ORIG 0
#define rogue_like_commands 0
#define player ((void *)0)
#define OPT(p, key) (false)
#define KTRL(k) ((unsigned char)((k) & 0x1f))
#define UN_KTRL(k) ((unsigned char)((k) | 0x40))

static int text_out_indent;
static void (*text_out_hook)(void);
static void text_out_to_screen(void) {}
static bool angdroid_zh_tw_enabled(void) { return true; }
static void Term_get_size(int *w, int *h) { *w = 80; *h = 24; }
static void clear_from(int row) { (void)row; }
static void Term_gotoxy(int col, int row) { (void)col; (void)row; }
static unsigned char cmd_lookup_key(int cmd, int mode)
{
    (void)cmd;
    (void)mode;
    return 'i';
}
static void text_out(const char *format, ...)
    __attribute__((format(printf, 1, 2)));
static void text_out(const char *format, ...) { (void)format; }
static void text_out_c(int color, const char *format, ...)
    __attribute__((format(printf, 2, 3)));
static void text_out_c(int color, const char *format, ...)
{
    (void)color;
    (void)format;
}
'''


def compile_function(function):
    compiler = shlex.split(os.environ.get("CC", "cc"))
    if not compiler:
        raise AssertionError("CC must specify an available compiler")
    with tempfile.TemporaryDirectory(prefix="target-help-format-") as td:
        path = Path(td) / "target_help_format.c"
        path.write_text(HARNESS + "\n" + function, encoding="utf-8")
        return subprocess.run(
            compiler + ["-std=c99", "-Wall", "-Wextra", "-Werror",
                        "-Wformat", "-Werror=format-security",
                        "-pedantic", "-fsyntax-only",
                        "-I", str(CPP), str(path)],
            capture_output=True, text=True, check=False,
        )


def main():
    source = SOURCE.read_text(encoding="utf-8")
    prefix = source.partition("void target_display_help(")
    if not prefix[1]:
        raise AssertionError("Missing target_display_help function")
    function = "void target_display_help(" + prefix[2].partition(
        "static bool is_running_keymap(")[0]
    if "static bool is_running_keymap(" not in prefix[2]:
        raise AssertionError("Missing target help function boundary")

    mapped = re.findall(r'\bangdroid_target_help_text\s*\(', function)
    safe = re.findall(
        r'\btext_out\s*\(\s*"%s"\s*,\s*'
        r'angdroid_target_help_text\s*\(\s*'
        r'"(?:\\.|[^"\\])*"\s*,\s*zh_tw\s*\)\s*\)\s*;',
        function)
    if len(mapped) != 18 or len(safe) != 18:
        raise AssertionError(
            f"All 18 target help strings must use literal %s (found "
            f"{len(mapped)} mapped, {len(safe)} safe)")
    if re.search(r'\btext_out\s*\(\s*angdroid_target_help_text\s*\(', function):
        raise AssertionError("Dynamic translated string passed as format")

    result = compile_function(function)
    if result.returncode:
        raise AssertionError("Target-help printf-security compile failed:\n"
                             + result.stderr)

    # Negative regression: the original bug must fail the same compiler.
    unsafe = function.replace(
        'text_out("%s", angdroid_target_help_text(',
        'text_out(angdroid_target_help_text(', 1)
    if unsafe == function:
        raise AssertionError("Cannot create regression negative control")
    result = compile_function(unsafe)
    if result.returncode == 0 or "not a string literal" not in result.stderr:
        raise AssertionError(
            "Compiler did not reject the original unsafe target-help call:\n"
            + result.stderr)

    print("PASS: 18 literal %s calls, actual target help C99 compile, "
          "unsafe-call negative regression")
    return 0


if __name__ == "__main__":
    sys.exit(main())
