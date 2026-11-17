#!/usr/bin/env python3
"""Compile actual curses/droid.c soft-keyboard transitions with host C99 mocks."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "app/src/main/cpp/curses/droid.c").read_text(encoding="utf-8")
names = ("soft_kbd_flash", "soft_kbd_replace", "soft_kbd_clear",
         "soft_kbd_flush", "strdeldup")


def extract(name):
    found = re.search(rf"^void {name}\([^\n]*\)\n\{{.*?^\}}",
                      source, re.MULTILINE | re.DOTALL)
    if not found:
        raise SystemExit(f"Missing production function: {name}")
    return found.group(0)


header = r"""
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app/src/main/cpp/curses/soft-kbd-replace.h"
static char soft_kbd_buffer[256] = "";
static bool flashing_keys = true;
static char last_sent[256];
static int send_count;
void strdeldup(char *str);
static void my_strcat(char *dst, const char *src, size_t cap)
{
    size_t used = strlen(dst);
    if (src && used < cap) strncat(dst, src, cap - used - 1);
}
static int record_keys(const char *keys)
{
    if (!keys) return -1;
    strncpy(last_sent, keys, sizeof(last_sent) - 1);
    last_sent[sizeof(last_sent) - 1] = '\0';
    ++send_count;
    return 0;
}
#define Term_control_keys(s) record_keys(s)
"""

test = r"""
static int assertions;
#define CHECK(c) do { \
    if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); \
        return 1; } \
    ++assertions; \
} while (0)
int main(void)
{
    int previous;
    soft_kbd_flash("lv");
    CHECK(strcmp(soft_kbd_buffer, "lv") == 0);
    soft_kbd_replace("abc[]");
    CHECK(strcmp(soft_kbd_buffer, "abc[]") == 0);
    soft_kbd_flush();
    CHECK(strcmp(last_sent, "abc[]") == 0);

    /* A Yes/No confirmation appends its token, so remove old row tags. */
    previous = send_count;
    soft_kbd_replace("");
    CHECK(soft_kbd_buffer[0] == '\0');
    CHECK(strcmp(last_sent, "${clear}") == 0);
    CHECK(send_count == previous + 1);
    soft_kbd_flash("${yes_no}");
    CHECK(strcmp(soft_kbd_buffer, "${yes_no}") == 0);
    soft_kbd_flush();
    CHECK(strcmp(last_sent, "${yes_no}") == 0);

    /* Cancel returns to the picker without retaining Yes/No controls. */
    soft_kbd_replace("");
    CHECK(strcmp(last_sent, "${clear}") == 0);
    soft_kbd_replace("de[]");
    CHECK(strcmp(soft_kbd_buffer, "de[]") == 0);
    soft_kbd_flush();
    CHECK(strcmp(last_sent, "de[]") == 0);

    /* Reconfirmation and final exit both release their temporary hints. */
    soft_kbd_replace("");
    soft_kbd_flash("${yes_no}");
    CHECK(strcmp(soft_kbd_buffer, "${yes_no}") == 0);
    soft_kbd_clear(false);
    CHECK(soft_kbd_buffer[0] == '\0');
    CHECK(strcmp(last_sent, "${clear}") == 0);
    soft_kbd_replace("[]");
    soft_kbd_replace("");
    CHECK(soft_kbd_buffer[0] == '\0');
    CHECK(strcmp(last_sent, "${clear}") == 0);
    printf("PASS: %d production native ability hint lifecycle checks\n",
           assertions);
    return 0;
}
"""

if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_ability_hint_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    header + "\n" + "\n\n".join(extract(name) for name in names)
    + "\n\n" + test, encoding="utf-8")
