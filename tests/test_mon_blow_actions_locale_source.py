#!/usr/bin/env python3
"""Original monster action messages must have exact display-only translations."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "mon-blows.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-mon-blow-actions.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_mon_blow_actions_locale.c").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class MonsterBlowActionsLocaleSourceTest(unittest.TestCase):
    def test_complete_bilingual_canonical_action_table(self):
        keys = set()
        for variant in ("angband", "faangband"):
            txt = (ROOT / "variant_lib" / variant / "gamedata" /
                   "blow_methods.txt").read_text(encoding="utf-8")
            keys.update(line[4:] for line in txt.splitlines()
                        if line.startswith("act:"))
        action_section = HEADER.split(
            "static inline const char *angdroid_mon_blow_action_template(", 1
        )[1].split(
            "static inline const char *angdroid_mon_blow_save_feedback(", 1
        )[0]
        pairs = re.findall(r'\{ "((?:\\.|[^"])*)", "((?:\\.|[^"])*)" \}',
                           action_section)
        self.assertEqual(65, len(pairs))
        self.assertEqual(keys, {en for en, _ in pairs})
        self.assertEqual(65, len({en for en, _ in pairs}))
        for en, zh in pairs:
            with self.subTest(en=en):
                self.assertTrue(zh)
                self.assertNotIn("%", zh)
                self.assertIn(
                    'verify(angdroid_mon_blow_action_template, "' + en +
                    '", "' + zh + '");', NATIVE)

    def test_original_random_selection_and_public_api_unchanged(self):
        for token in (
            "int choice = randint0(method->num_messages);",
            "const struct blow_message *msg = method->messages;",
            "while (choice--)",
            "blow_tag_lookup(tag)",
            "monster_desc(m_name,",
            "MDESC_TARG | MDESC_POSS",
            "return string_make(buf);",
            "char *monster_blow_method_action(const struct blow_method *method, int midx)",
            "return monster_blow_method_action_render(method, midx, NULL);",
            "in_cursor = angdroid_mon_blow_action_template(msg->act_msg,",
            "if (translated) *translated = zh_action;",
            'zh_action ? "你" : "you"',
            'zh_action ? "你的" : "your"',
            'if (!zh_action && !strchr(punct, *in_cursor))',
        ):
            self.assertIn(token, SOURCE)

    def test_player_and_monster_sound_damage_and_punctuation(self):
        for token in (
            'msgt(method->msgt, "%s%s%s（%d）", m_name, act,',
            'msgt(method->msgt, "%s %s%s (%d)", m_name, act,',
            'msgt(method->msgt, "%s%s%s", m_name, act, fullstop);',
            'msgt(method->msgt, "%s %s%s", m_name, act, fullstop);',
            'suffix(act, "！")',
            'suffix(act, "？")',
            'suffix(act, "。")',
            'suffix(act, "」")',
            'string_free(act);',
        ):
            self.assertIn(token, SOURCE)

    def test_saving_throws_and_unknown_english_fallback(self):
        for token in (
            'OF_PROT_FEAR, true, "You stand your ground!"',
            'OF_FREE_ACT, true, "You resist the effects!"',
            'msg("%s", angdroid_mon_blow_save_feedback(save_msg,',
            'if (!english || !zh_tw) return english;',
            'return english;',
        ):
            self.assertIn(token, SOURCE + HEADER)
        for key, zh in (
            ("You stand your ground!", "你堅定地站穩腳步！"),
            ("You resist the effects!", "你抵抗了這次效果！"),
        ):
            self.assertIn(
                'verify(angdroid_mon_blow_save_feedback, "' + key +
                '", "' + zh + '");', NATIVE)
        self.assertIn('angdroid_mon_blow_save_feedback(NULL, 1) == NULL', NATIVE)
        self.assertIn('angdroid_mon_blow_action_template(NULL, 1) == NULL', NATIVE)

    def test_offline_registration_and_checker(self):
        self.assertIn("test_mon_blow_actions_locale.c", RUNNER)
        self.assertIn("tests/test_mon_blow_actions_locale_source.py", VERIFY)
        self.assertIn("len(action_pairs) == 65", CHECKER)
        self.assertIn("canonical_action_keys = set()", CHECKER)


if __name__ == "__main__":
    unittest.main()
