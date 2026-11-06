#!/usr/bin/env python3
"""Regression contracts for Native object-projection dynamic locale messages."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "project-obj.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-projection-feedback.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_projection_feedback_locale.c").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
VERBS = (
    ("melts", "融化了"),
    ("melt", "融化了"),
    ("is destroyed", "被摧毀了"),
    ("are destroyed", "被摧毀了"),
    ("burns up", "燒毀了"),
    ("burn up", "燒毀了"),
    ("shatters", "碎裂了"),
    ("shatter", "碎裂了"),
)

class ProjectionItemFeedbackLocaleSourceTest(unittest.TestCase):
    def test_inventory_damage_keeps_original_count_and_english(self):
        part = SOURCE.split("int inven_damage(", 1)[1].split(
            "typedef struct project_object_handler_context_s", 1)[0]
        for snippet in (
            'msgt(MSG_DESTROY, "你的%s（%c）%s%s！", o_name,',
            'msgt(MSG_DESTROY, "%sour %s (%c) %s %s!",',
            'angdroid_project_inventory_scope(obj->number, amt)',
            'gear_to_label(p, obj)',
            'damage ? "受損了" : "被摧毀了"',
            '((amt == obj->number) ? "All of y" :',
            '(amt > 1 ? "Some of y" : "One of y")',
            '((amt > 1) ? "were" : "was")',
            '(damage ? "damaged" : "destroyed")',
            'amt = obj->number;',
            'if (randint0(10000) < chance) amt++;',
            'gear_object_for_use(p, obj, amt,',
            'k += amt;',
        ):
            self.assertIn(snippet, part)

    def test_artifact_immunity_and_english_agreement_preserved(self):
        part = SOURCE.split("bool project_o(", 1)[1]
        for snippet in (
            'if (obj->artifact || ignore)',
            'if (obvious && obj->known',
            '!ignore_item_ok(player, obj)',
            'msg("「%s」沒有受到影響！", o_name);',
            'msg("The %s %s unaffected!", o_name,',
            'VERB_AGREEMENT(obj->number, "is", "are")',
        ):
            self.assertIn(snippet, part)

    def test_floor_destruction_and_original_rules_preserved(self):
        part = SOURCE.split("bool project_o(", 1)[1]
        for snippet in (
            'if (obvious && obj->known && note_kill',
            'msgt(MSG_DESTROY, "「%s」%s！", o_name,',
            'angdroid_project_object_destroy_verb(note_kill, true)',
            'msgt(MSG_DESTROY, "The %s %s!", o_name, note_kill);',
            'do_kill = context.do_kill && (obj != protected_obj);',
            'ignore = context.ignore;',
            'note_kill = context.note_kill;',
            'cmd_disable_repeat_floor_item();',
            'square_delete_object(cave, grid, obj, true, true);',
        ):
            self.assertIn(snippet, part)

    def test_all_eight_original_verbs_and_english_fallback(self):
        part = HEADER.split(
            "static inline const char *angdroid_project_object_destroy_verb(",
            1)[1].split(
            "static inline const char *angdroid_project_inventory_scope(",
            1)[0]
        entries = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', part)
        self.assertEqual(list(VERBS), entries)
        self.assertEqual(8, len({en for en, _ in entries}))
        for en, zh in VERBS:
            self.assertIn(
                'verify(angdroid_project_object_destroy_verb, "' + en +
                '", "' + zh + '");', NATIVE)
        self.assertIn("if (!english || !zh_tw) return english;", part)
        self.assertIn("return english;", part)
        self.assertIn("angdroid_project_object_destroy_verb(NULL, 1) == NULL",
                      NATIVE)

    def test_inventory_scope_four_boundaries(self):
        for snippet in (
            'if (total <= 1) return "";',
            'if (affected >= total) return "全部";',
            'if (affected > 1) return "部分";',
            'return "其中一件";',
        ):
            self.assertIn(snippet, HEADER)
        for args in ("1, 1", "4, 4", "4, 2", "4, 1"):
            self.assertIn("angdroid_project_inventory_scope(" + args + ")",
                          NATIVE)

    def test_new_source_and_host_tests_are_registered(self):
        self.assertIn("object_verb_header = projection_header.partition(",
                      CHECKER)
        self.assertIn('msgt(MSG_DESTROY, "你的%s（%c）%s%s！",', CHECKER)
        self.assertIn("tests/test_projection_item_feedback_locale_source.py",
                      VERIFY)
        self.assertIn("test_projection_feedback_locale.c",
                      (ROOT / "scripts/test-native-utf8.sh").read_text(
                          encoding="utf-8"))

if __name__ == "__main__":
    unittest.main()
