#!/usr/bin/env python3
"""Source checks for Traditional Chinese ignore menus, subwindows and colors."""
from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1] / "app/src/main/cpp/angband"
UI = (ROOT / "ui-options.c").read_text(encoding="utf-8")
DICT = (ROOT / "zh-tw-ui.h").read_text(encoding="utf-8")
IGNORE = (ROOT / "list-ignore-types.h").read_text(encoding="utf-8")
WINDOWS = (ROOT / "ui-display.c").read_text(encoding="utf-8")
OBJ_IGNORE = (ROOT / "obj-ignore.c").read_text(encoding="utf-8")

PAIRS = re.findall(r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', DICT)
TRANSLATED = dict(PAIRS)


class IgnoreMenuLocalizationSourceTest(unittest.TestCase):
    def test_ignore_and_subwindow_labels_covered(self):
        ignore_types = re.findall(r'ITYPE\(\s*\w+,\s*"([^"]+)"', IGNORE)
        sval_table = re.search(
            r'static tval_desc sval_dependent\[\]\s*=\s*\{(.*?)\n\};',
            UI, flags=re.S
        )
        self.assertIsNotNone(sval_table)
        sval_names = re.findall(r'\{\s*TV_\w+,\s*"([^"]+)"', sval_table.group(1))
        window_table = re.search(
            r'const char \*window_flag_desc\[32\]\s*=\s*\{(.*?)\n\};',
            WINDOWS, flags=re.S
        )
        self.assertIsNotNone(window_table)
        window_names = re.findall(r'"([^"]+)"', window_table.group(1))
        self.assertGreaterEqual(len(ignore_types), 25)
        self.assertGreaterEqual(len(sval_names), 15)
        self.assertGreaterEqual(len(window_names), 14)
        self.assertEqual(len(PAIRS), len(TRANSLATED), "duplicate text key")
        all_labels = (
            ignore_types + sval_names + window_names +
            ["no ignore", "bad", "average", "good", "non-artifact"] +
            ["Quality ignore menu", "Quality ignoring options",
             "Ego ignoring options", "Autoinscription setup"]
        )
        self.assertEqual([x for x in all_labels if x not in TRANSLATED], [])

    def test_ignore_menu_is_display_only_and_cjk_width_safe(self):
        self.assertRegex(UI, r"angdroid_game_ui_text\(\s*quality_choices\[oid\]\.name\s*,")
        self.assertRegex(UI, r"angdroid_game_ui_text\(\s*quality_values\[level\]\.name\s*,")
        self.assertRegex(UI, r"angdroid_game_ui_text\(\s*quality_values\[oid\]\.name\s*,")
        self.assertIn("angdroid_menu_copy_columns(clipped,", UI)
        self.assertIn("angdroid_game_ui_text(sval_dependent[line].desc", UI)
        self.assertIn("angdroid_game_ui_text(extra_item_options[line].name", UI)
        self.assertIn("ignore_level[oid] = menu.cursor;", UI)
        self.assertIn("sval_menu(sval_dependent[oid].tval, sval_dependent[oid].desc)", UI)
        self.assertIn("quality_name_struct quality_values[IGNORE_MAX]", OBJ_IGNORE)
        self.assertIn("EGO_MENU_HELPTEXT);", UI)

    def test_color_and_keymap_prompts_are_localized(self):
        self.assertIn('format("顏色：%d　名稱：%s　索引：%c"', UI)
        self.assertIn('format("Color = %d, Name = %s, Index = %c"', UI)
        self.assertIn('angdroid_game_ui_text("Command (n/N/k/K/r/R/g/G/b/B): "', UI)
        self.assertIn('format("（鍵位設定最多 %d 個按鍵。）"', UI)
        self.assertIn('format("(Maximum keymap length is %d keys.)"', UI)


if __name__ == "__main__":
    unittest.main()
