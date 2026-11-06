#!/usr/bin/env python3
"""Static guards on Angband's screen-wrapped long spell descriptions."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = (ROOT / "app/src/main/cpp/angband/ui-output.c").read_text(
    encoding="utf-8")
SPELL = (ROOT / "app/src/main/cpp/angband/ui-spell.c").read_text(
    encoding="utf-8")
LAYOUT = (ROOT / "app/src/main/cpp/curses/unicode_layout.h").read_text(
    encoding="utf-8")
TERMINAL = (ROOT / "app/src/main/cpp/angband/ui-term.c").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_screen_text_wrap.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")


class SpellDescriptionScreenWrapSourceTest(unittest.TestCase):
    def test_screen_wrap_uses_same_native_terminal_cell_width(self):
        self.assertIn('#include "../curses/unicode_layout.h"', OUTPUT)
        self.assertIn('angdroid_screen_text_needs_wrap(x, wrap, ch, a)', OUTPUT)
        self.assertIn('span = angdroid_is_wide_text(ch, a) ? 2 : 1;', OUTPUT)
        self.assertIn('x += span;', OUTPUT)
        # The legacy one-cell increment is valid only while moving ASCII words.
        self.assertEqual(1, OUTPUT.count('if (++x > wrap) x = wrap;'))
        self.assertIn('angdroid_is_wide_text(ch, attr) ? 2 : 1;', LAYOUT)
        self.assertIn('return column + span > wrap - 1;', LAYOUT)
        self.assertIn("int span = angdroid_is_wide_text(c, a) ? 2 : 1;", TERMINAL)
        self.assertIn("Term->scr->cx += span;", TERMINAL)

    def test_ascii_word_reflow_cannot_split_cjk_continuation(self):
        self.assertIn('angdroid_is_wide_text(cv[i], av[i])', OUTPUT)
        self.assertIn('angdroid_is_wide_text(previous_char,', OUTPUT)
        self.assertIn('n = wrap;', OUTPUT)
        self.assertIn("if (ch == L' ')", OUTPUT)
        self.assertIn('if (n < wrap) Term_erase(n, y, 255);', OUTPUT)

    def test_utf8_buffer_and_screen_height_bounded(self):
        self.assertIn('text_mbstowcs(buf, str, 1023);', OUTPUT)
        self.assertIn('buf[1023] = 0;', OUTPUT)
        self.assertIn('if (++y >= h) return;', OUTPUT)
        self.assertIn('loc->col > 0 ? loc->col - 1 : 0', SPELL)
        self.assertIn('loc->col > 0 ? 1 : 0', SPELL)

    def test_spell_browser_wiring_and_test_registration(self):
        self.assertIn('text_out_hook = text_out_to_screen;', SPELL)
        self.assertIn('angdroid_zh_tw_spell_description(', SPELL)
        self.assertIn('test_screen_text_wrap.c', RUNNER)
        self.assertIn('tests/test_spell_screen_wrap_source.py', VERIFY)
        self.assertIn('angdroid_screen_text_needs_wrap', NATIVE)
        self.assertIn('L"中文法術說明"', NATIVE)


if __name__ == "__main__":
    unittest.main()
