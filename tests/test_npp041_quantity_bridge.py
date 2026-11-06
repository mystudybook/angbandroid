#!/usr/bin/env python3
"""NPP 0.4.1 native quantity lifecycle, shared Android bridge and zh-TW guards.

Host C smoke test compiles the actual control callback and the unchanged droid.c
logic with minimal host stubs. This is not a complete Android/NDK build.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
MAIN = (CPP / "npp041/main-android.c").read_text(encoding="utf-8")
NATIVE = (CPP / "npp041/util.c").read_text(encoding="utf-8")
CMAKE = (CPP / "npp041/CMakeLists.txt").read_text(encoding="utf-8")
BRIDGE = (CPP / "curses/droid.c").read_text(encoding="utf-8")
HEADER = (CPP / "curses/droid.h").read_text(encoding="utf-8")
CLASSIFIER = (ROOT / "app/src/main/java/org/rephial/xyangband/QuantityPromptText.java").read_text(encoding="utf-8")
POPUP = (ROOT / "app/src/main/java/org/rephial/xyangband/QuantityPopup.java").read_text(encoding="utf-8")
ACTIVITY = (ROOT / "app/src/main/java/org/rephial/xyangband/GameActivity.java").read_text(encoding="utf-8")


def quantity_body():
    return NATIVE.split("s16b get_quantity(cptr prompt, int max)", 1)[1].split(
        "\n/*\n * Hack - duplication", 1
    )[0]


def extract_function(source, prefix):
    """Extract one C function body, including its braces, from checked source."""
    start = source.index(prefix)
    opening = source.index("{", start)
    depth = 0
    for i in range(opening, len(source)):
        if source[i] == "{":
            depth += 1
        elif source[i] == "}":
            depth -= 1
            if depth == 0:
                return source[start:i + 1]
    raise AssertionError("Unclosed C function: " + prefix)


class Npp041QuantityBridgeTest(unittest.TestCase):
    def test_unique_shared_bridge_and_callback_ownership(self):
        self.assertEqual(1, CMAKE.count("../curses/droid.c"))
        self.assertIn('#include "droid.h"', MAIN)
        self.assertIn('#include "droid.h"', NATIVE)
        self.assertEqual(1, MAIN.count("int Term_control(int what, const char *msg)"))
        self.assertNotIn("Term_control_android", MAIN)
        self.assertIn("control_msg(what, pbuf)", MAIN)
        self.assertIn("extern int Term_control_quantity(", HEADER)
        self.assertIn("#define TERM_CONTROL_QUANTITY 7", HEADER)
        self.assertIn("int Term_control_quantity(", BRIDGE)
        self.assertIn("Term_control(TERM_CONTROL_QUANTITY, buffer);", BRIDGE)
        self.assertNotIn("int Term_control(int what", BRIDGE)

    def test_start_end_bounded_to_interactive_quantity(self):
        body = quantity_body()
        pre = body.split("Term_control_quantity(prompt, max, amt);", 1)[0]
        self.assertIn("if (p_ptr->command_arg)", pre)
        self.assertIn("repeat_pull(&amt)", pre)
        self.assertIn("allow_quantity", pre)
        self.assertIn("else if ((max != 1) && allow_quantity)", pre)
        self.assertEqual(1, body.count("Term_control_quantity(prompt, max, amt);"))
        self.assertEqual(2, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertIn("if (!get_string(prompt, buf, 7)) {", body)
        cancel = body.split("if (!get_string(prompt, buf, 7)) {", 1)[1].split(
            "return (0);", 1
        )[0]
        self.assertIn("soft_kbd_clear(true);", cancel)
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', cancel)
        success = body.split("return (0);", 1)[1]
        self.assertLess(success.index('Term_control(TERM_CONTROL_QUANTITY, "end");'),
                        success.index("amt = atoi(buf);"))
        self.assertIn("if (isalpha((unsigned char)buf[0])) amt = max;", success)
        self.assertIn("if (amt > max) amt = max;", success)
        self.assertIn("if (amt < 0) amt = 0;", success)
        self.assertIn("if (amt) repeat_push(amt);", success)

    def test_fallback_keyboard_and_exact_variant_translations(self):
        tag = chr(36) + "{quant}"
        self.assertIn('soft_kbd_linger("' + tag + '");', quantity_body())
        self.assertIn("if (!inkey_scan) soft_kbd_flush();", NATIVE)
        self.assertIn("soft_kbd_clear(false);", NATIVE)
        self.assertIn('keys = keys.replace("' + tag + '", "0123456789");', ACTIVITY)
        self.assertIn("FROM_ZERO_NPP041", CLASSIFIER)
        self.assertIn("return isNpp041 ? FROM_ZERO_NPP041 : FROM_ZERO_SILQ;", CLASSIFIER)
        self.assertIn("state.currentPlugin == Plugins.Plugin.npp041", POPUP)
        self.assertIn("case QuantityPromptText.FROM_ZERO_NPP041:", POPUP)
        self.assertIn("R.string.quantity_prompt_npp041", POPUP)
        self.assertIn('sprintf(tmp, "Quantity (0-%d): ", max);', NATIVE)
        for locale in ("values", "values-zh-rTW"):
            entries = {e.attrib["name"]: e.text or "" for e in
                       ET.parse(ROOT / "app/src/main/res" / locale / "strings.xml")
                       .getroot().findall("string")}
            self.assertEqual(1, entries["quantity_prompt_npp041"].count("%1$d"))
            if locale == "values-zh-rTW":
                self.assertIn("英文字母代表全部", entries["quantity_prompt_npp041"])
                self.assertNotIn("非數字", entries["quantity_prompt_npp041"])

    def test_native_bridge_host_c_smoke(self):
        callback = extract_function(MAIN, "int Term_control(int what, const char *msg)")
        # Compile real droid.c source, replacing only its three #include lines
        # with host declarations. No change to the implementation bodies.
        shared_source = "\n".join(line for line in BRIDGE.splitlines()
                                  if line not in ('#include "angband.h"',
                                                  '#include "curses.h"',
                                                  '#include "droid.h"'))
        prefix = r"""
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define TERM_CONTROL_LIST_KEYS 1
#define TERM_CONTROL_CONTEXT 2
#define TERM_CONTROL_VISUAL_STATE 4
#define TERM_CONTROL_SHOW_CURSOR 5
#define TERM_CONTROL_DEBUG 6
#define TERM_CONTROL_QUANTITY 7
#define IN_THE_DUNGEON 0
#define strnfmt snprintf
#define Term_control_keys(msg) Term_control(TERM_CONTROL_LIST_KEYS, msg)
#define LOGD(...) ((void)fprintf(stderr, __VA_ARGS__))
typedef int errr;
static int observed_what = -1;
static char observed_msg[2048];
int control_msg(int what, const char *msg) {
    observed_what = what;
    snprintf(observed_msg, sizeof(observed_msg), "%s", msg);
    return 0;
}
void my_strcat(char *dest, const char *source, size_t length) {
    size_t used = strlen(dest);
    if (used < length) strncat(dest, source, length - used - 1);
}
int Term_control(int what, const char *msg);
void strdeldup(char *str);
"""
        suffix = r"""
int main(void) {
    assert(Term_control_quantity("Quantity (0-25): ", 25, 1) == 0);
    assert(observed_what == TERM_CONTROL_QUANTITY);
    assert(strcmp(observed_msg, "25:1:Quantity (0-25): ") == 0);
    assert(Term_control(TERM_CONTROL_QUANTITY, "end") == 0);
    assert(observed_what == TERM_CONTROL_QUANTITY);
    assert(strcmp(observed_msg, "end") == 0);
    soft_kbd_linger("\x24{quant}");
    soft_kbd_flush();
    assert(observed_what == TERM_CONTROL_LIST_KEYS);
    assert(strcmp(observed_msg, "\x24{quant}") == 0);
    soft_kbd_clear(true);
    assert(observed_what == TERM_CONTROL_LIST_KEYS);
    assert(strcmp(observed_msg, "\x24{clear}") == 0);
    return 0;
}
"""
        with tempfile.TemporaryDirectory(prefix="npp041-quantity-") as temp:
            root = Path(temp)
            src, binary = root / "bridge.c", root / "bridge"
            src.write_text(prefix + "\n" + callback + "\n"
                           + shared_source + "\n" + suffix, encoding="utf-8")
            cc = os.environ.get("CC", "cc")
            compile_run = subprocess.run(
                [cc, "-std=c99", "-Wall", "-Wextra", "-Werror",
                 str(src), "-o", str(binary)],
                text=True, capture_output=True, check=False
            )
            self.assertEqual(0, compile_run.returncode, compile_run.stdout
                             + "\n" + compile_run.stderr)
            run = subprocess.run([str(binary)], text=True, capture_output=True, check=False)
            self.assertEqual(0, run.returncode, run.stdout + "\n" + run.stderr)


if __name__ == "__main__":
    unittest.main()
