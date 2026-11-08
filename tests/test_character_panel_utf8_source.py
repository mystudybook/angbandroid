#!/usr/bin/env python3
"""Character-sheet rendering: invalid UTF-8 at 96-byte boundaries is display-only."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
PLAYER = (CPP / "angband/ui-player.c").read_text(encoding="utf-8")
UNICODE = (CPP / "curses/unicode_display.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_unicode_display.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(
    encoding="utf-8")


class CharacterPanelUtf8SourceTest(unittest.TestCase):
    def test_panel_formats_then_copies_only_complete_utf8_for_display(self):
        self.assertIn("char value[96];", PLAYER)
        self.assertIn("vstrnfmt(pl->value, sizeof pl->value, fmt, vp);", PLAYER)
        self.assertIn("char value[sizeof(p->lines[i].value)];", PLAYER)
        self.assertIn(
            "angdroid_utf8_copy_complete(value, sizeof(value), pl->value);",
            PLAYER)
        self.assertNotIn("my_strcpy(value, pl->value, sizeof(value));", PLAYER)
        self.assertIn('#include "../curses/unicode_display.h"', PLAYER)

    def test_column_width_and_original_panel_position_preserved(self):
        for part in (
            "available = w - offset;",
            "if (available <= 0) continue;",
            "angdroid_utf8_clip_columns(value, (size_t)available);",
            "columns = (int)angdroid_utf8_display_columns(value);",
            "Term_putstr(col + offset, row, -1, pl->attr, value);",
            "Term_putstr(col + w - columns, row, -1, pl->attr, value);",
            "Term_putstr(col, row, -1, COLOUR_WHITE, pl->label);",
        ):
            with self.subTest(part=part):
                self.assertIn(part, PLAYER)

    def test_underlying_stats_storage_and_user_export_unchanged(self):
        self.assertIn("va_start(vp, fmt);", PLAYER)
        self.assertIn("va_end(vp);", PLAYER)
        self.assertIn("angdroid_zh_tw_class_title(show_title()", PLAYER)
        self.assertIn("bool dump_save(const char *path)", PLAYER)
        self.assertIn("get_character_name(namebuf, sizeof namebuf)", PLAYER)
        self.assertIn("my_strcpy(player->full_name, namebuf,", PLAYER)

    def test_reused_unicode_decoder_handles_invalid_tail(self):
        for token in (
            "static inline size_t angdroid_utf8_copy_complete(",
            "angdroid_utf8_next_scalar(text + offset, &scalar)",
            "count > capacity - used - 1",
            "dst[used++] = '?';",
            "dst[used] = '\\0';",
        ):
            self.assertIn(token, UNICODE)
        for token in (
            "char formatted[96], rendered[96];",
            "formatted[93] = (char)0xE4;",
            "formatted[94] = (char)0xB8;",
            'strcmp(rendered + 93, "??") == 0',
            "angdroid_utf8_clip_columns(rendered, 16);",
            "formatted[93] = (char)0xF0;",
        ):
            self.assertIn(token, NATIVE)

    def test_tests_registered_without_github_actions(self):
        self.assertIn("test_unicode_display.c", RUNNER)
        self.assertIn("tests/test_character_panel_utf8_source.py", VERIFY)
        self.assertIn("Character sheet must sanitize panel bytes", CHECKER)
        self.assertIn("Character panel UTF-8 96-byte boundary", CHECKER)


if __name__ == "__main__":
    unittest.main()
