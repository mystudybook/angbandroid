#!/usr/bin/env python3
"""Regression contracts for UTF-8-safe native text editing."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
UI = (CPP / "ui-input.c").read_text(encoding="utf-8")
HELPER = (CPP / "ui-input-utf8.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_ui_input_utf8.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
BIRTH = (CPP / "ui-birth.c").read_text(encoding="utf-8")
PLAYER = (CPP / "ui-player.c").read_text(encoding="utf-8")


class InputUtf8SourceTest(unittest.TestCase):
    def test_forward_delete_uses_aligned_cursor_pointer(self):
        handler = UI.split("bool askfor_aux_keypress(", 1)[1].split(
            "\n\n/**", 1)[0]
        self.assertIn('#include "ui-input-utf8.h"', UI)
        self.assertIn('oshift = angdroid_input_next_codepoint(ocurs);', handler)
        self.assertNotIn('utf8_fskip(buf + *curs, 1, NULL)', handler)
        self.assertIn('memmove(ocurs, oshift, *len - (oshift - buf));', handler)
        self.assertIn('angdroid_utf8_next_scalar(cursor, &scalar)', HELPER)

    def test_both_input_widgets_use_columns_and_lossless_prefill(self):
        self.assertEqual(
            2, UI.count('Term_gotoxy(x + (int)angdroid_input_cursor_columns(buf, k), y);'))
        self.assertEqual(
            4, UI.count('if (!angdroid_input_prefill_fits(buf, len))'))
        self.assertNotIn("buf[len-1] =", UI)
        self.assertIn('angdroid_terminal_columns(scalar)', HELPER)
        self.assertIn('if (!text || !capacity) return false;', HELPER)

    def test_name_editor_preserves_existing_saved_name(self):
        method = UI.split("bool get_character_name(", 1)[1].split(
            "\n\n/**", 1)[0]
        self.assertIn('angdroid_input_prefill_copy(buf, buflen, player->full_name)', method)
        self.assertIn('if (!res)', method)
        self.assertIn('msg("%s", angdroid_zh_tw_enabled() ?', method)
        self.assertNotIn('my_strcpy(buf, player->full_name, buflen)', method)
        self.assertIn('char namebuf[PLAYER_NAME_LEN] = "";', PLAYER)
        self.assertIn('if (!source || !angdroid_input_prefill_fits(source, capacity))',
                      HELPER)
        self.assertIn('memcpy(dest, source, length + 1);', HELPER)

    def test_history_editor_preserves_existing_saved_text(self):
        method = BIRTH.split("static enum birth_stage get_history_command(", 1)[1].split(
            "\n\n/**", 1)[0]
        self.assertIn('#include "ui-input-utf8.h"', BIRTH)
        self.assertIn('angdroid_input_prefill_copy(history, sizeof(history),',
                      method)
        self.assertIn('return BIRTH_HISTORY_CHOICE;', method)
        self.assertIn('switch (edit_text(history, sizeof(history)))', method)
        self.assertNotIn('my_strcpy(history, player->history', method)
        self.assertNotIn('char old_history[240]', method)
        self.assertIn('if (n == 1 && scalar == 0xFFFDu', HELPER)

    def test_birth_history_editor_uses_cjk_columns_and_safe_cancel(self):
        method = BIRTH.split("static int edit_text(char *buffer, int buflen)", 1)[1].split(
            "\n\n/**\n * ------------------------------------------------------------------------\n * Allowing the player", 1)[0]
        self.assertIn('get_screen_loc(cursor, &x, &y, n_lines, line_starts, line_lengths);',
                      method)
        self.assertIn('angdroid_input_cursor_columns(', method)
        self.assertIn('buffer, line_start', method)
        self.assertIn('buffer, (size_t)cursor', method)
        self.assertIn('if (cols >= (size_t)area.width)', method)
        self.assertIn('if (n_lines == 0 || (size_t)y >= n_lines) break;', method)
        cancel = method.split('case ESCAPE:', 1)[1].split('case KC_ENTER:', 1)[0]
        self.assertLess(cancel.index('mem_free(line_starts);'),
                        cancel.index('return -1;'))
        self.assertLess(cancel.index('mem_free(line_lengths);'),
                        cancel.index('return -1;'))
        self.assertLess(cancel.index('textblock_free(tb);'),
                        cancel.index('return -1;'))

    def test_host_regressions_in_offline_runners(self):
        self.assertIn('tests/native/test_ui_input_utf8.c', RUN)
        self.assertIn('"$BUILD_DIR/test_ui_input_utf8"', RUN)
        self.assertIn('test_ui_input_utf8_source.py', VERIFY)
        self.assertIn('angdroid_input_cursor_columns("A🐱B", 2)', NATIVE)
        self.assertIn('angdroid_input_prefill_fits(short_name, 6)', NATIVE)


if __name__ == "__main__":
    unittest.main()
