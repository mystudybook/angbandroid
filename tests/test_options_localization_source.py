#!/usr/bin/env python3
"""Source-level regression checks for Angband's option-menu zh-TW display.

No Android SDK required. This deliberately checks display-only translation,
not the preference parser, savefile format, or runtime rendering on devices.
"""
from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
OPTIONS = (CPP / "ui-options.c").read_text(encoding="utf-8")
CATALOG = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
SOURCE_OPTIONS = (CPP / "list-options.h").read_text(encoding="utf-8")


class OptionsLocalizationSourceTest(unittest.TestCase):
    def test_every_option_description_has_a_translation(self):
        descriptions = re.findall(
            r'OP\(\s*[A-Za-z0-9_]+,\s*"([^"]+)"', SOURCE_OPTIONS
        )
        self.assertGreaterEqual(len(descriptions), 40)
        mappings = re.findall(
            r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', CATALOG
        )
        translated = {source: target for source, target in mappings}
        self.assertEqual(len(mappings), len(translated), "duplicate UI mapping key")
        missing = [name for name in descriptions if name not in translated]
        self.assertEqual(missing, [], "untranslated option descriptions")

    def test_display_uses_locale_and_display_cells(self):
        self.assertIn("angdroid_game_ui_text(option_desc(oid)", OPTIONS)
        self.assertIn("angdroid_menu_copy_columns(clipped, sizeof(clipped)", OPTIONS)
        self.assertIn("format(\"%s%*s\", clipped, (int)(45 - columns)", OPTIONS)
        self.assertNotIn("utf8_strlen(desc)", OPTIONS)
        self.assertIn("option_name(oid)", OPTIONS)
        self.assertIn("option_set(option_name(oid)", OPTIONS)

    def test_menus_and_both_save_paths_are_localized(self):
        self.assertIn('m->title = angdroid_game_ui_text(name,', OPTIONS)
        self.assertIn('option_menu->title = angdroid_game_ui_text("Options Menu",', OPTIONS)
        self.assertIn('m->prompt = angdroid_game_ui_text("Set option', OPTIONS)
        for prompt in (
            "Successfully saved.  Press any key to continue.",
            "Save failed.  Press any key to continue.",
            "Restore failed.  Press any key to continue.",
        ):
            self.assertGreaterEqual(
                OPTIONS.count('get_com(angdroid_game_ui_text("' + prompt + '"'),
                2,
                "keyboard and mouse option menus must show localized feedback",
            )
        self.assertNotRegex(
            OPTIONS,
            r'get_com\("(?:(?:Successfully saved)|(?:Save failed)|'
            r'(?:Restore failed))',
        )


if __name__ == "__main__":
    unittest.main()
