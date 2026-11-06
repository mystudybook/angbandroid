#!/usr/bin/env python3
"""Source contracts for ranged target and monster crushing display locale.

The game mechanics, sounds and deletion logic are deliberately unchanged.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
ATTACK = (CPP / "player-attack.c").read_text(encoding="utf-8")
MOVE = (CPP / "mon-move.c").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
BRACES = (ROOT / "tests/test_native_locale_branch_braces.py").read_text(
    encoding="utf-8")


class CombatFeedbackEdgeLocaleSourceTest(unittest.TestCase):
    def test_ranged_out_of_range_warning_localized_with_same_distance(self):
        start = ATTACK.index("/* Check for target validity */")
        end = ATTACK.index("/* Sound */", start)
        section = ATTACK[start:end]
        self.assertIn("if ((dir == DIR_TARGET) && target_okay())", section)
        self.assertIn("taim = distance(grid, target);", section)
        self.assertIn("if (taim > range)", section)
        self.assertIn("char msg[80];", section)
        self.assertIn("if (angdroid_zh_tw_enabled()) {", section)
        self.assertIn(
            '"目標超出射程 %d 格，仍要繼續嗎？ ", taim - range);', section)
        self.assertIn('"Target out of range by %d squares. Fire anyway? ",',
                      section)
        self.assertEqual(2, section.count("taim - range"))
        self.assertEqual(2, section.count("strnfmt(msg, sizeof(msg),"))
        self.assertIn("if (!get_check(msg)) return;", section)

    def test_warning_confirmation_precedes_shoot_and_turn_consumption(self):
        start = ATTACK.index("/* Check for target validity */")
        end = ATTACK.index("/* Calculate the path */", start)
        ranged = ATTACK[start:end]
        self.assertLess(ranged.index("if (!get_check(msg)) return;"),
                        ranged.index("sound(MSG_SHOOT);"))
        self.assertLess(ranged.index("sound(MSG_SHOOT);"),
                        ranged.index("p->upkeep->energy_use ="))
        self.assertIn("taim = distance(grid, target);", ranged)

    def test_monster_item_crush_visibility_sound_and_delete_preserved(self):
        start = MOVE.index("/* Try to pick up, or crush */")
        end = MOVE.index("/* Next object */", start)
        section = MOVE[start:end]
        self.assertIn("if (safe) {", section)
        self.assertIn("} else if (rf_has(mon->race->flags, RF_TAKE_ITEM)) {",
                      section)
        self.assertIn("if (square_isseen(cave, new) && !ignore_item_ok(player, obj)) {",
                      section)
        self.assertIn('msgt(MSG_DESTROY, "%s壓碎了%s。", m_name, o_name);',
                      section)
        self.assertIn('msgt(MSG_DESTROY, "%s crushes %s.", m_name, o_name);',
                      section)
        self.assertEqual(2, section.count(
            "square_delete_object(cave, new, obj, true, true);"))
        self.assertIn("if (angdroid_zh_tw_enabled())", section)
        self.assertIn("test_monster_crush_message_only_when_visible_and_not_ignored",
                      BRACES)

    def test_literal_formats_preserve_argument_counts_and_english_fallback(self):
        self.assertEqual(2, len(re.findall(
            r'msgt\(MSG_DESTROY, "[^"]*%s[^"]*%s[^"]*", m_name, o_name\)',
            MOVE)))
        self.assertIn('"%s crushes %s."', MOVE)
        self.assertIn('"Target out of range by %d squares. Fire anyway? "',
                      ATTACK)
        self.assertIn('"目標超出射程 %d 格，仍要繼續嗎？ "', ATTACK)

    def test_registered_offline_without_ci(self):
        self.assertIn("tests/test_combat_feedback_edge_locale_source.py", VERIFY)
        self.assertIn("tests/test_native_locale_branch_braces.py", VERIFY)


if __name__ == "__main__":
    unittest.main()
