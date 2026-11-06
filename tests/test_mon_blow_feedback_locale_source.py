#!/usr/bin/env python3
"""Regression guards for display-only Angband monster melee messages."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
BLOWS = (CPP / "mon-blows.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-mon-blow-feedback.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_mon_blow_feedback_locale.c").read_text(
    encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class MonsterBlowFeedbackLocaleSourceTest(unittest.TestCase):
    def test_all_thirteen_fixed_english_messages_are_in_dictionary(self):
        entries = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', HEADER)
        self.assertEqual(13, len(entries))
        self.assertEqual(13, len({en for en, _ in entries}))
        self.assertEqual(13, BLOWS.count('angdroid_mon_blow_feedback('))
        for en, zh in entries:
            with self.subTest(english=en):
                self.assertIn('angdroid_mon_blow_feedback("' + en + '",', BLOWS)
                self.assertIn('verify("' + en + '", "' + zh + '");', C_TEST)
                self.assertNotIn('msg("' + en + '");', BLOWS)
                self.assertNotIn("%", zh)

    def test_messages_stay_in_original_damage_life_and_gold_branches(self):
        for text in (
            'case PROJ_ACID:',
            'case PROJ_ELEC:',
            'case PROJ_FIRE:',
            'case PROJ_COLD:',
            'if (player_of_has(context->p, OF_HOLD_LIFE)',
            'player_exp_lose(context->p, d / 10, false);',
            'player_exp_lose(context->p, d, false);',
            'current_player->au -= gold;',
            'if (gold <= 0)',
            'while (gold > 0)',
            'if (current_player->au) {',
        ):
            self.assertIn(text, BLOWS)
        self.assertIn(
            'msgt(method->msgt, "你受到 %d 點傷害。", damage);', BLOWS)
        self.assertIn(
            'msgt(method->msgt, "You take %d damage.", damage);', BLOWS)
        self.assertIn('msg("%d 枚金幣被偷走了！", gold);', BLOWS)
        self.assertIn('msg("%d coins were stolen!", gold);', BLOWS)

    def test_theft_and_food_english_fallback_and_sounds(self):
        for literal in (
            'msg("%s tries to steal %s %s, but fails.", m_name,',
            'msg("%s試圖偷走你的%s%s，但失敗了。", m_name,',
            'msg("%s %s (%c) was stolen!",',
            'msg("你的%s%s（%c）被偷走了！",',
            'msg("Your %s (%c) was eaten!", o_name,',
            'msg("你的%s（%c）被吃掉了！", o_name,',
            'msg("One of your %s (%c) was eaten!", o_name,',
            'msg("你的一件%s（%c）被吃掉了！", o_name,',
            'gear_object_for_use(context->p, obj, 1,',
            'monster_carry(cave, context->mon, stolen)',
            'context->blinked = true;',
        ):
            self.assertIn(literal, BLOWS)

    def test_english_unknown_and_null_fallback(self):
        self.assertIn('if (!english || !zh_tw) return english;', HEADER)
        self.assertIn('return english;', HEADER)
        self.assertIn('angdroid_mon_blow_feedback("future attack", 1)', C_TEST)
        self.assertIn('angdroid_mon_blow_feedback(NULL, 1) == NULL', C_TEST)
        self.assertIn('"droid.h"', BLOWS)
        self.assertIn('"zh-tw-mon-blow-feedback.h"', BLOWS)

    def test_offline_script_and_checker_coverage(self):
        self.assertIn("test_mon_blow_feedback_locale.c", RUNNER)
        self.assertIn("tests/test_mon_blow_feedback_locale_source.py", VERIFY)
        self.assertIn("len(blow_entries) == 13", CHECKER)
        self.assertIn("blow_ui.count('angdroid_mon_blow_feedback(') == 13",
                      CHECKER)


if __name__ == "__main__":
    unittest.main()
