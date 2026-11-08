#!/usr/bin/env python3
"""Source-level regression for display-only Angband spell effect summaries."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
PLAYER = (CPP / "player-spell.c").read_text(encoding="utf-8")
UI = (CPP / "ui-spell.c").read_text(encoding="utf-8")
EFFECTS = (CPP / "effects-info.c").read_text(encoding="utf-8")
EFFECT_CODES = (CPP / "list-effects.h").read_text(encoding="utf-8")
MAPPING = (CPP / "zh-tw-spell-effect-info.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class SpellEffectInfoLocaleSourceTest(unittest.TestCase):
    def test_every_canonical_effect_info_label_is_translated(self):
        effect_info = set(re.findall(
            r'EFFECT\(\s*\w+,\s*(?:true|false),\s*"([^"]+)"',
            EFFECT_CODES))
        self.assertEqual(effect_info, {
            "hurt", "heal", "dam", "dur", "power", "range", "blows"})
        translations = dict(re.findall(
            r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},\s*$',
            MAPPING, re.M))
        self.assertEqual(len(translations), 12)
        self.assertEqual(effect_info | {
            "random", ", rad %d", ", rad 2", "rad 2", ", len %d"},
            translations.keys())
        for key, value in translations.items():
            with self.subTest(key=key):
                self.assertTrue(value)
                self.assertEqual(key.count("%d"), value.count("%d"))
                self.assertNotIn("%s", value)

    def test_display_translation_does_not_change_effect_info_semantics(self):
        self.assertIn('#include "zh-tw-spell-effect-info.h"', PLAYER)
        self.assertIn("type = effect_info(effect);", PLAYER)
        self.assertIn("if (type == NULL) return;", PLAYER)
        self.assertIn("type = angdroid_zh_tw_spell_effect_info(type,", PLAYER)
        self.assertIn("angdroid_zh_tw_enabled());", PLAYER)
        self.assertLess(PLAYER.index("type = effect_info(effect);"),
                        PLAYER.index("type = angdroid_zh_tw_spell_effect_info(type,"))
        self.assertIn("dice_roll(effect->dice, &rv);", PLAYER)
        self.assertIn("ist->pre_rv = rv;", PLAYER)
        self.assertIn("if (!effect_do(spell->effect,", PLAYER)
        self.assertIn('streq(effect_info(effect), "dam")', EFFECTS)

    def test_radius_length_and_random_are_render_only(self):
        self.assertEqual(3, PLAYER.count('angdroid_zh_tw_spell_effect_info(", rad %d",'))
        self.assertIn('angdroid_zh_tw_spell_effect_info(", rad 2",', PLAYER)
        self.assertIn('angdroid_zh_tw_spell_effect_info("rad 2",', PLAYER)
        self.assertIn('angdroid_zh_tw_spell_effect_info(", len %d",', PLAYER)
        self.assertIn('angdroid_zh_tw_spell_effect_info("random",', PLAYER)
        self.assertIn("if (!english || !zh_tw) return english;", MAPPING)
        self.assertIn("return english;", MAPPING)

    def test_spell_view_shows_localized_projection_and_safe_utf8_column_clip(self):
        self.assertIn('#include "zh-tw-projection-terms.h"', UI)
        self.assertIn("angdroid_projection_description(", UI)
        self.assertIn("effect_projection(e), angdroid_zh_tw_enabled());", UI)
        self.assertIn("char help[192];", UI)
        self.assertIn("char out[256];", UI)
        self.assertIn("angdroid_utf8_clip_columns(help, 22);", UI)
        self.assertIn("get_spell_info(spell_index, help, sizeof(help));", UI)
        self.assertIn("text_out_c(COLOUR_L_GREEN", UI)
        self.assertNotIn("projections[e->subtype].player_desc =", UI)

    def test_spell_summary_appends_whole_utf8_scalars(self):
        unicode = (ROOT / "app/src/main/cpp/curses/unicode_display.h").read_text(
            encoding="utf-8")
        native = (ROOT / "tests/native/test_spell_effect_info_locale.c").read_text(
            encoding="utf-8")
        self.assertIn('#include "../curses/unicode_display.h"', PLAYER)
        self.assertIn(
            "static inline size_t angdroid_utf8_append_complete(", unicode)
        self.assertIn(
            "return offset + angdroid_utf8_copy_complete(", unicode)
        self.assertEqual(
            5, PLAYER.count("angdroid_utf8_append_complete("))
        self.assertIn(
            'offset = angdroid_utf8_append_complete(p, len, offset, type);',
            PLAYER)
        self.assertIn('offset += append_random_value_string(', PLAYER)
        self.assertIn(
            "offset = angdroid_utf8_append_complete(",
            PLAYER)
        self.assertNotIn(
            'strnfmt(p + offset, len - offset, " %s ", type)', PLAYER)
        self.assertNotIn(
            'strnfmt(p + offset, len - offset, "%s", special)', PLAYER)
        self.assertIn("if (!p || !len) return;", PLAYER)
        self.assertIn('angdroid_utf8_append_complete(summary, sizeof(summary),',
                      native)
        self.assertIn('strcmp(summary, "  傷害 1234;") == 0', native)
        self.assertIn('angdroid_utf8_append_complete(NULL, 0, 0, "中文")', native)

    def test_both_offline_test_runners_registered(self):
        self.assertIn("tests/native/test_spell_effect_info_locale.c", RUNNER)
        self.assertIn("tests/test_spell_effect_info_locale_source.py", VERIFY)


if __name__ == "__main__":
    unittest.main()
