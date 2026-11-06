#!/usr/bin/env python3
"""Display-only locale contracts for native projection effects on terrain/objects."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
TERRAIN = (CPP / "project-feat.c").read_text(encoding="utf-8")
MONSTER = (CPP / "project-mon.c").read_text(encoding="utf-8")
OBJECT = (CPP / "project-obj.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-projection-feedback.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_projection_feedback_locale.c").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

TRANSLATIONS = (
    ("The rubble turns into mud!", "碎石堆化成了泥漿！"),
    ("There was something buried in the rubble!", "碎石堆裡原來埋著東西！"),
    ("The door turns into mud!", "門化成了泥漿！"),
    ("The vein turns into mud!", "礦脈化成了泥漿！"),
    ("You have found something!", "你發現了某樣東西！"),
    ("The wall turns into mud!", "牆壁化成了泥漿！"),
    ("There is a bright flash of light!", "一道耀眼的光芒閃過！"),
    ("The trap seizes up.", "陷阱卡住不動了。"),
    ("Click!", "喀噠！"),
    ("You come to rest next to a wall.", "你在牆邊停了下來。"),
    ("You are thrown into molten lava!", "你被拋進滾燙的熔岩中！"),
)

class ProjectionTerrainFeedbackLocaleSourceTest(unittest.TestCase):
    def test_all_original_environment_messages_have_exact_ui_translations(self):
        self.assertEqual(11, len(TRANSLATIONS))
        sources = (TERRAIN, MONSTER, OBJECT)
        for en, zh in TRANSLATIONS:
            with self.subTest(english=en):
                self.assertIn('{ "' + en + '", "' + zh + '" }', HEADER)
                self.assertIn('verify(angdroid_project_feedback, "' + en +
                              '", "' + zh + '");', NATIVE)
                key = 'angdroid_project_feedback("' + en + '",'
                expected = 2 if en in ("Click!", "The vein turns into mud!") else 1
                self.assertEqual(expected, sum(s.count(key) for s in sources))
                self.assertNotIn("%", zh)

    def test_all_13_fixed_format_calls_retained_and_localization_enabled(self):
        for source, count in ((TERRAIN, 10), (MONSTER, 2), (OBJECT, 1)):
            self.assertEqual(count, source.count(
                'msg("%s", angdroid_project_feedback('))
            self.assertIn('#include "droid.h"', source)
            self.assertIn('#include "zh-tw-projection-feedback.h"', source)
            # Keep the fixed-text sink count exact: project-obj.c now
            # also contains dynamic damage/immunity/destruction messages.
            self.assertGreaterEqual(source.count('angdroid_zh_tw_enabled()'), count)
        for en, _ in TRANSLATIONS:
            self.assertNotIn('msg("' + en + '");', TERRAIN)
            self.assertNotIn('msg("' + en + '");', MONSTER)
            self.assertNotIn('msg("' + en + '");', OBJECT)

    def test_original_visibility_and_terrain_operations_are_unchanged(self):
        for name in ("square_isseen(cave, grid)", "square_isview(cave, grid)",
                     "square_destroy_rubble(cave, grid);",
                     "square_destroy_door(cave, grid);",
                     "square_disable_trap(cave, grid);",
                     "square_unlock_door(cave, grid);",
                     "place_gold(cave, grid, player->depth, ORIGIN_FLOOR);"):
            self.assertIn(name, TERRAIN)
        self.assertIn("if (square(cave, grid)->mon < 0)", MONSTER)
        self.assertIn("square_isfiery(cave, grid)", MONSTER)
        self.assertIn("monster_swap(grid, next);", MONSTER)
        self.assertIn("sqinfo_off(square(cave, grid)->info, SQUARE_PROJECT);",
                      MONSTER)
        for snippet in ("is_locked_chest(context->obj)",
                        "unlock_chest((struct object * const)context->obj);",
                        "!ignore_item_ok(player, context->obj)"):
            self.assertIn(snippet, OBJECT)

    def test_dictionary_is_unique_and_english_fallback_is_unmodified(self):
        section = HEADER.split(
            "static inline const char *angdroid_project_feedback(", 1)[1].split(
            "static inline const char *angdroid_project_stat(", 1)[0]
        pairs = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', section)
        self.assertEqual(30, len(pairs))
        self.assertEqual(30, len({en for en, _ in pairs}))
        self.assertIn("if (!english || !zh_tw) return english;", section)
        self.assertIn("return english;", section)
        self.assertIn("len(matches) == count", CHECKER)
        self.assertIn('("angdroid_project_feedback(", 30)', CHECKER)
        self.assertIn('translate(english, 0) == english', NATIVE)

    def test_offline_regression_registration(self):
        self.assertIn("tests/test_projection_terrain_feedback_locale_source.py",
                      VERIFY)
        self.assertIn('test_projection_feedback_locale.c',
                      (ROOT / "scripts/test-native-utf8.sh").read_text(
                          encoding="utf-8"))

if __name__ == "__main__":
    unittest.main()
