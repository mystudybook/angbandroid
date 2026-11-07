#!/usr/bin/env python3
"""Display-only localization guards for player-calcs state notifications."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "player-calcs.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-player-calcs-feedback.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_player_calcs_feedback_locale.c").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class PlayerCalcsFeedbackSourceTest(unittest.TestCase):
    def test_all_twelve_fixed_status_messages_have_safe_english_fallback(self):
        pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', HEADER)
        self.assertEqual(12, len(pairs))
        self.assertEqual(12, len({en for en, _ in pairs}))
        self.assertEqual(12, SOURCE.count("angdroid_player_calcs_feedback("))
        for en, zh in pairs:
            with self.subTest(en=en):
                self.assertTrue(zh)
                self.assertNotIn("%", zh)
                self.assertIn(
                    'angdroid_player_calcs_feedback("' + en + '",', SOURCE)
                self.assertIn('verify("' + en + '", "' + zh + '");', C_TEST)
                self.assertNotIn('msg("' + en + '");', SOURCE)

    def test_inventory_and_equipment_condition_gates_unchanged(self):
        for token in (
            'if (character_dungeon) {',
            'if (character_dungeon && p->upkeep->inven_cnt == old_inven_cnt)',
            'if (p->state.heavy_shoot != state.heavy_shoot)',
            'if (state.heavy_shoot)',
            'else if (equipped_item_by_slot_name(p, "shooting"))',
            'if (p->state.heavy_wield != state.heavy_wield)',
            'if (state.heavy_wield)',
            'else if (equipped_item_by_slot_name(p, "weapon"))',
            'if (p->state.bless_wield != state.bless_wield)',
            'if (p->state.cumber_armor != state.cumber_armor)',
            'memcpy(&p->state, &state, sizeof(state));',
        ):
            self.assertIn(token, SOURCE)

    def test_spell_forget_remember_translated_only_after_flag_updates(self):
        self.assertEqual(2, SOURCE.count('msg("你忘記了「%s」。",'))
        self.assertEqual(2, SOURCE.count(
            'msg("You have forgotten the %s of %s.",'))
        self.assertEqual(1, SOURCE.count('msg("你重新想起了「%s」。",'))
        self.assertEqual(1, SOURCE.count(
            'msg("You have remembered the %s of %s.",'))
        self.assertEqual(3, SOURCE.count(
            'angdroid_zh_tw_spell_name(spell->name, true)'))
        self.assertIn('spell->slevel <= p->lev', SOURCE)
        self.assertIn('spell->slevel > p->lev', SOURCE)
        self.assertIn('p->spell_flags[j] |= PY_SPELL_FORGOTTEN;', SOURCE)
        self.assertIn('p->spell_flags[j] &= ~PY_SPELL_LEARNED;', SOURCE)
        self.assertIn('p->spell_flags[j] |= PY_SPELL_LEARNED;', SOURCE)
        self.assertIn('p->upkeep->new_spells++;', SOURCE)
        self.assertIn('p->upkeep->new_spells--;', SOURCE)

    def test_study_count_english_grammar_and_chinese_stay_separate(self):
        self.assertIn(
            'msg("你還可以學習 %d 項新法術。", p->upkeep->new_spells);',
            SOURCE)
        self.assertIn(
            'msg("You can learn %d more %s.", p->upkeep->new_spells, buf);',
            SOURCE)
        for token in (
            'my_strcpy(buf, r->spell_noun, sizeof(buf));',
            'my_strcat(buf, "s", sizeof(buf));',
            'my_strcat(buf, ", ", sizeof(buf));',
            'my_strcat(buf, " or ", sizeof(buf));',
            'p->upkeep->redraw |= (PR_STUDY | PR_OBJECT);',
        ):
            self.assertIn(token, SOURCE)
        self.assertIn('"zh-tw-spell-names.h"', SOURCE)
        self.assertIn('"droid.h"', SOURCE)

    def test_unknown_and_null_fallback_and_validation_registration(self):
        self.assertIn('if (!english || !zh_tw) return english;', HEADER)
        self.assertIn('return english;', HEADER)
        self.assertIn(
            'angdroid_player_calcs_feedback("future equipment", 1)', C_TEST)
        self.assertIn('angdroid_player_calcs_feedback(NULL, 1) == NULL',
                      C_TEST)
        self.assertIn("test_player_calcs_feedback_locale.c", RUNNER)
        self.assertIn("tests/test_player_calcs_feedback_locale_source.py",
                      VERIFY)
        self.assertIn("len(calcs_pairs) == 12", CHECKER)


if __name__ == "__main__":
    unittest.main()
