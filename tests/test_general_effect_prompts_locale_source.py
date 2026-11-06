#!/usr/bin/env python3
"""Regression checks for Angband's display-only zh-TW effect prompts."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ANGBAND = ROOT / "app/src/main/cpp/angband"
SOURCE = (ANGBAND / "effect-handler-general.c").read_text(encoding="utf-8")
HEADER = (ANGBAND / "zh-tw-general-effect-messages.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_general_effect_messages_locale.c").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

EXPECTED = (
    ("Enchant which item? ", "要為哪件物品附魔？ "),
    ("You have nothing to enchant.", "你沒有可以附魔的物品。"),
    ("Uncurse which item? ", "要解除哪件物品的詛咒？ "),
    ("You have no curses to remove.", "你沒有可以解除的詛咒。"),
    ("Identify which item? ", "要鑑定哪件物品？ "),
    ("You have nothing to identify.", "你沒有需要鑑定的物品。"),
    ("Recharge which item? ", "要為哪件物品重新充能？ "),
    ("You have nothing to recharge.", "你沒有可以重新充能的物品。"),
    ("Brand which kind of ammunition? ", "要為哪種彈藥附加屬性？ "),
    ("You have nothing to brand.", "你沒有可以附加屬性的彈藥。"),
    ("Brand which bolts? ", "要為哪一組弩矢附加屬性？ "),
    ("You have no bolts to brand.", "你沒有可附加屬性的弩矢。"),
    ("Make arrows from which staff? ", "要用哪根木杖製作箭矢？ "),
    ("You have no staff to use.", "你沒有可用來製箭的木杖。"),
    ("Drain charges from which item? ", "要從哪件物品汲取充能？ "),
    ("You have nothing to drain charges from.", "你沒有可以汲取充能的物品。"),
    ("Are you sure you want to descend? ", "確定要下到更深的樓層嗎？ "),
    ("Set recall depth to current depth? ", "要將下次回返深度設為目前樓層嗎？ "),
    ("Word of Recall is already active.  Do you want to cancel it? ", "回返法術已啟動，要取消嗎？ "),
)

class GeneralEffectPromptsLocaleSourceTest(unittest.TestCase):
    def test_each_prompt_is_translated_exactly_once(self):
        self.assertEqual(19, len(EXPECTED))
        for en, zh in EXPECTED:
            with self.subTest(english=en):
                lookup = 'angdroid_general_effect_message("' + en + '", angdroid_zh_tw_enabled())'
                self.assertIn(lookup, SOURCE)
                self.assertEqual(1, SOURCE.count(lookup))
                self.assertIn('{ "' + en + '", "' + zh + '" }', HEADER)
                self.assertIn('{ "' + en + '", "' + zh + '" }', NATIVE)
                self.assertEqual(en.endswith(" "), zh.endswith(" "))
                self.assertNotIn("%", zh)

    def test_gameplay_selectors_and_recall_values_stay_original(self):
        self.assertEqual(8, SOURCE.count("cmd_get_item("))
        self.assertIn('cmd_get_item(context->cmd, "tgtitem", &obj, prompt,', SOURCE)
        self.assertIn('cmd_get_item(cmd, "tgtitem", &obj, q, s, filter,', SOURCE)
        for snippet in (
            "item_tester_uncursable, itemmode",
            "item_tester_unknown, itemmode",
            "tval_can_have_charges, itemmode",
            "tval_is_ammo, itemmode",
            "tval_is_bolt, itemmode",
            "tval_is_staff, itemmode",
            "player->recall_depth = player->max_depth = player->depth;",
            "player->word_recall = randint0(20) + 15;",
            "player->word_recall = 0;",
        ):
            self.assertIn(snippet, SOURCE)

    def test_translation_dictionary_exact_order_unique_and_fallback(self):
        regexp = r'\{ "([^"]+)", "([^"]+)" \}'
        header = re.findall(regexp, HEADER)
        native = re.findall(regexp, NATIVE)
        self.assertEqual(90, len(header))
        self.assertEqual(90, len({en for en, _ in header}))
        self.assertEqual(header, native)
        self.assertIn("if (!english || !zh_tw) return english;", HEADER)
        self.assertIn("return english;", HEADER)
        self.assertIn("angdroid_general_effect_message(cases[i].en, 0) == cases[i].en", NATIVE)
        self.assertIn("len(general_effect_pairs) == 90", CHECKER)

    def test_registered_in_local_verification_only(self):
        self.assertIn("tests/test_general_effect_prompts_locale_source.py", VERIFY)
        self.assertIn("test_general_effect_messages_locale.c",
            (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8"))

if __name__ == "__main__":
    unittest.main()
