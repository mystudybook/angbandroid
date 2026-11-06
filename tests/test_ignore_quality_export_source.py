#!/usr/bin/env python3
"""Native source regression: ignore-quality menu and screen export zh-TW.

Checks canonical original data, on-screen-only labels, English fallback, and
unchanged quality/selection IDs, export choices and filenames. Does not build
the Android APK.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
UI = (CPP / "ui-object.c").read_text(encoding="utf-8")
HELPER = (CPP / "zh-tw-ignore-quality-action.h").read_text(encoding="utf-8")
DICT = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
TYPES = (CPP / "list-ignore-types.h").read_text(encoding="utf-8")
IGNORE = (CPP / "obj-ignore.c").read_text(encoding="utf-8")
COMMAND = (CPP / "ui-command.c").read_text(encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class IgnoreQualityExportLocaleTest(unittest.TestCase):
    def test_every_canonical_quality_and_ignore_type_is_translated(self):
        types = re.findall(r'ITYPE\(\s*\w+,\s*"([^"]+)"\s*\)', TYPES)
        quality = re.findall(r'\{\s*IGNORE_\w+,\s*"([^"]+)"\s*\}', IGNORE)
        labels = re.findall(r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', DICT)
        names = dict(labels)
        self.assertGreaterEqual(len(types), 25)
        self.assertEqual(5, len(quality))
        self.assertEqual(len(names), len(labels), "duplicate localization dictionary key")
        for english in set(types + quality) - {""}:
            self.assertIn(english, names, "untranslated native ignore label")
            self.assertNotEqual(english, names[english])

    def test_menu_caption_is_display_only_not_a_new_ignore_rule(self):
        self.assertIn('#include "zh-tw-ignore-quality-action.h"', UI)
        menu = UI.split("void textui_cmd_ignore_menu(", 1)[1].split(
            "menu_dynamic_free(m);", 1)[0]
        self.assertIn("value = ignore_level_of(obj);", menu)
        self.assertIn("type = ignore_type_of(obj);", menu)
        self.assertIn("if (value != IGNORE_MAX && type != ITYPE_MAX)", menu)
        self.assertIn("angdroid_ignore_quality_action_text(out_val, sizeof out_val,", menu)
        self.assertIn("quality_values[value].name, ignore_name_for_type(type),", menu)
        self.assertIn("menu_dynamic_add(m, out_val, IGNORE_THIS_QUALITY);", menu)
        self.assertIn("if (selected == IGNORE_THIS_QUALITY)", menu)
        self.assertIn("ignore_level[ignore_type] = ignore_value;", menu)
        self.assertIn("m->selections = all_letters_nohjkl;", menu)
        self.assertIn("menu_dynamic_select(m)", menu)
        self.assertIn("menu_dynamic_longest_entry(m)", menu)

    def test_translator_fallback_and_no_partial_utf8_output(self):
        self.assertIn('#include "zh-tw-ui.h"', HELPER)
        self.assertIn("angdroid_game_ui_text(quality, zh_tw)", HELPER)
        self.assertIn("angdroid_game_ui_text(type, zh_tw)", HELPER)
        self.assertIn('snprintf(out, cap, "忽略所有%s%s", quality_zh, type_zh)', HELPER)
        self.assertIn('snprintf(out, cap, "All %s %s", quality, type)', HELPER)
        self.assertIn("strcmp(quality_zh, quality) != 0", HELPER)
        self.assertIn("strcmp(type_zh, type) != 0", HELPER)
        self.assertIn("if (n < 0 || (size_t)n >= cap)", HELPER)
        self.assertIn("out[0] = '\\0';", HELPER)

    def test_export_translates_only_prompts_not_keycodes_or_files(self):
        table = dict(re.findall(
            r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', DICT))
        required = {
            "Dump as (H)TML or (F)orum text? ": "匯出格式：H HTML／F 論壇文字？ ",
            "Include monster list? ": "要包含怪物清單嗎？ ",
            "Screen dump failed.": "畫面匯出失敗。",
        }
        for english, chinese in required.items():
            self.assertEqual(chinese, table.get(english))
        section = COMMAND.split("static void do_cmd_save_screen_html(", 1)[1]
        self.assertIn("mode == 0 ? \"dump.html\" : \"dump.txt\"", section)
        self.assertIn('path_build(file_name, sizeof(file_name), ANGBAND_DIR_USER, "dump.prf")', section)
        self.assertIn("html_screenshot(tmp_val, mode, other_term);", section)
        self.assertIn("reset_visuals(true);", section)
        self.assertIn("process_pref_file(file_name, true, false);", section)
        self.assertIn('angdroid_game_ui_text("Screen dump failed.", angdroid_zh_tw_enabled())', section)
        self.assertIn('msg("畫面已匯出為 %s 格式。", mode ? "論壇文字" : "HTML")', section)
        self.assertIn('msg("%s screen dump saved.", mode ? "Forum text" : "HTML")', section)
        self.assertIn('"Dump as (H)TML or (F)orum text? ", angdroid_zh_tw_enabled()', section)
        self.assertIn('"hf", 2,', section)
        self.assertIn("case 'h':", section)
        self.assertIn("case 'f':", section)
        self.assertIn('"Include monster list? ", angdroid_zh_tw_enabled()', section)
        self.assertIn("if (!get_check(angdroid_game_ui_text(", section)
        self.assertIn("do_cmd_save_screen_html(mode, ml_term);", section)

    def test_both_tests_wired_to_existing_offline_runner(self):
        self.assertIn("tests/native/test_ignore_quality_action_locale.c", NATIVE_RUNNER)
        self.assertIn("tests/test_ignore_quality_export_source.py", RUNNER)


if __name__ == "__main__":
    unittest.main()
