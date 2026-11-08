#!/usr/bin/env python3
"""Monster spell template tags and CJK text must remain display-only."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SPELL = (CPP / "mon-spell.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-mon-spell-render.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_mon_spell_render_utf8.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(
    encoding="utf-8")


class MonsterSpellRenderSourceTest(unittest.TestCase):
    def test_exact_length_tag_match_no_partial_aliases(self):
        self.assertIn('spell_tag_lookup(const char *tag, size_t length)', SPELL)
        self.assertIn('spell_tag_lookup(tag, (size_t)(s - tag))', SPELL)
        for name in ("name", "pronoun", "target", "type", "oftype"):
            self.assertIn(
                f'angdroid_mon_spell_tag_matches(tag, length, "{name}")', SPELL)
            self.assertIn(f'"{name}"', NATIVE)
        self.assertIn("strlen(known) == bytes", HEADER)
        self.assertIn("memcmp(tag, known, bytes) == 0", HEADER)
        for bad in ("named", "pronouns", "targeted", "typed", "oftypes"):
            self.assertIn(f'"{bad}"', NATIVE)
        self.assertNotIn('strncmp(tag, "target", 6)', SPELL)

    def test_chinese_path_uses_complete_scalar_appends(self):
        self.assertIn('#include "zh-tw-mon-spell-render.h"', SPELL)
        self.assertIn("static inline size_t angdroid_mon_spell_append_utf8(",
                      HEADER)
        self.assertIn("angdroid_utf8_next_scalar(source + offset, &scalar)",
                      HEADER)
        self.assertIn("step > bytes - offset", HEADER)
        self.assertIn("step > capacity - used - 1", HEADER)
        self.assertIn("dst[used++] = '?';", HEADER)
        self.assertGreaterEqual(
            SPELL.count("angdroid_mon_spell_append_utf8("), 9)
        self.assertIn("in_cursor, (size_t)(next - in_cursor));", SPELL)
        self.assertIn("in_cursor, strlen(in_cursor));", SPELL)
        self.assertIn('"（", strlen("（")', SPELL)
        self.assertIn('"）", strlen("）")', SPELL)
        self.assertIn("if (zh_spell)", SPELL)

    def test_english_and_spell_combat_path_not_changed(self):
        for source in (
            'strnfcat(buf, 1024, &end, "%.*s",',
            'strnfcat(buf, 1024, &end, "%s", in_cursor);',
            'mdesc_mode |= MDESC_CAPITAL;',
            'mdesc_mode |= MDESC_COMMA;',
            'angdroid_mon_spell_lash(type_name, true)',
            'msgt(spell->msgt, "%s", buf);',
            'spell_check_for_fail_rune(spell);',
            'find_alternate_spell_message(mon->race,',
            'monster_desc(m_name, sizeof(m_name),',
            'bool zh_spell = angdroid_zh_tw_enabled();',
        ):
            with self.subTest(source=source):
                self.assertIn(source, SPELL)

    def test_native_utf8_and_tag_boundary_cases(self):
        for token in (
            '"龍龍龍龍"', '"怪物{name}攻擊"', '"?X"',
            '"name} tail"', '"targeted"', '"oftypes"',
            'angdroid_mon_spell_append_utf8(NULL, 0, 0,',
            'angdroid_mon_spell_tag_matches(NULL, 4,',
        ):
            self.assertIn(token, NATIVE)

    def test_offline_test_and_checker_registered(self):
        self.assertIn("tests/native/test_mon_spell_render_utf8.c", RUNNER)
        self.assertIn("tests/test_mon_spell_render_utf8_source.py", VERIFY)
        self.assertIn("Monster spell tags require exact length", CHECKER)


if __name__ == "__main__":
    unittest.main()
